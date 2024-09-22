#include "pipeline_active_object.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <memory>
#include <mutex>

class Server {
public:
    Server() {}

    void run() {
        int listener = setup_listener();
        if (listener == -1) {
            std::cerr << "Failed to setup listener\n";
            return;
        }

        Pipeline pipeline(listener);
        pipeline.start();

        std::cout << "Server running on port " << PORT << std::endl;

        // Wait for 'q' input to quit
        while (std::cin.get() != 'q') {}

        close(listener);
    }

private:
    static constexpr const char* PORT = "9034";
    size_t numThreads;

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