#include "shs_lib_ARGB_SensorEffect.h"

#include <math.h>

#include <shs_ProgramTime.h>

shs::argb::SensorRingEffect::SensorRingEffect(CRGB* leds,
    const uint16_t num_leds,
    std::function<void()> on_change_callback,
    const Config& config)
    : Effect(leds, num_leds, on_change_callback, Effects::SENSOR_RING), m_config(config)
{}

void shs::argb::SensorRingEffect::setValue(const float value)
{
    m_value = value;
    m_target_leds = m_calculateTargetLeds();
}

void shs::argb::SensorRingEffect::start()
{
    m_target_leds = m_calculateTargetLeds();
    m_setPhase(Phase::FILL);
}

void shs::argb::SensorRingEffect::update()
{
    if (m_phase == Phase::IDLE || m_phase == Phase::DONE) return;

    if (m_phase == Phase::FILL && m_phaseProgress(m_config.fill_ms) >= 1.0f)
    {
        m_setPhase(Phase::ASSESS);
        return;
    }

    if (m_phase == Phase::ASSESS && m_phaseProgress(m_config.assess_ms) >= 1.0f)
    {
        m_setPhase(Phase::FADE);
        return;
    }

    if (m_phase == Phase::FADE && m_phaseProgress(m_config.fade_ms) >= 1.0f)
    {
        m_setPhase(Phase::DONE);
    }
}

void shs::argb::SensorRingEffect::render(CRGB* out)
{
    if (out == nullptr || m_num_leds == 0 || m_phase == Phase::IDLE || m_phase == Phase::DONE) return;

    for (uint16_t i = 0; i < m_num_leds; ++i) out[i] = CRGB::Black;

    const CRGB status_color = m_valueInGoodRange() ? m_config.ok_color : m_config.alarm_color;

    if (m_phase == Phase::FILL)
    {
        const float t = m_phaseProgress(m_config.fill_ms);
        const uint8_t count = m_clampLeds(static_cast<uint8_t>(roundf(m_target_leds * t)));
        m_drawArc(out, 0, count, m_config.base_color);
        return;
    }

    if (m_phase == Phase::ASSESS)
    {
        const float t = m_phaseProgress(m_config.assess_ms);
        const CRGB color = m_blendColor(m_config.base_color, status_color, t);
        m_drawArc(out, 0, m_target_leds, color);
        return;
    }

    const float t = m_phaseProgress(m_config.fade_ms);
    const uint8_t brightness = static_cast<uint8_t>(255U - static_cast<uint16_t>(255.0f * t));
    const uint16_t shift = static_cast<uint16_t>((static_cast<float>(m_num_leds) * t));
    m_drawArc(out, shift, m_target_leds, status_color, brightness);
}

void shs::argb::SensorRingEffect::stop()
{
    m_phase = Phase::DONE;
}

uint8_t shs::argb::SensorRingEffect::m_clampLeds(const uint8_t value) const
{
    if (m_num_leds == 0) return 0;

    const uint8_t min_leds = static_cast<uint8_t>(m_config.min_leds == 0 ? 1 : m_config.min_leds);
    const uint8_t max_leds = static_cast<uint8_t>(m_num_leds > 255 ? 255 : m_num_leds);

    if (value < min_leds) return min_leds;
    if (value > max_leds) return max_leds;
    return value;
}

uint8_t shs::argb::SensorRingEffect::m_calculateTargetLeds() const
{
    if (m_num_leds == 0) return 0;

    const float range = m_config.max_value - m_config.min_value;
    if (range <= 0.0f) return m_clampLeds(1);

    const float normalized = m_clamp01((m_value - m_config.min_value) / range);
    const float mapped = 1.0f + normalized * static_cast<float>(m_num_leds - 1);
    return m_clampLeds(static_cast<uint8_t>(roundf(mapped)));
}

bool shs::argb::SensorRingEffect::m_valueInGoodRange() const
{
    return m_value >= m_config.good_min && m_value <= m_config.good_max;
}

float shs::argb::SensorRingEffect::m_phaseProgress(const uint16_t phase_ms) const
{
    if (phase_ms == 0) return 1.0f;

    const auto now = static_cast<shs::t::shs_time_t>(shs::ProgramTime::s_milliseconds());
    const auto elapsed = static_cast<shs::t::shs_time_t>(now - m_phase_started_ms);
    return m_clamp01(static_cast<float>(elapsed) / static_cast<float>(phase_ms));
}

float shs::argb::SensorRingEffect::m_clamp01(const float value)
{
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

CRGB shs::argb::SensorRingEffect::m_blendColor(const CRGB& from, const CRGB& to, const float t)
{
    const float tc = m_clamp01(t);
    const auto blend_ch = [tc](const uint8_t a, const uint8_t b) -> uint8_t
    {
        return static_cast<uint8_t>(static_cast<float>(a) + (static_cast<float>(b) - static_cast<float>(a)) * tc);
    };

    return CRGB(blend_ch(from.r, to.r), blend_ch(from.g, to.g), blend_ch(from.b, to.b));
}

void shs::argb::SensorRingEffect::m_setPhase(const Phase phase)
{
    m_phase = phase;
    m_phase_started_ms = static_cast<shs::t::shs_time_t>(shs::ProgramTime::s_milliseconds());
}

void shs::argb::SensorRingEffect::m_drawArc(CRGB* out, const uint16_t start, const uint8_t count, const CRGB& color, const uint8_t brightness) const
{
    if (out == nullptr || m_num_leds == 0 || count == 0 || brightness == 0) return;

    const CRGB c(
        static_cast<uint8_t>((static_cast<uint16_t>(color.r) * brightness) / 255U),
        static_cast<uint8_t>((static_cast<uint16_t>(color.g) * brightness) / 255U),
        static_cast<uint8_t>((static_cast<uint16_t>(color.b) * brightness) / 255U));

    const uint8_t arc_len = m_clampLeds(count);
    for (uint8_t i = 0; i < arc_len; ++i)
    {
        const uint16_t index = static_cast<uint16_t>((start + i) % m_num_leds);
        out[index] = c;
    }
}
