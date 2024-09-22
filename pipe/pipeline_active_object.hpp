#ifndef PIPELINE_ACTIVE_OBJECT_HPP
#define PIPELINE_ACTIVE_OBJECT_HPP

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector>
#include <atomic>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include "Graph.hpp"
#include "MSTFactory.hpp"

class ActiveObject {
public:
    using Task = std::function<void()>;

    ActiveObject() : stop(false) {
        worker = std::thread(&ActiveObject::run, this);
    }

    ~ActiveObject() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            stop = true;
        }
        condition.notify_one();
        if (worker.joinable()) {
            worker.join();
        }
    }

    void enqueue(Task task) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            tasks.push(std::move(task));
        }
        condition.notify_one();
    }

private:
    void run() {
        while (true) {
            Task task;
            {
                std::unique_lock<std::mutex> lock(mutex);
                condition.wait(lock, [this] { return stop || !tasks.empty(); });
                if (stop && tasks.empty()) return;
                task = std::move(tasks.front());
                tasks.pop();
            }
            task();
        }
    }

    std::queue<Task> tasks;
    std::thread worker;
    std::mutex mutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
};

class Pipeline {
public:
    Pipeline(int listenerSocket) 
        : listenerSocket(listenerSocket), 
          stop(false),
          acceptor(std::make_unique<ActiveObject>()),
          parser(std::make_unique<ActiveObject>()),
          executor(std::make_unique<ActiveObject>()),
          responder(std::make_unique<ActiveObject>()) {}

    ~Pipeline() {
        stop = true;
        // Optionally, add code here to ensure all ActiveObjects have finished their tasks
    }

    void start() {
        acceptor->enqueue([this] { acceptConnections(); });
    }

    void stopPipeline() {
        stop = true;
        // Add code to wake up the acceptor thread if it's blocked on accept()
        // For example, you could use a self-pipe trick or close the listener socket
    }

private:
    void acceptConnections() {
        while (!stop) {
            sockaddr_storage remoteaddr;
            socklen_t addrlen = sizeof remoteaddr;
            int clientfd = accept(listenerSocket, (struct sockaddr *)&remoteaddr, &addrlen);
            if (clientfd == -1) {
                if (errno != EINTR) {  // EINTR could be caused by signal interruption during shutdown
                    perror("accept");
                }
                continue;
            }
            std::cout << "New connection accepted\n";
            parser->enqueue([this, clientfd] { readAndParse(clientfd); });
        }
    }

    void readAndParse(int clientfd) {
        char buf[1024];
        std::string buffer;
        while (!stop) {
            int nbytes = recv(clientfd, buf, sizeof buf, 0);
            if (nbytes <= 0 || stop) {
                if (nbytes == 0) std::cout << "Socket " << clientfd << " hung up\n";
                else if (!stop) perror("recv");
                break;
            }
            buffer.append(buf, nbytes);
            size_t pos;
            while ((pos = buffer.find('\n')) != std::string::npos) {
                std::string command = buffer.substr(0, pos);
                buffer.erase(0, pos + 1);
                std::vector<std::string> parsedCommand = graph.parse(command);
                executor->enqueue([this, clientfd, parsedCommand] { executeCommand(clientfd, parsedCommand); });
            }
        }
        close(clientfd);
    }

    void executeCommand(int clientfd, const std::vector<std::string>& command) {
        bool result;
        {
            std::lock_guard<std::mutex> lock(graph_mutex);
            result = graph.eval(command);
        }
        responder->enqueue([this, clientfd, result] { sendResponse(clientfd, result); });
    }

    void sendResponse(int clientfd, bool result) {
        std::string response = result ? "Command processed successfully\n" : "Command processing failed\n";
        send(clientfd, response.c_str(), response.length(), 0);
        std::cout << "Client " << clientfd << " - Sent response: " << response;
    }

    int listenerSocket;
    std::atomic<bool> stop;
    std::unique_ptr<ActiveObject> acceptor;
    std::unique_ptr<ActiveObject> parser;
    std::unique_ptr<ActiveObject> executor;
    std::unique_ptr<ActiveObject> responder;
    Graph graph;
    std::mutex graph_mutex;
};

#endif // PIPELINE_ACTIVE_OBJECT_HPP