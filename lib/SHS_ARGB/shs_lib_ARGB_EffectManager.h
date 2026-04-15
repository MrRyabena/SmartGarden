#pragma once

#include <stdint.h>
#include <functional>

#include <shs_Process.h>

#include "shs_lib_ARGB_Effect.h"

class CRGB;

namespace shs
{
    namespace argb
    {
        class EffectManager;
    }
}

class shs::argb::EffectManager : public shs::Process
{
public:
    static constexpr uint8_t MAX_LAYERS = 8;

    explicit EffectManager(CRGB* leds, uint16_t num_leds, std::function<void()> on_change_callback = nullptr);
    ~EffectManager() override;

    bool addEffect(Effect* effect, uint8_t opacity = 255, Effect::LayerMode mode = Effect::LayerMode::ALPHA);
    void removeEffect(const Effect* effect);
    void clearEffects();

    void setLayerOpacity(const Effect* effect, uint8_t opacity);
    void setLayerMode(const Effect* effect, Effect::LayerMode mode);

    void start() override;
    void tick() override;
    void stop() override;

private:
    struct Layer
    {
        Effect* effect = nullptr;
        uint8_t opacity = 255;
        Effect::LayerMode mode = Effect::LayerMode::ALPHA;
        bool active = false;
    };

    std::function<void()> m_on_change_callback;

    CRGB* const m_leds;
    CRGB* m_layer_buffer;

    const uint16_t m_num_leds;
    Layer m_layers[MAX_LAYERS];

    void m_clear(CRGB* leds) const;
    void m_compose(const CRGB* src, uint8_t opacity, Effect::LayerMode mode);
};
