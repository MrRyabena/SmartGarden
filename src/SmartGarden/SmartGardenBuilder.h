#pragma once

#include <memory>

namespace shs
{
    class SmartGarden;
    class SmartGardenConfig;
    class SmartGardenBuilder;
}


class shs::SmartGardenBuilder
{
public:
    static std::shared_ptr<shs::SmartGarden> build(const SmartGardenConfig& config);
};
