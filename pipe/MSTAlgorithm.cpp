#include "MSTAlgorithm.hpp"
#include <algorithm>
#include <queue>
#include <limits>

// Boruvka's Algorithm
MST BoruvkaAlgorithm::solve(const Graph& graph) {
    int n = graph.getNumVertices();
    MST mst(n);
    std::vector<int> components(n);
    for (int i = 0; i < n; ++i) components[i] = i;

    auto find = [&](int x) {
        while (x != components[x]) {
            components[x] = components[components[x]];
            x = components[x];
        }
        return x;
    };

    auto merge = [&](int x, int y) {
        x = find(x);
        y = find(y);
        if (x != y) components[x] = y;
    };

    bool changed;
    do {
        changed = false;
        std::vector<std::pair<int, std::pair<int, int>>> cheapest(n, {std::numeric_limits<int>::max(), {-1, -1}});

        for (int i = 0; i < n; ++i) {
            for (const auto& edge : graph.getAdjList()[i]) {
                int j = edge.first;
                int weight = edge.second;
                int ci = find(i);
                int cj = find(j);
                if (ci != cj) {
                    if (weight < cheapest[ci].first) cheapest[ci] = {weight, {i, j}};
                    if (weight < cheapest[cj].first) cheapest[cj] = {weight, {i, j}};
                }
            }
        }

        for (int i = 0; i < n; ++i) {
            if (cheapest[i].second.first != -1) {
                int u = cheapest[i].second.first;
                int v = cheapest[i].second.second;
                if (find(u) != find(v)) {
                    mst.addEdge(u, v, cheapest[i].first);
                    merge(u, v);
                    changed = true;
                }
            }
        }
    } while (changed);

    return mst;
}

// Prim's Algorithm
MST PrimAlgorithm::solve(const Graph& graph) {
    int n = graph.getNumVertices();
    MST mst(n);
    std::vector<bool> visited(n, false);
    std::priority_queue<std::pair<int, std::pair<int, int>>, 
                        std::vector<std::pair<int, std::pair<int, int>>>, 
                        std::greater<std::pair<int, std::pair<int, int>>>> pq;

    pq.push({0, {0, -1}});

    while (!pq.empty()) {
        int u = pq.top().second.first;
        int parent = pq.top().second.second;
        int weight = pq.top().first;
        pq.pop();

        if (visited[u]) continue;
        visited[u] = true;

        if (parent != -1) {
            mst.addEdge(parent, u, weight);
        }

        for (const auto& edge : graph.getAdjList()[u]) {
            int v = edge.first;
            int w = edge.second;
            if (!visited[v]) {
                pq.push({w, {v, u}});
            }
        }
    }

    return mst;
}

// Kruskal's Algorithm
MST KruskalAlgorithm::solve(const Graph& graph) {
    int n = graph.getNumVertices();
    MST mst(n);
    std::vector<std::pair<int, std::pair<int, int>>> edges;
    
    for (int i = 0; i < n; ++i) {
        for (const auto& edge : graph.getAdjList()[i]) {
            int j = edge.first;
            int weight = edge.second;
            if (i < j) {  // Avoid duplicates
                edges.push_back({weight, {i, j}});
            }
        }
    }
    
    std::sort(edges.begin(), edges.end());
    
    std::vector<int> parent(n);
    for (int i = 0; i < n; ++i) parent[i] = i;
    
    struct FindUnion {
        std::vector<int>& parent;
        
        FindUnion(std::vector<int>& p) : parent(p) {}
        
        int find(int x) {
            if (parent[x] != x) parent[x] = find(parent[x]);
            return parent[x];
        }
        
        void unite(int x, int y) {
            x = find(x);
            y = find(y);
            if (x != y) parent[x] = y;
        }
    };
    
    FindUnion fu(parent);
    
    for (const auto& edge : edges) {
        int weight = edge.first;
        int u = edge.second.first;
        int v = edge.second.second;
        
        if (fu.find(u) != fu.find(v)) {
            fu.unite(u, v);
            mst.addEdge(u, v, weight);
        }
    }
    
    return mst;
}

// Tarjan's Algorithm
// Note: This is a simplified version that doesn't implement the full Tarjan's algorithm
// It uses a combination of Kruskal's and Union-Find data structure
MST TarjanAlgorithm::solve(const Graph& graph) {
    int n = graph.getNumVertices();
    MST mst(n);
    std::vector<std::pair<int, std::pair<int, int>>> edges;
    
    for (int i = 0; i < n; ++i) {
        for (const auto& edge : graph.getAdjList()[i]) {
            int j = edge.first;
            int weight = edge.second;
            if (i < j) {  // Avoid duplicates
                edges.push_back({weight, {i, j}});
            }
        }
    }
    
    std::sort(edges.begin(), edges.end());
    
    class UnionFind {
    private:
        std::vector<int> parent, rank;
    
    public:
        UnionFind(int n) : parent(n), rank(n, 0) {
            for (int i = 0; i < n; ++i) parent[i] = i;
        }
        
        int find(int x) {
            if (parent[x] != x) parent[x] = find(parent[x]);
            return parent[x];
        }
        
        bool unite(int x, int y) {
            x = find(x);
            y = find(y);
            if (x == y) return false;
            if (rank[x] < rank[y]) std::swap(x, y);
            parent[y] = x;
            if (rank[x] == rank[y]) ++rank[x];
            return true;
        }
    };
    
    UnionFind uf(n);
    
    for (const auto& edge : edges) {
        int weight = edge.first;
        int u = edge.second.first;
        int v = edge.second.second;
        
        if (uf.unite(u, v)) {
            mst.addEdge(u, v, weight);
        }
    }
    
    return mst;
}

// Integer MST Algorithm
// Note: This is a simplified version that assumes all weights are integers
// It uses counting sort to achieve linear time complexity
MST IntegerMSTAlgorithm::solve(const Graph& graph) {
    int n = graph.getNumVertices();
    MST mst(n);
    
    // Find the maximum weight
    int max_weight = 0;
    for (int i = 0; i < n; ++i) {
        for (const auto& edge : graph.getAdjList()[i]) {
            max_weight = std::max(max_weight, edge.second);
        }
    }
    
    // Counting sort
    std::vector<std::vector<std::pair<int, int>>> count(max_weight + 1);
    for (int i = 0; i < n; ++i) {
        for (const auto& edge : graph.getAdjList()[i]) {
            int j = edge.first;
            int weight = edge.second;
            if (i < j) {  // Avoid duplicates
                count[weight].push_back({i, j});
            }
        }
    }
    
    class UnionFind {
    private:
        std::vector<int> parent, rank;
    
    public:
        UnionFind(int n) : parent(n), rank(n, 0) {
            for (int i = 0; i < n; ++i) parent[i] = i;
        }
        
        int find(int x) {
            if (parent[x] != x) parent[x] = find(parent[x]);
            return parent[x];
        }
        
        bool unite(int x, int y) {
            x = find(x);
            y = find(y);
            if (x == y) return false;
            if (rank[x] < rank[y]) std::swap(x, y);
            parent[y] = x;
            if (rank[x] == rank[y]) ++rank[x];
            return true;
        }
    };
    
    UnionFind uf(n);
    
    for (int weight = 0; weight <= max_weight; ++weight) {
        for (const auto& edge : count[weight]) {
            int u = edge.first;
            int v = edge.second;
            
            if (uf.unite(u, v)) {
                mst.addEdge(u, v, weight);
            }
        }
    }
    
    return mst;
}