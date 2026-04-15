#include "SmartGardenIndication.h"

#include "SmartGardenData.h"
#include "SmartGardenConfig.h"
#include <shs_lib_ARGB_Effect.h>

#include "shs_debug.h"

shs::SmartGardenIndication::SmartGardenIndication()
    : m_effect_manager(m_leds, LEDS_COUNT, [this]() { FastLED.show(); }),
    m_sensor_effect(m_leds, LEDS_COUNT, nullptr, shs::argb::SensorRingEffect::Config{}),
    m_loading_effect(m_leds, LEDS_COUNT, nullptr, shs::argb::LoadingRingEffect::Config{}),
    m_breath_effect(m_leds, LEDS_COUNT, nullptr, shs::argb::BreathBrightnessEffect::Config{}),
    m_loading_timer(5000)
{
    FastLED.addLeds<WS2812B, shs::SmartGardenConfig::ARGB_PIN, GRB>(m_leds, LEDS_COUNT);
    m_applyDefaultLayerStack();
}

void shs::SmartGardenIndication::start()
{
    doutln("Starting SmartGardenIndication...");
    FastLED.setBrightness(255);

    // Keep LEDs definitely off until an explicit indication scene is enabled.
    for (auto& led : m_leds) led = CRGB::Black;
    FastLED.clear(true);

    m_sensor_effect.setEnabled(false);
    m_loading_effect.setEnabled(false);
    m_breath_effect.setEnabled(false);

    m_started = true;
    m_scene = Scene::NONE;
}

void shs::SmartGardenIndication::tick()
{
    if (!m_started) return;

    if (m_loading_timer)
    {
        m_loading_effect.setEnabled(false);
        m_breath_effect.setEnabled(false);
    }

    if (m_scene == Scene::NONE)
    {
        FastLED.clear(true);
        return;
    }

    m_effect_manager.tick();

    if (!m_show_all_active || m_scene != Scene::SENSOR || !m_sensor_effect.finished()) return;

    if (m_show_all_stage == ShowAllStage::SOIL)
    {
        m_startShowAllStage(ShowAllStage::TEMPERATURE);
        return;
    }

    if (m_show_all_stage == ShowAllStage::TEMPERATURE)
    {
        m_startShowAllStage(ShowAllStage::HUMIDITY);
        return;
    }


    m_show_all_active = false;
    m_show_all_stage = ShowAllStage::NONE;
    m_sensor_effect.setEnabled(false);
    m_scene = Scene::NONE;
    FastLED.clear(true);
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
    m_show_all_active = false;
    m_show_all_stage = ShowAllStage::NONE;

    shs::argb::SensorRingEffect::Config cfg;
    cfg.min_value = 0.0f;
    cfg.max_value = 100.0f;
    cfg.good_min = 40.0f;
    cfg.good_max = 100.0f;
    cfg.base_color = CRGB::Cyan;
    cfg.assess_ms = 1500;
    cfg.fade_ms = 1000;
    cfg.fill_ms = 1500;

    m_prepareSensorEffect(value, cfg);
}

void shs::SmartGardenIndication::showTemperature()
{
    showTemperature(static_cast<float>(m_data.temperature_celsius));
}

void shs::SmartGardenIndication::showTemperature(const float value)
{
    m_show_all_active = false;
    m_show_all_stage = ShowAllStage::NONE;

    shs::argb::SensorRingEffect::Config cfg;
    cfg.min_value = 0.0f;
    cfg.max_value = 50.0f;
    cfg.good_min = 18.0f;
    cfg.good_max = 25.0f;
    cfg.base_color = CRGB::Yellow;
    cfg.assess_ms = 1500;
    cfg.fade_ms = 1000;
    cfg.fill_ms = 1500;

    m_prepareSensorEffect(value, cfg);
}

void shs::SmartGardenIndication::showHumidity()
{
    showHumidity(static_cast<float>(m_data.humidity_percent));
}

void shs::SmartGardenIndication::showHumidity(const float value)
{
    m_show_all_active = false;
    m_show_all_stage = ShowAllStage::NONE;

    shs::argb::SensorRingEffect::Config cfg;
    cfg.min_value = 0.0f;
    cfg.max_value = 100.0f;
    cfg.good_min = 45.0f;
    cfg.good_max = 75.0f;
    cfg.base_color = CRGB::Blue;
    cfg.assess_ms = 1500;
    cfg.fade_ms = 1000;
    cfg.fill_ms = 1500;

    m_prepareSensorEffect(value, cfg);
}

void shs::SmartGardenIndication::showAll()
{
    m_show_all_active = true;
    m_startShowAllStage(ShowAllStage::SOIL);
}

void shs::SmartGardenIndication::showLoading(const CRGB& color, const shs::t::shs_time_t duration_ms)
{
    auto cfg = m_loading_effect.getConfig();
    cfg.color = color;
    cfg.step_ms = 100;
    cfg.tail_length = 3;
    cfg.tail_falloff = 100;

    m_loading_effect.setConfig(cfg);
    m_enableLoadingScene();
    m_loading_effect.start();
    if (duration_ms > 0)
    {
        m_loading_timer.setTimeout(duration_ms);
        m_loading_timer.reset();
    }
}

void shs::SmartGardenIndication::m_prepareSensorEffect(const float value, const shs::argb::SensorRingEffect::Config& config)
{
    m_sensor_effect.setConfig(config);
    m_sensor_effect.setValue(value);

    m_enableSensorScene();
    m_sensor_effect.start();
}

void shs::SmartGardenIndication::m_startShowAllStage(const ShowAllStage stage)
{
    m_show_all_stage = stage;

    if (stage == ShowAllStage::SOIL)
    {
        shs::argb::SensorRingEffect::Config cfg;
        cfg.min_value = 0.0f;
        cfg.max_value = 100.0f;
        cfg.good_min = 40.0f;
        cfg.good_max = 100.0f;
        cfg.base_color = CRGB::Cyan;
        cfg.assess_ms = 1500;
        cfg.fade_ms = 1000;
        cfg.fill_ms = 1500;

        m_prepareSensorEffect(static_cast<float>(m_data.soil_moisture_percent), cfg);
        return;
    }

    if (stage == ShowAllStage::TEMPERATURE)
    {
        shs::argb::SensorRingEffect::Config cfg;
        cfg.min_value = 0.0f;
        cfg.max_value = 50.0f;
        cfg.good_min = 18.0f;
        cfg.good_max = 25.0f;
        cfg.base_color = CRGB::Yellow;
        cfg.assess_ms = 1500;
        cfg.fade_ms = 1000;
        cfg.fill_ms = 1500;

        m_prepareSensorEffect(static_cast<float>(m_data.temperature_celsius), cfg);
        return;
    }

    shs::argb::SensorRingEffect::Config cfg;
    cfg.min_value = 0.0f;
    cfg.max_value = 100.0f;
    cfg.good_min = 45.0f;
    cfg.good_max = 75.0f;
    cfg.base_color = CRGB::Blue;
    m_prepareSensorEffect(static_cast<float>(m_data.humidity_percent), cfg);
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
