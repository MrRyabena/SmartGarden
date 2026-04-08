#pragma once

#include <shs_types.h>

namespace shs
{
    struct SmartGardenData;
}

struct shs::SmartGardenData
{
    shs::t::shs_fixed_t soil_moisture_percent;
    shs::t::shs_fixed_t light_level_percent;
    shs::t::shs_fixed_t temperature_celsius;
    shs::t::shs_fixed_t humidity_percent;
};