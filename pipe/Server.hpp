#ifndef SERVER_HPP
#define SERVER_HPP

#include "pipeline_active_object.hpp"
#include <atomic>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

class Server {
public:
    Server() : pipeline(nullptr), listener(-1), running(false) {}
    ~Server() {
        stop();
    }

    void run() {
        listener = setup_listener();
        if (listener == -1) {
            std::cerr << "Failed to setup listener\n";
            return;
        }

        pipeline = std::make_unique<Pipeline>(listener);
        pipeline->start();
        std::cout << "Server running on port " << PORT << std::endl;

        running = true;
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void stop() {
        running = false;
        if (pipeline) {
            pipeline->stopPipeline();
            pipeline.reset();
        }
        if (listener != -1) {
            close(listener);
            listener = -1;
        }
    }

private:
    static constexpr const char* PORT = "9034";
    std::unique_ptr<Pipeline> pipeline;
    int listener;
    std::atomic<bool> running;

    static void signal_handler(int signal) {
        std::cout << "Received signal " << signal << std::endl;
        // Use a static instance to call the non-static member function
        static_cast<Server*>(nullptr)->stop();
    }

    static void* get_in_addr(struct sockaddr *sa) {
        if (sa->sa_family == AF_INET) {
            return &(((struct sockaddr_in*)sa)->sin_addr);
        }
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }

    int setup_listener() {
        struct addrinfo hints{}, *ai, *p;
        int listener;
        int yes = 1;
        int rv;

        std::memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        if ((rv = getaddrinfo(nullptr, PORT, &hints, &ai)) != 0) {
            std::cerr << "getaddrinfo: " << gai_strerror(rv) << '\n';
            return -1;
        }
    
        for(p = ai; p != nullptr; p = p->ai_next) {
            listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
            if (listener < 0) continue;
        
            setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
            if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
                close(listener);
                continue;
            }
            break;
        }

        freeaddrinfo(ai);

        if (p == nullptr) {
            std::cerr << "Failed to bind\n";
            return -1;
        }

        if (listen(listener, 10) == -1) {
            perror("listen");
            return -1;
        }

        return listener;
    }
};

#endif // SERVER_HPP