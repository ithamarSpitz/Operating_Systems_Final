#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <string>

class Graph {
public:
    Graph();
    void NewGraph(int n, int m);
    void NewEdge(int i, int j, int weight);
    void RemoveEdge(int i, int j);
    std::vector<std::string> parse(const std::string& command);
    bool eval(const std::vector<std::string>& parts);

    int getNumVertices() const { return n; }
    const std::vector<std::vector<std::pair<int, int>>>& getAdjList() const { return adj; }

private:
    int n; // Number of vertices
    int m; // Number of arcs
    std::vector<std::vector<std::pair<int, int>>> adj; // Adjacency list (vertex, weight)
    bool evalEdges(const std::vector<std::string>& parts);
};

#endif // GRAPH_HPP