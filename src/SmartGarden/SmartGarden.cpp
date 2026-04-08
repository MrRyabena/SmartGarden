#include "SmartGarden.h"

shs::SmartGarden::SmartGarden(std::shared_ptr<shs::SensorAnalogMapped> soil_sensor,
    std::shared_ptr<shs::SensorAnalogMapped> light_sensor,
    std::shared_ptr<shs::DHT> dht,
    std::shared_ptr<shs::TimedLoad> water_pump,
    std::shared_ptr<shs::SmartGardenIndication> indication
)
    : m_soil_sensor(std::move(soil_sensor)),
    m_light_sensor(std::move(light_sensor)),
    m_dht(std::move(dht)),
    m_water_pump(std::move(water_pump)),
    m_indication(indication)
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
    if (m_soil_sensor) m_soil_sensor->setup();
    if (m_light_sensor) m_light_sensor->setup();
    if (m_dht) m_dht->setup();
    if (m_water_pump) m_water_pump->setup();

}

void shs::SmartGarden::tick()
{
}

void shs::SmartGarden::stop()
{
}

