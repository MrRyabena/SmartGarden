#include "shs_lib_ARGB_RingLoad.h"

shs::argb::RingLoad::RingLoad(CRGB* leds, const uint16_t num_leds, std::function<void()> on_change_callback)
    : LoadingRingEffect(leds, num_leds, on_change_callback, LoadingRingEffect::Config{}, Effects::RING_LOAD)
{}
