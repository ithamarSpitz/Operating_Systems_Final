#ifndef MST_ALGORITHM_HPP
#define MST_ALGORITHM_HPP

#include "Graph.hpp"
#include "MST.hpp"

class MSTAlgorithm {
public:
    virtual MST solve(const Graph& graph) = 0;
    virtual ~MSTAlgorithm() = default;
};

class BoruvkaAlgorithm : public MSTAlgorithm {
public:
    MST solve(const Graph& graph) override;
};

class PrimAlgorithm : public MSTAlgorithm {
public:
    MST solve(const Graph& graph) override;
};

class KruskalAlgorithm : public MSTAlgorithm {
public:
    MST solve(const Graph& graph) override;
};

class TarjanAlgorithm : public MSTAlgorithm {
public:
    MST solve(const Graph& graph) override;
};

class IntegerMSTAlgorithm : public MSTAlgorithm {
public:
    MST solve(const Graph& graph) override;
};

#endif // MST_ALGORITHM_HPP