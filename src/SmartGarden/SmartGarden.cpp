#include "SmartGarden.h"


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
    m_data_update_timer(5000)
{
}

void shs::SmartGarden::waterPlants()
{
    if (m_water_pump)
    {
        m_water_pump->on(m_water_pump_duration_ms);
    }
}

void shs::SmartGarden::start()
{
    if (m_sensor) m_sensor->start();
    if (m_light) m_light->setup();
}

void shs::SmartGarden::tick()
{
    if (m_sensor) m_sensor->tick();
    if (m_water_pump) m_water_pump->tick();


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
        m_data.soil_moisture_percent = m_sensor->getValueFx(static_cast<uint8_t>(shs::SmartGardenSensor::Metrics::SOIL_MOISTURE_PERCENT));
        m_data.light_level_percent = m_sensor->getValueFx(static_cast<uint8_t>(shs::SmartGardenSensor::Metrics::LIGHT_LEVEL_PERCENT));
        m_data.temperature_celsius = m_sensor->getValueFx(static_cast<uint8_t>(shs::SmartGardenSensor::Metrics::TEMPERATURE));
        m_data.humidity_percent = m_sensor->getValueFx(static_cast<uint8_t>(shs::SmartGardenSensor::Metrics::HUMIDITY));

        m_data_updated = true;
    }
}

