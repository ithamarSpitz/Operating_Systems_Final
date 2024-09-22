#include "MST.hpp"
#include <algorithm>
#include <limits>
#include <queue>

MST::MST(int n) : n(n), edges(n), distances(n, std::vector<int>(n, std::numeric_limits<int>::max())) {
    for (int i = 0; i < n; ++i) {
        distances[i][i] = 0;
    }
}

void MST::addEdge(int from, int to, int weight) {
    edges[from].push_back({to, weight});
    edges[to].push_back({from, weight});
}

int MST::getTotalWeight() const {
    int total = 0;
    for (int i = 0; i < n; ++i) {
        for (const auto& edge : edges[i]) {
            total += edge.second;
        }
    }
    return total / 2; // Each edge is counted twice
}

void MST::calculateDistances() {
    for (int start = 0; start < n; ++start) {
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;
        pq.push({0, start});
        
        while (!pq.empty()) {
            int dist = pq.top().first;
            int v = pq.top().second;
            pq.pop();
            
            if (dist > distances[start][v]) continue;
            
            for (const auto& edge : edges[v]) {
                int to = edge.first;
                int weight = edge.second;
                if (distances[start][v] + weight < distances[start][to]) {
                    distances[start][to] = distances[start][v] + weight;
                    pq.push({distances[start][to], to});
                }
            }
        }
    }
}

int MST::getLongestDistance() const {
    int maxDist = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (distances[i][j] != std::numeric_limits<int>::max()) {
                maxDist = std::max(maxDist, distances[i][j]);
            }
        }
    }
    return maxDist;
}

double MST::getAverageDistance() const {
    long long sum = 0;
    int count = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = i; j < n; ++j) {
            if (distances[i][j] != std::numeric_limits<int>::max()) {
                sum += distances[i][j];
                count++;
            }
        }
    }
    return static_cast<double>(sum) / count;
}

int MST::getShortestDistance() const {
    int minDist = std::numeric_limits<int>::max();
    for (int i = 0; i < n; ++i) {
        for (const auto& edge : edges[i]) {
            minDist = std::min(minDist, edge.second);
        }
    }
    return minDist;
}