#ifndef MST_HPP
#define MST_HPP

#include <vector>
#include <queue>
#include <limits>

class MST {
public:
    MST(int n) : n(n), edges(n), distances(n, std::vector<int>(n, std::numeric_limits<int>::max())) {
        for (int i = 0; i < n; ++i) {
            distances[i][i] = 0;
        }
    }

    void addEdge(int from, int to, int weight) {
        edges[from].push_back({to, weight});
        edges[to].push_back({from, weight});
    }

    int getTotalWeight() const {
        int total = 0;
        for (int i = 0; i < n; ++i) {
            for (const auto& edge : edges[i]) {
                total += edge.second;
            }
        }
        return total / 2; // Each edge is counted twice
    }

    void calculateDistances() {
        for (int start = 0; start < n; ++start) {
            std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;
            std::vector<int> dist(n, std::numeric_limits<int>::max());
            pq.push({0, start});
            dist[start] = 0;
            
            while (!pq.empty()) {
                int d = pq.top().first;
                int v = pq.top().second;
                pq.pop();
                
                if (d > dist[v]) continue;
                
                for (const auto& edge : edges[v]) {
                    int to = edge.first;
                    int weight = edge.second;
                    if (dist[v] + weight < dist[to]) {
                        dist[to] = dist[v] + weight;
                        pq.push({dist[to], to});
                    }
                }
            }
            
            for (int i = 0; i < n; ++i) {
                distances[start][i] = dist[i];
            }
        }
    }

    int getLongestDistance() const {
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

    double getAverageDistance() const {
        long long sum = 0;
        int count = 0;
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (distances[i][j] != std::numeric_limits<int>::max()) {
                    sum += distances[i][j];
                    count++;
                }
            }
        }
        return count > 0 ? static_cast<double>(sum) / count : 0.0;
    }

    int getShortestDistance() const {
        int minDist = std::numeric_limits<int>::max();
        for (int i = 0; i < n; ++i) {
            for (const auto& edge : edges[i]) {
                minDist = std::min(minDist, edge.second);
            }
        }
        return minDist;
    }

private:
    int n;
    std::vector<std::vector<std::pair<int, int>>> edges; // (to, weight)
    std::vector<std::vector<int>> distances;
};

#endif // MST_HPP