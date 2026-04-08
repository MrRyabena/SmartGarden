#pragma once


#include <memory>

#include <shs_Process.h>
#include <shs_LoadSwitch.h>
#include <shs_types.h>

#include "SmartGardenData.h"
#include "SmartGardenSensor.h"
#include "TimedLoad.h"

namespace shs
{
    class SmartGardenIndication;
    class SmartGarden;
}


class shs::SmartGarden : public shs::Process
{
public:
    explicit SmartGarden(std::shared_ptr<shs::SensorAnalogMapped> soil_sensor,
        std::shared_ptr<shs::SensorAnalogMapped> light_sensor,
        std::shared_ptr<shs::DHT> dht,
        std::shared_ptr<shs::TimedLoad> water_pump,
        std::shared_ptr<shs::SmartGardenIndication> indication = nullptr);

    ~SmartGarden() = default;

    void setWaterPumpDuration(const uint16_t duration_ms) { m_water_pump_duration_ms = duration_ms; }
    [[nodiscard]] uint16_t getWaterPumpDuration() const { return m_water_pump_duration_ms; }
    void waterPlants();

    void setSoilSensor(std::shared_ptr<shs::SensorAnalogMapped> soil_sensor) { m_soil_sensor = std::move(soil_sensor); }
    void setLightSensor(std::shared_ptr<shs::SensorAnalogMapped> light_sensor) { m_light_sensor = std::move(light_sensor); }
    void setDHT(std::shared_ptr<shs::DHT> dht) { m_dht = std::move(dht); }
    void setWaterPump(std::shared_ptr<shs::TimedLoad> water_pump) { m_water_pump = std::move(water_pump); }
    void setIndication(std::shared_ptr<shs::SmartGardenIndication> indication) { m_indication = std::move(indication); }

    const SmartGardenData& getData() const { return m_data; }

    void start() override;
    void tick() override;
    void stop() override;

protected:
    std::shared_ptr<shs::SmartGardenSensor> m_sensor;
    std::shared_ptr<shs::TimedLoad> m_water_pump;
    std::shared_ptr<shs::LoadSwitch> m_light;
    std::shared_ptr<shs::SmartGardenIndication> m_indication;

    uint16_t m_water_pump_duration_ms;
};
