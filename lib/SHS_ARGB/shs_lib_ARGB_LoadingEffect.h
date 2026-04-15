#pragma once

#include <stdint.h>
#include <functional>

#include <FastLED.h>
#include <shs_types.h>

#include "shs_lib_ARGB_Effect.h"

namespace shs
{
    namespace argb
    {
        class LoadingRingEffect;
    }
}

class shs::argb::LoadingRingEffect : public shs::argb::Effect
{
public:
    struct Config
    {
        CRGB color = CRGB::Blue;
        uint8_t tail_length = 3;
        uint16_t step_ms = 90;
        bool clockwise = true;
        uint8_t tail_falloff = 85;
    };

    explicit LoadingRingEffect(CRGB* leds,
        uint16_t num_leds,
        std::function<void()> on_change_callback,
        const Config& config,
        Effects effect = Effects::LOADING_RING);

    void setConfig(const Config& config) { m_config = config; }
    const Config& getConfig() const { return m_config; }

    void start() override;
    void update() override;
    void render(CRGB* out) override;
    void stop() override;

private:
    Config m_config;
    bool m_running = false;

    uint16_t m_head = 0;
    shs::t::shs_time_t m_last_step_ms = 0;

    static CRGB m_scaleColor(const CRGB& color, uint8_t scale);
};
