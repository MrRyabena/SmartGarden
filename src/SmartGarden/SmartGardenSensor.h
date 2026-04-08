#pragma once

#include <memory>

#include <GyverIO.h>

#include <shs_Sensor.h>
#include <shs_SensorAnalogMapped.h>
#include <shs_lib_DHT.h>
#include <shs_Process.h>
#include <shs_types.h>
#include <shs_ProgramTimer.h>


namespace shs
{
    class SmartGardenSensor;
}


class shs::SmartGardenSensor : public shs::Sensor, public shs::Process
{
public:

    enum class Status : uint8_t
    {
        NOT_INITIALIZED,
        UPDATED,
        UPDATING_STARTED,
        ENABLING_SOIL,
        SOIL_UPDATED,
        ENABLING_LIGHT,
        LIGHT_UPDATED,
        ERROR,
    };

    enum class Metrics : uint8_t
    {
        SOIL_MOISTURE_RAW,
        SOIL_MOISTURE_PERCENT,
        SOIL_MOISTURE_NORMALIZED_255,
        LIGHT_LEVEL_RAW,
        LIGHT_LEVEL_PERCENT,
        LIGHT_LEVEL_NORMALIZED_255,
        TEMPERATURE,
        HUMIDITY,
    };

    SmartGardenSensor(std::shared_ptr<shs::SensorAnalogMapped> soil_sensor,
        std::shared_ptr<shs::SensorAnalogMapped> light_sensor,
        std::shared_ptr<shs::DHT> dht,
        shs::t::shs_pin_t soil_sensor_en_high = -1,
        shs::t::shs_pin_t soil_sensor_en_low = -1,
        shs::t::shs_pin_t light_sensor_en_high = -1,
        shs::t::shs_pin_t light_sensor_en_low = -1,
        shs::t::shs_time_t enabling_timeout = 500
    );

    ~SmartGardenSensor() override = default;


    void setSoilSensor(std::shared_ptr<shs::SensorAnalogMapped> soil_sensor) { m_soil_sensor = std::move(soil_sensor); }
    void setLightSensor(std::shared_ptr<shs::SensorAnalogMapped> light_sensor) { m_light_sensor = std::move(light_sensor); }
    void setDHT(std::shared_ptr<shs::DHT> dht) { m_dht = std::move(dht); }
    std::shared_ptr<shs::SensorAnalogMapped> getSoilSensor() const { return m_soil_sensor; }
    std::shared_ptr<shs::SensorAnalogMapped> getLightSensor() const { return m_light_sensor; }
    std::shared_ptr<shs::DHT> getDHT() const { return m_dht; }

    void setEnablingTimeout(const shs::t::shs_time_t timeout) { m_enabling_timer.setTimeout(timeout); }
    shs::t::shs_time_t getEnablingTimeout() const { return m_enabling_timer.getTimeout(); }

    void setSoilSensorEnablePins(const shs::t::shs_pin_t en_high, const shs::t::shs_pin_t en_low);
    void setLightSensorEnablePins(const shs::t::shs_pin_t en_high, const shs::t::shs_pin_t en_low);

    std::pair<shs::t::shs_pin_t, shs::t::shs_pin_t> getSoilSensorEnablePins() const { return { m_soil_sensor_en_high, m_soil_sensor_en_low }; }
    std::pair<shs::t::shs_pin_t, shs::t::shs_pin_t> getLightSensorEnablePins() const { return { m_light_sensor_en_high, m_light_sensor_en_low }; }

    void setup() override;

    void update() override;
    void updateFast() override;
    void updateForced(bool fast = false) override;

    void start() override { setup(); }
    void tick() override;
    void stop() override {}

    [[nodiscard]] bool isUpdated() override;
    [[nodiscard]] uint8_t getStatus() override;

    void clearCache() override;
    void setCacheExpiration(const uint32_t expiration_time) override;
    uint32_t getCacheExpiration() const override;

    [[nodiscard]] int32_t getValueI(const uint8_t metric = 0) override;
    [[nodiscard]] shs::t::shs_fixed_t getValueFx(const uint8_t metric = 0) override;
    [[nodiscard]] shs::t::shs_float_t getValueF(const uint8_t metric = 0) override;
    [[nodiscard]] shs::t::shs_double_t getValueD(const uint8_t metric = 0) override;


protected:
    std::shared_ptr<shs::SensorAnalogMapped> m_soil_sensor;
    std::shared_ptr<shs::SensorAnalogMapped> m_light_sensor;
    std::shared_ptr<shs::DHT> m_dht;

    shs::ProgramTimer m_enabling_timer;

    shs::t::shs_pin_t m_soil_sensor_en_high;
    shs::t::shs_pin_t m_soil_sensor_en_low;
    shs::t::shs_pin_t m_light_sensor_en_high;
    shs::t::shs_pin_t m_light_sensor_en_low;

    Status m_status;
    bool m_update_fast;

    void m_enableSoilSensor(bool enable);
    void m_enableLightSensor(bool enable);
};
