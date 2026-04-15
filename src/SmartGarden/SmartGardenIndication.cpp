#include "SmartGardenIndication.h"

#include "SmartGardenData.h"

#include <shs_lib_ARGB_Effect.h>

shs::SmartGardenIndication::SmartGardenIndication()
    : m_effect_manager(m_leds, LEDS_COUNT, [this]() { FastLED.show(); }),
    m_sensor_effect(m_leds, LEDS_COUNT, nullptr, shs::argb::SensorRingEffect::Config{}),
    m_loading_effect(m_leds, LEDS_COUNT, nullptr, shs::argb::LoadingRingEffect::Config{}),
    m_breath_effect(m_leds, LEDS_COUNT, nullptr, shs::argb::BreathBrightnessEffect::Config{})
{
    FastLED.addLeds<WS2812, 5, GRB>(m_leds, LEDS_COUNT);
    m_applyDefaultLayerStack();
}

void shs::SmartGardenIndication::start()
{
    FastLED.setBrightness(255);

    for (auto& led : m_leds) led = CRGB::Black;
    FastLED.show();

    m_started = true;
    m_scene = Scene::NONE;
    m_effect_manager.start();
}

void shs::SmartGardenIndication::tick()
{
    if (!m_started) return;
    m_effect_manager.tick();
}

void shs::SmartGardenIndication::stop()
{
    m_effect_manager.stop();
    m_started = false;
    m_scene = Scene::NONE;
}

void shs::SmartGardenIndication::showSoilMoisture()
{
    showSoilMoisture(static_cast<float>(m_data.soil_moisture_percent));
}

void shs::SmartGardenIndication::showSoilMoisture(const float value)
{
    shs::argb::SensorRingEffect::Config cfg;
    cfg.min_value = 0.0f;
    cfg.max_value = 100.0f;
    cfg.good_min = 35.0f;
    cfg.good_max = 65.0f;
    cfg.base_color = CRGB::Cyan;

    m_prepareSensorEffect(value, cfg);
}

void shs::SmartGardenIndication::showTemperature()
{
    showTemperature(static_cast<float>(m_data.temperature_celsius));
}

void shs::SmartGardenIndication::showTemperature(const float value)
{
    shs::argb::SensorRingEffect::Config cfg;
    cfg.min_value = 0.0f;
    cfg.max_value = 50.0f;
    cfg.good_min = 18.0f;
    cfg.good_max = 30.0f;
    cfg.base_color = CRGB::Yellow;

    m_prepareSensorEffect(value, cfg);
}

void shs::SmartGardenIndication::showHumidity()
{
    showHumidity(static_cast<float>(m_data.humidity_percent));
}

void shs::SmartGardenIndication::showHumidity(const float value)
{
    shs::argb::SensorRingEffect::Config cfg;
    cfg.min_value = 0.0f;
    cfg.max_value = 100.0f;
    cfg.good_min = 45.0f;
    cfg.good_max = 75.0f;
    cfg.base_color = CRGB::Green;

    m_prepareSensorEffect(value, cfg);
}

void shs::SmartGardenIndication::showLoading(const CRGB& color)
{
    auto cfg = m_loading_effect.getConfig();
    cfg.color = color;
    cfg.step_ms = 85;
    cfg.tail_length = 3;
    cfg.tail_falloff = 85;

    m_loading_effect.setConfig(cfg);
    m_enableLoadingScene();
    m_loading_effect.start();
}

void shs::SmartGardenIndication::m_prepareSensorEffect(const float value, const shs::argb::SensorRingEffect::Config& config)
{
    m_sensor_effect.setConfig(config);
    m_sensor_effect.setValue(value);

    m_enableSensorScene();
    m_sensor_effect.start();
}

void shs::SmartGardenIndication::m_enableSensorScene()
{
    m_loading_effect.setEnabled(false);
    m_breath_effect.setEnabled(false);

    m_sensor_effect.setEnabled(true);
    m_scene = Scene::SENSOR;
}

void shs::SmartGardenIndication::m_enableLoadingScene()
{
    m_sensor_effect.setEnabled(false);

    m_loading_effect.setEnabled(true);
    m_breath_effect.setEnabled(true);
    m_breath_effect.start();

    m_scene = Scene::LOADING;
}

void shs::SmartGardenIndication::m_applyDefaultLayerStack()
{
    m_effect_manager.clearEffects();

    m_effect_manager.addEffect(&m_sensor_effect, 255, shs::argb::Effect::LayerMode::ALPHA);
    m_effect_manager.addEffect(&m_loading_effect, 255, shs::argb::Effect::LayerMode::ALPHA);
    m_effect_manager.addEffect(&m_breath_effect, 255, shs::argb::Effect::LayerMode::MULTIPLY);

    m_sensor_effect.setEnabled(false);
    m_loading_effect.setEnabled(false);
    m_breath_effect.setEnabled(false);
}
