#pragma once

#include <shs_Process.h>
#include <FastLED.h>

#include <shs_lib_ARGB_BreathBrightnessEffect.h>
#include <shs_lib_ARGB_EffectManager.h>
#include <shs_lib_ARGB_LoadingEffect.h>
#include <shs_lib_ARGB_SensorEffect.h>


namespace shs
{
    class SmartGardenData;
    class SmartGardenIndication;

}


class shs::SmartGardenIndication : public shs::Process
{
public:
    static constexpr uint8_t LEDS_COUNT = 8;

    explicit SmartGardenIndication();
    ~SmartGardenIndication() = default;

    void setData(const SmartGardenData& data) { m_data = data; }

    void showSoilMoisture();
    void showSoilMoisture(float value);

    void showTemperature();
    void showTemperature(float value);

    void showHumidity();
    void showHumidity(float value);

    void showLoading(const CRGB& color = CRGB::Blue);
        
    void start() override;
    void tick() override;
    void stop() override;

private:
    enum class Scene : uint8_t { NONE, SENSOR, LOADING };

    CRGB m_leds[LEDS_COUNT];
    SmartGardenData m_data{};

    shs::argb::EffectManager m_effect_manager;
    shs::argb::SensorRingEffect m_sensor_effect;
    shs::argb::LoadingRingEffect m_loading_effect;
    shs::argb::BreathBrightnessEffect m_breath_effect;

    Scene m_scene = Scene::NONE;
    bool m_started = false;

    void m_prepareSensorEffect(float value, const shs::argb::SensorRingEffect::Config& config);
    void m_enableSensorScene();
    void m_enableLoadingScene();
    void m_applyDefaultLayerStack();

};
