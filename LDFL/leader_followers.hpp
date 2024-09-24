#ifndef LEADER_FOLLOWERS_HPP
#define LEADER_FOLLOWERS_HPP

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
#include <sstream>
#include "MSTFactory.hpp"

class LeaderFollowersThreadPool {
public:
    using Task = std::function<void()>;

    LeaderFollowersThreadPool(size_t numThreads, int listenerSocket)
        : stop(false), leader(std::thread::id()), listenerSocket(listenerSocket) {
        for (size_t i = 0; i < numThreads; ++i) {
            threads.emplace_back(&LeaderFollowersThreadPool::workerThread, this);
        }
    }

    ~LeaderFollowersThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker : threads) {
            worker.join();
        }
    }

    void enqueue(Task task) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.push(std::move(task));
        }
        condition.notify_one();
    }

private:
    void workerThread() {
        while (true) {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return stop || !tasks.empty() || leader == std::thread::id(); });

            if (stop && tasks.empty()) {
                return;
            }

            if (leader == std::thread::id()) {
                leader = std::this_thread::get_id();
                lock.unlock();

                int newfd = acceptConnection();
                if (newfd != -1) {
                    enqueue([this, newfd] { handleClient(newfd); });
                }

                lock.lock();
                if (leader == std::this_thread::get_id()) {
                    leader = std::thread::id();
                }
            } else if (!tasks.empty()) {
                Task task = std::move(tasks.front());
                tasks.pop();
                lock.unlock();

                task();
            }
        }
    }

    int acceptConnection() {
        sockaddr_storage remoteaddr;
        socklen_t addrlen = sizeof remoteaddr;
        int newfd = accept(listenerSocket, (struct sockaddr *)&remoteaddr, &addrlen);
        if (newfd == -1) {
            perror("accept");
        }
        return newfd;
    }

    void handleClient(int fd) {
        std::string buffer;
        char buf[256];
        while (true) {
            int nbytes = recv(fd, buf, sizeof buf, 0);
            if (nbytes <= 0) {
                if (nbytes == 0) {
                    std::cout << "Socket " << fd << " hung up\n";
                } else {
                    perror("recv");
                }
                close(fd);
                return;
            }
            buffer.append(buf, nbytes);
            size_t pos;
            while ((pos = buffer.find('\n')) != std::string::npos) {
                std::string command = buffer.substr(0, pos);
                buffer.erase(0, pos + 1);
                std::cout << "Client " << fd << " - Received command: " << command << std::endl;
                std::vector<std::string> data = g.parse(command);
                std::lock_guard<std::mutex> lock(graph_mutex);
                std::string result;
                bool success = g.eval(data);
                if (success) {
                    result = "Command processed successfully\n";
                    if (data[0] == "RunMST") {
                        const std::string& algorithm = data[1];
                        try {
                            auto mstAlgorithm = MSTFactory::createAlgorithm(algorithm);
                            MST mst = mstAlgorithm->solve(g);
                            mst.calculateDistances();
                            std::ostringstream oss;
                            oss << "MST total weight: " << mst.getTotalWeight() << "\n";
                            oss << "Longest distance: " << mst.getLongestDistance() << "\n";
                            oss << "Average distance: " << mst.getAverageDistance() << "\n";
                            oss << "Shortest distance: " << mst.getShortestDistance() << "\n";
                            result += oss.str();
                        } catch (const std::exception& e) {
                            result = "Error running MST algorithm: " + std::string(e.what()) + "\n";
                            success = false;
                        }
                    }
                } else {
                    result = "Command processing failed\n";
                }
                send(fd, result.c_str(), result.length(), 0);
                
                std::cout << "Client " << fd << " - Sent response: " << result;
                if (!success) std::cerr << "exit" << std::endl;
            }
        }
    }

    std::vector<std::thread> threads;
    std::queue<Task> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
    std::atomic<std::thread::id> leader;
    int listenerSocket;

    Graph g;
    std::mutex graph_mutex;
};

#endif // LEADER_FOLLOWERS_HPP