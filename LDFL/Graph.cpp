#include "Graph.hpp"
#include <sstream>
#include <algorithm>
#include <iostream>
#include "MSTFactory.hpp"

Graph::Graph() : n(0), m(0) {}

void Graph::NewGraph(int n, int m) {
    this->n = n;
    this->m = m;
    this->adj.assign(n, std::vector<std::pair<int, int>>());
}

void Graph::NewEdge(int i, int j, int weight) {
    if (i > n || j > n) return;
    adj[i - 1].push_back({j - 1, weight});
}

void Graph::RemoveEdge(int i, int j) {
    auto& edges = adj[i - 1];
    edges.erase(std::remove_if(edges.begin(), edges.end(),
                               [j](const std::pair<int, int>& e) { return e.first == j - 1; }),
                edges.end());
}

std::vector<std::string> Graph::parse(const std::string& command) {
    std::vector<std::string> parts;
    std::istringstream iss(command);
    std::string part;
    
    while (iss >> part) {
        parts.push_back(part);
    }

    if (parts.size() > 1 && parts[0] != "NewGraph") {
        std::istringstream iss_args(parts[1]);
        std::vector<std::string> args;
        while (std::getline(iss_args, part, ',')) {
            args.push_back(part);
        }
        parts.erase(parts.begin() + 1);
        parts.insert(parts.end(), args.begin(), args.end());
    }

    return parts;
}

bool Graph::eval(const std::vector<std::string>& parts) {
    if (parts.empty()) return true;

    const std::string& cmd = parts[0];

    if (cmd == "NewGraph") {
        if (parts.size() < 3) return false;
        int n = std::stoi(parts[1]);
        int m = std::stoi(parts[2]);
        if (n < 1 || m < 1) return false;
        NewGraph(n, m);
        return evalEdges(parts);
    } else if (cmd == "NewEdge") {
        if (parts.size() != 4) return false;
        int i = std::stoi(parts[1]);
        int j = std::stoi(parts[2]);
        int weight = std::stoi(parts[3]);
        if (i > n || i < 1 || j > n || j < 1) return false;
        NewEdge(i, j, weight);
    } else if (cmd == "RemoveEdge") {
        if (parts.size() != 3) return false;
        int i = std::stoi(parts[1]);
        int j = std::stoi(parts[2]);
        if (i > n || i < 1 || j > n || j < 1) return false;
        RemoveEdge(i, j);
    } else if (cmd == "RunMST") {
        if (parts.size() != 2) return false;
        const std::string& algorithm = parts[1];
        try {
            auto mstAlgorithm = MSTFactory::createAlgorithm(algorithm);
            MST mst = mstAlgorithm->solve(*this);
            // Print or return MST results
            std::cout << "MST total weight: " << mst.getTotalWeight() << std::endl;
            // Add more output as needed
        } catch (const std::exception& e) {
            std::cerr << "Error running MST algorithm: " << e.what() << std::endl;
            return false;
        }
    } else {
        return false;
    }

    return true;
}

bool Graph::evalEdges(const std::vector<std::string>& parts) {
    for (size_t i = 3; i < parts.size(); ++i) {
        std::istringstream iss_edge(parts[i]);
        std::string edge;
        std::getline(iss_edge, edge, ',');
        int from = std::stoi(edge);
        std::getline(iss_edge, edge, ',');
        int to = std::stoi(edge);
        std::getline(iss_edge, edge, ',');
        int weight = std::stoi(edge);
        if (from > n || from < 1 || to > n || to < 1) return false;
        NewEdge(from, to, weight);
    }
    return true;
}