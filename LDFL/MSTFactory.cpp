#include "MSTFactory.hpp"
#include "MSTAlgorithm.hpp"
#include <stdexcept>
#include <memory>

std::unique_ptr<MSTAlgorithm> MSTFactory::createAlgorithm(const std::string& algorithmName) {
    if (algorithmName == "Boruvka") {
        return std::make_unique<BoruvkaAlgorithm>();
    } else if (algorithmName == "Prim") {
        return std::make_unique<PrimAlgorithm>();
    } else if (algorithmName == "Kruskal") {
        return std::make_unique<KruskalAlgorithm>();
    } else if (algorithmName == "Tarjan") {
        return std::make_unique<TarjanAlgorithm>();
    } else if (algorithmName == "Integer") {
        return std::make_unique<IntegerMSTAlgorithm>();
    } else {
        throw std::invalid_argument("Unknown algorithm: " + algorithmName);
    }
}