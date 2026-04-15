#include "shs_lib_ARGB_EffectManager.h"

#include <FastLED.h>

namespace
{
    uint8_t scaleChannel(const uint8_t value, const uint8_t opacity)
    {
        return static_cast<uint8_t>((static_cast<uint16_t>(value) * opacity) / 255U);
    }
}

shs::argb::EffectManager::EffectManager(CRGB* leds, const uint16_t num_leds, std::function<void()> on_change_callback)
    : m_on_change_callback(on_change_callback), m_leds(leds), m_layer_buffer(new CRGB[num_leds]), m_num_leds(num_leds)
{}

shs::argb::EffectManager::~EffectManager()
{
    delete[] m_layer_buffer;
}

bool shs::argb::EffectManager::addEffect(Effect* effect, const uint8_t opacity, const Effect::LayerMode mode)
{
    if (effect == nullptr) return false;

    for (auto& layer : m_layers)
    {
        if (!layer.active)
        {
            layer.effect = effect;
            layer.opacity = opacity;
            layer.mode = mode;
            layer.active = true;
            return true;
        }
    }

    return false;
}

void shs::argb::EffectManager::removeEffect(const Effect* effect)
{
    for (auto& layer : m_layers)
    {
        if (layer.active && layer.effect == effect)
        {
            layer.effect = nullptr;
            layer.active = false;
            layer.opacity = 255;
            layer.mode = Effect::LayerMode::ALPHA;
            return;
        }
    }
}

void shs::argb::EffectManager::clearEffects()
{
    for (auto& layer : m_layers)
    {
        layer.effect = nullptr;
        layer.active = false;
        layer.opacity = 255;
        layer.mode = Effect::LayerMode::ALPHA;
    }
}

void shs::argb::EffectManager::setLayerOpacity(const Effect* effect, const uint8_t opacity)
{
    for (auto& layer : m_layers)
    {
        if (layer.active && layer.effect == effect)
        {
            layer.opacity = opacity;
            return;
        }
    }
}

void shs::argb::EffectManager::setLayerMode(const Effect* effect, const Effect::LayerMode mode)
{
    for (auto& layer : m_layers)
    {
        if (layer.active && layer.effect == effect)
        {
            layer.mode = mode;
            return;
        }
    }
}

void shs::argb::EffectManager::start()
{
    for (auto& layer : m_layers)
    {
        if (layer.active && layer.effect != nullptr) layer.effect->start();
    }
}

void shs::argb::EffectManager::tick()
{
    m_clear(m_leds);

    for (const auto& layer : m_layers)
    {
        if (!layer.active || layer.effect == nullptr || !layer.effect->isEnabled()) continue;

        layer.effect->tick();

        m_clear(m_layer_buffer);
        layer.effect->render(m_layer_buffer);
        m_compose(m_layer_buffer, layer.opacity, layer.mode);
    }

    if (m_on_change_callback) m_on_change_callback();
}

void shs::argb::EffectManager::stop()
{
    for (auto& layer : m_layers)
    {
        if (layer.active && layer.effect != nullptr) layer.effect->stop();
    }

    m_clear(m_leds);
    if (m_on_change_callback) m_on_change_callback();
}

void shs::argb::EffectManager::m_clear(CRGB* leds) const
{
    for (uint16_t i = 0; i < m_num_leds; ++i) leds[i] = CRGB::Black;
}

void shs::argb::EffectManager::m_compose(const CRGB* src, const uint8_t opacity, const Effect::LayerMode mode)
{
    for (uint16_t i = 0; i < m_num_leds; ++i)
    {
        if (mode == Effect::LayerMode::ALPHA)
        {
            m_leds[i] = blend(m_leds[i], src[i], opacity);
            continue;
        }

        if (mode == Effect::LayerMode::ADD)
        {
            m_leds[i].r = qadd8(m_leds[i].r, scaleChannel(src[i].r, opacity));
            m_leds[i].g = qadd8(m_leds[i].g, scaleChannel(src[i].g, opacity));
            m_leds[i].b = qadd8(m_leds[i].b, scaleChannel(src[i].b, opacity));
            continue;
        }

        if (mode == Effect::LayerMode::MAX)
        {
            m_leds[i].r = max(m_leds[i].r, scaleChannel(src[i].r, opacity));
            m_leds[i].g = max(m_leds[i].g, scaleChannel(src[i].g, opacity));
            m_leds[i].b = max(m_leds[i].b, scaleChannel(src[i].b, opacity));
            continue;
        }

        const uint8_t mr = scaleChannel(src[i].r, opacity);
        const uint8_t mg = scaleChannel(src[i].g, opacity);
        const uint8_t mb = scaleChannel(src[i].b, opacity);

        m_leds[i].r = static_cast<uint8_t>((static_cast<uint16_t>(m_leds[i].r) * mr) / 255U);
        m_leds[i].g = static_cast<uint8_t>((static_cast<uint16_t>(m_leds[i].g) * mg) / 255U);
        m_leds[i].b = static_cast<uint8_t>((static_cast<uint16_t>(m_leds[i].b) * mb) / 255U);
    }
}
