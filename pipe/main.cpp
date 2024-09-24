#include "Server.hpp"
#include <csignal>
#include <atomic>
#include <iostream>

std::atomic<bool> running(true);

void signal_handler(int signal) {
    std::cout << "Received signal " << signal << std::endl;
    running = false;
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    Server server;
    
    std::thread server_thread([&server]() {
        server.run();
    });

    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Shutting down server..." << std::endl;
    server.stop();
    
    if (server_thread.joinable()) {
        server_thread.join();
    }

    std::cout << "Server shut down completely." << std::endl;
    return 0;
}