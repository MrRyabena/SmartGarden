#pragma once

#include <stdint.h>
#include <functional>

#include <shs_Process.h>


class CRGB;

namespace shs
{
    class Process;

    namespace argb
    {
        class Effect;
    }
}


class shs::argb::Effect : public shs::Process
{
public:
    enum class Effects : uint16_t { NO_EFFECT, RING_LOAD, SENSOR_RING, LOADING_RING, BREATH_BRIGHTNESS };
    enum class LayerMode : uint8_t { ALPHA, ADD, MAX, MULTIPLY };

    explicit Effect(CRGB* leds, uint16_t num_leds, std::function<void()> on_change_callback, Effects effect = Effects::NO_EFFECT);

    Effects getEffect() const { return m_effect; }
    bool isEnabled() const { return m_enabled; }
    void setEnabled(const bool enabled) { m_enabled = enabled; }

    void setCallback(std::function<void()> on_change_callback) { m_on_change_callback = on_change_callback; }

    void start() override {}
    void tick() override { update(); }
    void stop() override {}

    virtual void update() = 0;
    virtual void render(CRGB* out) = 0;


protected:
    std::function<void()> m_on_change_callback;

    bool m_enabled = true;
    CRGB* const m_leds;
    const uint16_t m_num_leds;
    const Effects m_effect;
};
