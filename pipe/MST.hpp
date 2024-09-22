#ifndef MST_HPP
#define MST_HPP

#include <vector>

class MST {
public:
    MST(int n);
    void addEdge(int from, int to, int weight);
    int getTotalWeight() const;
    int getLongestDistance() const;
    double getAverageDistance() const;
    int getShortestDistance() const;

private:
    int n;
    std::vector<std::vector<std::pair<int, int>>> edges; // (to, weight)
    std::vector<std::vector<int>> distances;
    void calculateDistances();
};

#endif // MST_HPP