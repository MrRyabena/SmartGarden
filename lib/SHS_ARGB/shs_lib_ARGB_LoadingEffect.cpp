#include "shs_lib_ARGB_LoadingEffect.h"

#include <shs_ProgramTime.h>

shs::argb::LoadingRingEffect::LoadingRingEffect(CRGB* leds,
    const uint16_t num_leds,
    std::function<void()> on_change_callback,
    const Config& config,
    const Effects effect)
    : Effect(leds, num_leds, on_change_callback, effect), m_config(config)
{}

void shs::argb::LoadingRingEffect::start()
{
    m_running = true;
    m_head = 0;
    m_last_step_ms = static_cast<shs::t::shs_time_t>(shs::ProgramTime::s_milliseconds());
}

void shs::argb::LoadingRingEffect::update()
{
    if (!m_running || m_num_leds == 0 || m_config.step_ms == 0) return;

    const auto now = static_cast<shs::t::shs_time_t>(shs::ProgramTime::s_milliseconds());
    const auto elapsed = static_cast<shs::t::shs_time_t>(now - m_last_step_ms);
    if (elapsed < m_config.step_ms) return;

    const uint16_t steps = elapsed / m_config.step_ms;
    m_last_step_ms = static_cast<shs::t::shs_time_t>(m_last_step_ms + steps * m_config.step_ms);

    if (m_config.clockwise)
    {
        m_head = static_cast<uint16_t>((m_head + steps) % m_num_leds);
    }
    else
    {
        const uint16_t shift = static_cast<uint16_t>(steps % m_num_leds);
        m_head = static_cast<uint16_t>((m_head + m_num_leds - shift) % m_num_leds);
    }
}

void shs::argb::LoadingRingEffect::render(CRGB* out)
{
    if (out == nullptr || !m_running || m_num_leds == 0) return;

    for (uint16_t i = 0; i < m_num_leds; ++i) out[i] = CRGB::Black;

    const uint8_t tail = m_config.tail_length == 0 ? 1 : m_config.tail_length;

    for (uint8_t i = 0; i < tail; ++i)
    {
        const uint8_t dim = static_cast<uint8_t>(255 - i * m_config.tail_falloff);
        const uint16_t pos = static_cast<uint16_t>((m_head + m_num_leds - i) % m_num_leds);
        out[pos] = m_scaleColor(m_config.color, dim);
    }
}

void shs::argb::LoadingRingEffect::stop()
{
    m_running = false;
}

CRGB shs::argb::LoadingRingEffect::m_scaleColor(const CRGB& color, const uint8_t scale)
{
    return CRGB(
        static_cast<uint8_t>((static_cast<uint16_t>(color.r) * scale) / 255U),
        static_cast<uint8_t>((static_cast<uint16_t>(color.g) * scale) / 255U),
        static_cast<uint8_t>((static_cast<uint16_t>(color.b) * scale) / 255U));
}
