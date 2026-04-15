#include "shs_lib_ARGB_BreathBrightnessEffect.h"

#include <shs_ProgramTime.h>

shs::argb::BreathBrightnessEffect::BreathBrightnessEffect(CRGB* leds,
    const uint16_t num_leds,
    std::function<void()> on_change_callback,
    const Config& config)
    : Effect(leds, num_leds, on_change_callback, Effects::BREATH_BRIGHTNESS), m_config(config)
{}

void shs::argb::BreathBrightnessEffect::start()
{
    m_running = true;
    m_started_ms = static_cast<shs::t::shs_time_t>(shs::ProgramTime::s_milliseconds());
}

void shs::argb::BreathBrightnessEffect::update()
{
    if (!m_running) return;
}

void shs::argb::BreathBrightnessEffect::render(CRGB* out)
{
    if (out == nullptr || !m_running || m_num_leds == 0) return;

    const uint8_t brightness = m_currentBrightness();
    const CRGB mask(brightness, brightness, brightness);

    for (uint16_t i = 0; i < m_num_leds; ++i) out[i] = mask;
}

void shs::argb::BreathBrightnessEffect::stop()
{
    m_running = false;
}

uint8_t shs::argb::BreathBrightnessEffect::m_currentBrightness() const
{
    if (m_config.period_ms == 0) return m_config.max_brightness;

    const auto now = static_cast<shs::t::shs_time_t>(shs::ProgramTime::s_milliseconds());
    const auto elapsed = static_cast<shs::t::shs_time_t>(now - m_started_ms);
    const uint16_t phase = static_cast<uint16_t>(elapsed % m_config.period_ms);

    const uint16_t half = static_cast<uint16_t>(m_config.period_ms / 2U);
    uint16_t wave = 255;

    if (half != 0)
    {
        if (phase <= half)
        {
            wave = static_cast<uint16_t>((static_cast<uint32_t>(phase) * 255U) / half);
        }
        else
        {
            wave = static_cast<uint16_t>((static_cast<uint32_t>(m_config.period_ms - phase) * 255U) / half);
        }
    }

    const uint8_t min_b = m_config.min_brightness;
    const uint8_t max_b = m_config.max_brightness;
    const uint8_t diff = static_cast<uint8_t>(max_b - min_b);

    return static_cast<uint8_t>(min_b + (static_cast<uint16_t>(diff) * wave) / 255U);
}
