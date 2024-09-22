#include "Server.hpp"
#include <iostream>

// Initialize the static member outside the class
std::atomic<bool> Server::stop{false};
// int main(int argc, char* argv[]) {
//     try {
//         size_t numThreads = std::thread::hardware_concurrency();
//         if (argc > 1) {
//             numThreads = std::stoul(argv[1]);
//         }

//         Server server(numThreads);
//         server.run();
//     } catch (const std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return 1;
//     }

//     return 0;
// }

int main() {
    Server server;
    server.run();
    return 0;
}