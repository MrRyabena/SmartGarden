#include "SmartGardenSensor.h"

#include <shs_utils.h>

shs::SmartGardenSensor::SmartGardenSensor(std::shared_ptr<shs::SensorAnalogMapped> soil_sensor,
    std::shared_ptr<shs::SensorAnalogMapped> light_sensor,
    std::shared_ptr<shs::DHT> dht,
    const shs::t::shs_pin_t soil_sensor_en_high,
    const shs::t::shs_pin_t soil_sensor_en_low,
    const shs::t::shs_pin_t light_sensor_en_high,
    const shs::t::shs_pin_t light_sensor_en_low,
    const shs::t::shs_time_t enabling_timeout
)
    :
    Sensor(),
    m_soil_sensor(std::move(soil_sensor)),
    m_light_sensor(std::move(light_sensor)),
    m_dht(std::move(dht)),
    m_enabling_timer(enabling_timeout),
    m_soil_sensor_en_high(soil_sensor_en_high),
    m_soil_sensor_en_low(soil_sensor_en_low),
    m_light_sensor_en_high(light_sensor_en_high),
    m_light_sensor_en_low(light_sensor_en_low)
{
}

void shs::SmartGardenSensor::setSoilSensorEnablePins(const shs::t::shs_pin_t en_high, const shs::t::shs_pin_t en_low)
{
    m_soil_sensor_en_high = en_high;
    m_soil_sensor_en_low = en_low;
}

void shs::SmartGardenSensor::setLightSensorEnablePins(const shs::t::shs_pin_t en_high, const shs::t::shs_pin_t en_low)
{
    m_light_sensor_en_high = en_high;
    m_light_sensor_en_low = en_low;
}

void shs::SmartGardenSensor::setup()
{
    if (m_soil_sensor) m_soil_sensor->setup();
    if (m_soil_sensor_en_high) gio::init(m_soil_sensor_en_high);
    if (m_soil_sensor_en_low)  gio::init(m_soil_sensor_en_low);

    if (m_light_sensor) m_light_sensor->setup();
    if (m_light_sensor_en_high) gio::init(m_light_sensor_en_high);
    if (m_light_sensor_en_low)  gio::init(m_light_sensor_en_low);

    if (m_dht) m_dht->setup();
}


void shs::SmartGardenSensor::update()
{
    m_status = Status::UPDATING_STARTED;
    m_update_fast = false;
}

void shs::SmartGardenSensor::updateFast()
{
    m_status = Status::UPDATING_STARTED;
    m_update_fast = true;
}

void shs::SmartGardenSensor::updateForced(bool fast)
{
    m_status = Status::UPDATING_STARTED;
    m_update_fast = fast;
}

void shs::SmartGardenSensor::tick()
{
    switch (m_status)
    {
        case Status::UPDATING_STARTED:
            if (m_soil_sensor)
            {
                m_enableSoilSensor(true);
                m_enabling_timer.reset();
                m_status = Status::ENABLING_SOIL;
            }
            else
            {
                m_status = Status::SOIL_UPDATED;
            }
            break;

        case Status::ENABLING_SOIL:
            {
                if (m_enabling_timer.check())
                {
                    m_soil_sensor->update();
                    m_enableSoilSensor(false);
                    m_status = Status::SOIL_UPDATED;
                }
            }
            break;

        case Status::SOIL_UPDATED:
            if (m_light_sensor)
            {
                m_enableLightSensor(true);
                m_enabling_timer.reset();
                m_status = Status::ENABLING_LIGHT;
            }
            else
            {
                m_status = Status::LIGHT_UPDATED;
            }
            break;

        case Status::ENABLING_LIGHT:
            {
                if (m_enabling_timer.check())
                {
                    m_light_sensor->update();
                    m_enableLightSensor(false);
                    m_status = Status::LIGHT_UPDATED;
                }
            }
            break;

        case Status::LIGHT_UPDATED:
            if (m_dht)
            {
                m_dht->update();
            }
            m_status = Status::UPDATED;
            break;

        default:
            break;
    }

}

[[nodiscard]] bool shs::SmartGardenSensor::isUpdated()
{
    if (m_status != Status::UPDATED) return false;

    bool soil_updated = m_soil_sensor ? m_soil_sensor->isUpdated() : true;
    bool light_updated = m_light_sensor ? m_light_sensor->isUpdated() : true;
    bool dht_updated = m_dht ? m_dht->isUpdated() : true;

    return soil_updated && light_updated && dht_updated;
}

[[nodiscard]] uint8_t shs::SmartGardenSensor::getStatus()
{
    return static_cast<uint8_t>(m_status);
}

void shs::SmartGardenSensor::clearCache()
{
    if (m_soil_sensor) m_soil_sensor->clearCache();
    if (m_light_sensor) m_light_sensor->clearCache();
    if (m_dht) m_dht->clearCache();
}

void shs::SmartGardenSensor::setCacheExpiration(const uint32_t expiration_time)
{
    if (m_soil_sensor) m_soil_sensor->setCacheExpiration(expiration_time);
    if (m_light_sensor) m_light_sensor->setCacheExpiration(expiration_time);
    if (m_dht) m_dht->setCacheExpiration(expiration_time);
}

uint32_t shs::SmartGardenSensor::getCacheExpiration() const
{
    return 0;
}

int32_t shs::SmartGardenSensor::getValueI(const uint8_t metric)
{
    using sam_metrics = shs::SensorAnalogMapped::Metrics;

    switch (static_cast<Metrics>(metric))
    {
        case Metrics::SOIL_MOISTURE_RAW:
            return m_soil_sensor ? m_soil_sensor->getValueI(shs::etoi(sam_metrics::RAW)) : 0;
        case Metrics::SOIL_MOISTURE_PERCENT:
            return m_soil_sensor ? m_soil_sensor->getValueI(shs::etoi(sam_metrics::PERCENT)) : 0;
        case Metrics::SOIL_MOISTURE_NORMALIZED_255:
            return m_soil_sensor ? m_soil_sensor->getValueI(shs::etoi(sam_metrics::NORMALIZED_255)) : 0;
        case Metrics::LIGHT_LEVEL_RAW:
            return m_light_sensor ? m_light_sensor->getValueI(shs::etoi(sam_metrics::RAW)) : 0;
        case Metrics::LIGHT_LEVEL_PERCENT:
            return m_light_sensor ? m_light_sensor->getValueI(shs::etoi(sam_metrics::PERCENT)) : 0;
        case Metrics::LIGHT_LEVEL_NORMALIZED_255:
            return m_light_sensor ? m_light_sensor->getValueI(shs::etoi(sam_metrics::NORMALIZED_255)) : 0;
        case Metrics::TEMPERATURE:
            return m_dht ? m_dht->getValueI(shs::etoi(shs::DHT::Metrics::TEMPERATURE)) : 0;
        case Metrics::HUMIDITY:
            return m_dht ? m_dht->getValueI(shs::etoi(shs::DHT::Metrics::HUMIDITY)) : 0;
        default:
            return 0;
    }
}

shs::t::shs_fixed_t shs::SmartGardenSensor::getValueFx(const uint8_t metric)
{
    using sam_metrics = shs::SensorAnalogMapped::Metrics;

    switch (static_cast<Metrics>(metric))
    {
        case Metrics::SOIL_MOISTURE_RAW:
            if (m_soil_sensor) return m_soil_sensor->getValueFx(shs::etoi(sam_metrics::RAW));
            return shs::t::shs_fixed_t(0);
        case Metrics::SOIL_MOISTURE_PERCENT:
            if (m_soil_sensor) return m_soil_sensor->getValueFx(shs::etoi(sam_metrics::PERCENT));
            return shs::t::shs_fixed_t(0);
        case Metrics::SOIL_MOISTURE_NORMALIZED_255:
            if (m_soil_sensor) return m_soil_sensor->getValueFx(shs::etoi(sam_metrics::NORMALIZED_255));
            return shs::t::shs_fixed_t(0);
        case Metrics::LIGHT_LEVEL_RAW:
            if (m_light_sensor) return m_light_sensor->getValueFx(shs::etoi(sam_metrics::RAW));
            return shs::t::shs_fixed_t(0);
        case Metrics::LIGHT_LEVEL_PERCENT:
            if (m_light_sensor) return m_light_sensor->getValueFx(shs::etoi(sam_metrics::PERCENT));
            return shs::t::shs_fixed_t(0);
        case Metrics::LIGHT_LEVEL_NORMALIZED_255:
            if (m_light_sensor) return m_light_sensor->getValueFx(shs::etoi(sam_metrics::NORMALIZED_255));
            return shs::t::shs_fixed_t(0);
        case Metrics::TEMPERATURE:
            if (m_dht) return m_dht->getValueFx(shs::etoi(shs::DHT::Metrics::TEMPERATURE));
            return shs::t::shs_fixed_t(0);
        case Metrics::HUMIDITY:
            if (m_dht) return m_dht->getValueFx(shs::etoi(shs::DHT::Metrics::HUMIDITY));
            return shs::t::shs_fixed_t(0);
        default:
            return shs::t::shs_fixed_t(0);
    }

    return shs::t::shs_fixed_t(0);
}

shs::t::shs_float_t shs::SmartGardenSensor::getValueF(const uint8_t metric)
{
    using sam_metrics = shs::SensorAnalogMapped::Metrics;

    switch (static_cast<Metrics>(metric))
    {
        case Metrics::SOIL_MOISTURE_RAW:
            return m_soil_sensor ? m_soil_sensor->getValueF(shs::etoi(sam_metrics::RAW)) : 0;
        case Metrics::SOIL_MOISTURE_PERCENT:
            return m_soil_sensor ? m_soil_sensor->getValueF(shs::etoi(sam_metrics::PERCENT)) : 0;
        case Metrics::SOIL_MOISTURE_NORMALIZED_255:
            return m_soil_sensor ? m_soil_sensor->getValueF(shs::etoi(sam_metrics::NORMALIZED_255)) : 0;
        case Metrics::LIGHT_LEVEL_RAW:
            return m_light_sensor ? m_light_sensor->getValueF(shs::etoi(sam_metrics::RAW)) : 0;
        case Metrics::LIGHT_LEVEL_PERCENT:
            return m_light_sensor ? m_light_sensor->getValueF(shs::etoi(sam_metrics::PERCENT)) : 0;
        case Metrics::LIGHT_LEVEL_NORMALIZED_255:
            return m_light_sensor ? m_light_sensor->getValueF(shs::etoi(sam_metrics::NORMALIZED_255)) : 0;
        case Metrics::TEMPERATURE:
            return m_dht ? m_dht->getValueF(shs::etoi(shs::DHT::Metrics::TEMPERATURE)) : 0;
        case Metrics::HUMIDITY:
            return m_dht ? m_dht->getValueF(shs::etoi(shs::DHT::Metrics::HUMIDITY)) : 0;
        default:
            break;
    }
    return 0;
}

shs::t::shs_double_t shs::SmartGardenSensor::getValueD(const uint8_t metric)
{
    using sam_metrics = shs::SensorAnalogMapped::Metrics;

    switch (static_cast<Metrics>(metric))
    {
        case Metrics::SOIL_MOISTURE_RAW:
            return m_soil_sensor ? m_soil_sensor->getValueD(shs::etoi(sam_metrics::RAW)) : 0;
        case Metrics::SOIL_MOISTURE_PERCENT:
            return m_soil_sensor ? m_soil_sensor->getValueD(shs::etoi(sam_metrics::PERCENT)) : 0;
        case Metrics::SOIL_MOISTURE_NORMALIZED_255:
            return m_soil_sensor ? m_soil_sensor->getValueD(shs::etoi(sam_metrics::NORMALIZED_255)) : 0;
        case Metrics::LIGHT_LEVEL_RAW:
            return m_light_sensor ? m_light_sensor->getValueD(shs::etoi(sam_metrics::RAW)) : 0;
        case Metrics::LIGHT_LEVEL_PERCENT:
            return m_light_sensor ? m_light_sensor->getValueD(shs::etoi(sam_metrics::PERCENT)) : 0;
        case Metrics::LIGHT_LEVEL_NORMALIZED_255:
            return m_light_sensor ? m_light_sensor->getValueD(shs::etoi(sam_metrics::NORMALIZED_255)) : 0;
        case Metrics::TEMPERATURE:
            return m_dht ? m_dht->getValueD(shs::etoi(shs::DHT::Metrics::TEMPERATURE)) : 0;
        case Metrics::HUMIDITY:
            return m_dht ? m_dht->getValueD(shs::etoi(shs::DHT::Metrics::HUMIDITY)) : 0;
        default:
            break;
    }
    return shs::t::shs_double_t();
}


void shs::SmartGardenSensor::m_enableSoilSensor(const bool enable)
{
    if (!m_soil_sensor) return;
    if (enable)
    {
        if (m_soil_sensor_en_high >= 0)
        {
            gio::mode(m_soil_sensor_en_high, OUTPUT);
            gio::high(m_soil_sensor_en_high);
        }
        if (m_soil_sensor_en_low >= 0)
        {
            gio::mode(m_soil_sensor_en_low, OUTPUT);
            gio::low(m_soil_sensor_en_low);
        }
        return;
    }

    if (m_soil_sensor_en_high >= 0) gio::mode(m_soil_sensor_en_high, INPUT);
    if (m_soil_sensor_en_low >= 0) gio::mode(m_soil_sensor_en_low, INPUT);
}

void shs::SmartGardenSensor::m_enableLightSensor(const bool enable)
{
    if (!m_light_sensor) return;
    if (enable)
    {
        if (m_light_sensor_en_high >= 0)
        {
            gio::mode(m_light_sensor_en_high, OUTPUT);
            gio::high(m_light_sensor_en_high);
        }
        if (m_light_sensor_en_low >= 0)
        {
            gio::mode(m_light_sensor_en_low, OUTPUT);
            gio::low(m_light_sensor_en_low);
        }
        return;
    }

    if (m_light_sensor_en_high >= 0) gio::mode(m_light_sensor_en_high, INPUT);
    if (m_light_sensor_en_low >= 0) gio::mode(m_light_sensor_en_low, INPUT);
}
