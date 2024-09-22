#ifndef MST_FACTORY_HPP
#define MST_FACTORY_HPP

#include "MSTAlgorithm.hpp"
#include <memory>
#include <string>

class MSTFactory {
public:
    static std::unique_ptr<MSTAlgorithm> createAlgorithm(const std::string& name);
};

#endif // MST_FACTORY_HPP