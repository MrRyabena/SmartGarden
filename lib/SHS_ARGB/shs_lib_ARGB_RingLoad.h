#pragma once

#include <functional>

#include "shs_lib_ARGB_LoadingEffect.h"

namespace shs
{
    namespace argb
    {
        class RingLoad;
    }
}


class shs::argb::RingLoad : public shs::argb::LoadingRingEffect
{
public:
    explicit RingLoad(CRGB* leds, uint16_t num_leds, std::function<void()> on_change_callback = nullptr);
};
