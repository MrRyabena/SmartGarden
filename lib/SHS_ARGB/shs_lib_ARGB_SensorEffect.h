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
        class SensorRingEffect;
    }
}

class shs::argb::SensorRingEffect : public shs::argb::Effect
{
public:
    struct Config
    {
        float min_value = 0.0f;
        float max_value = 100.0f;

        float good_min = 35.0f;
        float good_max = 65.0f;

        CRGB base_color = CRGB::Cyan;
        CRGB ok_color = CRGB::Green;
        CRGB alarm_color = CRGB::Red;

        uint8_t min_leds = 1;

        uint16_t fill_ms = 1500;
        uint16_t assess_ms = 1000;
        uint16_t fade_ms = 1500;
    };

    enum class Phase : uint8_t { IDLE, FILL, ASSESS, FADE, DONE };

    explicit SensorRingEffect(CRGB* leds,
        uint16_t num_leds,
        std::function<void()> on_change_callback,
        const Config& config);

    void setConfig(const Config& config) { m_config = config; }
    const Config& getConfig() const { return m_config; }

    void setValue(float value);
    float getValue() const { return m_value; }

    Phase getPhase() const { return m_phase; }
    bool finished() const { return m_phase == Phase::DONE; }

    void start() override;
    void update() override;
    void render(CRGB* out) override;
    void stop() override;

private:
    Config m_config;

    float m_value = 0.0f;
    uint8_t m_target_leds = 1;

    Phase m_phase = Phase::IDLE;
    shs::t::shs_time_t m_phase_started_ms = 0;

    uint8_t m_clampLeds(uint8_t value) const;
    uint8_t m_calculateTargetLeds() const;
    bool m_valueInGoodRange() const;

    float m_phaseProgress(uint16_t phase_ms) const;
    static float m_clamp01(float value);
    static CRGB m_blendColor(const CRGB& from, const CRGB& to, float t);

    void m_setPhase(Phase phase);
    void m_drawArc(CRGB* out, uint16_t start, uint8_t count, const CRGB& color, uint8_t brightness = 255) const;
};
