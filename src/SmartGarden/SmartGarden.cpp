#include "SmartGarden.h"

#include "SmartGardenIndication.h"
#include "SmartGardenSensor.h"
#include "TimedLoad.h"


shs::SmartGarden::SmartGarden(std::shared_ptr<shs::SmartGardenSensor> sensor,
    std::shared_ptr<Button> button,
    std::shared_ptr<shs::TimedLoad> water_pump, std::shared_ptr<shs::Load> light,
    std::shared_ptr<shs::SmartGardenIndication> indication
)
    :
    m_sensor(sensor),
    m_button(button),
    m_water_pump(std::move(water_pump)),
    m_light(light),
    m_indication(indication),
    m_data_update_timer(5000),
    m_manual_light_switch_timer(1), // 3 hours
    m_water_pump_duration_ms(10000)
{
}

void shs::SmartGarden::waterPlants()
{
    if (m_water_pump)
    {
        m_water_pump->on(m_water_pump_duration_ms);
        m_indication->showLoading(CRGB::Cyan, m_water_pump_duration_ms);
    }
}

void shs::SmartGarden::switchLight()
{
    m_manual_light_switch_timer.setTimeout(3 * 60 * 60 * 1000); // 3 hours
    m_manual_light_switch_timer.reset();
    if (m_light)
    {
        if (m_light->getValue() == 0) m_light->on();
        else m_light->off();
    }
}

void shs::SmartGarden::showIndication()
{
    if (m_indication)
    {
        m_indication->setData(m_data);
        m_indication->showAll();
    }
}

void shs::SmartGarden::start()
{
    if (m_sensor) m_sensor->start();
    if (m_light) m_light->setup();
    if (m_water_pump) m_water_pump->setup();
    if (m_indication)
    {
        m_indication->start();
        m_indication->showLoading(CRGB::Cyan);
    }
}

void shs::SmartGarden::tick()
{
    if (m_sensor) m_sensor->tick();
    if (m_water_pump) m_water_pump->tick();
    if (m_indication) m_indication->tick();

    m_buttonHandler();
    m_autoLightControl();
    m_updateData();
}

void shs::SmartGarden::stop()
{
    if (m_sensor) m_sensor->stop();
    if (m_water_pump) m_water_pump->stop();
}

void shs::SmartGarden::m_updateData()
{
    if (m_data_update_timer.check())
    {
        if (m_sensor) m_sensor->update();
        m_data_updated = false;
    }

    if (!m_data_updated && m_sensor && m_sensor->isUpdated())
    {
        auto soil_raw = m_sensor->getValueI(static_cast<uint8_t>(shs::SmartGardenSensor::Metrics::SOIL_MOISTURE_RAW));
        auto light_raw = m_sensor->getValueI(static_cast<uint8_t>(shs::SmartGardenSensor::Metrics::LIGHT_LEVEL_RAW));
        m_data.soil_moisture_percent = m_sensor->getValueFx(static_cast<uint8_t>(shs::SmartGardenSensor::Metrics::SOIL_MOISTURE_PERCENT));
        m_data.light_level_percent = m_sensor->getValueFx(static_cast<uint8_t>(shs::SmartGardenSensor::Metrics::LIGHT_LEVEL_PERCENT));
        m_data.temperature_celsius = m_sensor->getValueFx(static_cast<uint8_t>(shs::SmartGardenSensor::Metrics::TEMPERATURE));
        m_data.humidity_percent = m_sensor->getValueFx(static_cast<uint8_t>(shs::SmartGardenSensor::Metrics::HUMIDITY));

        m_data_updated = true;

        dsep();
        doutln("Sensor data updated:");
        dout("  Soil moisture: "); dout(m_data.soil_moisture_percent.toFloat()); dout(" %  "); dout(soil_raw); doutln(" raw");
        dout("  Light level: "); dout(m_data.light_level_percent.toFloat()); dout(" %  "); dout(light_raw); doutln(" raw");
        dout("  Temperature: "); dout(m_data.temperature_celsius.toFloat()); doutln(" °C");
        dout("  Humidity: "); dout(m_data.humidity_percent.toFloat()); doutln(" %");
    }
}

void shs::SmartGarden::m_autoLightControl()
{
    if (!m_light || !m_sensor) return;

    // Don't auto control light if it was switched manually in the last minute
    if (!m_manual_light_switch_timer.expired()) return;

    const auto light_level = m_sensor->getValueI(static_cast<uint8_t>(shs::SmartGardenSensor::Metrics::LIGHT_LEVEL_PERCENT));

    // TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 

    if (light_level < 35.0f && m_light->getValue() == 0)
    {
        m_light->on();
    }
    else if (light_level > 60.0f && m_light->getValue() != 0)
    {
        m_light->off();
    }
}

void shs::SmartGarden::m_buttonHandler()
{
    if (!m_button) return;

    m_button->tick();

    if (m_button->hasClicks())
    {
        switch (m_button->getClicks())
        {
            case 1:
                doutln("Button 1 click detected: showing indication");
                showIndication();
                break;
            case 2:
                doutln("Button 2 clicks detected: switching light");
                switchLight();
                break;
            case 3:
                doutln("Button 3 clicks detected: watering plants");
                waterPlants();
                break;
        }
    }
}

