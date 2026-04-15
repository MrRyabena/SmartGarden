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
        class BreathBrightnessEffect;
    }
}

class shs::argb::BreathBrightnessEffect : public shs::argb::Effect
{
public:
    struct Config
    {
        uint16_t period_ms = 1600;
        uint8_t min_brightness = 48;
        uint8_t max_brightness = 255;
    };

    explicit BreathBrightnessEffect(CRGB* leds,
        uint16_t num_leds,
        std::function<void()> on_change_callback,
        const Config& config);

    void setConfig(const Config& config) { m_config = config; }

    void start() override;
    void update() override;
    void render(CRGB* out) override;
    void stop() override;

private:
    Config m_config;
    bool m_running = false;
    shs::t::shs_time_t m_started_ms = 0;

    uint8_t m_currentBrightness() const;
};
