#include "SmartGardenBuilder.h"


#include <shs_LoadSwitch.h>

#include "SmartGarden.h"
#include "SmartGardenConfig.h"
#include "SmartGardenSensor.h"
#include "TimedLoad.h"  
#include "SmartGardenIndication.h"

std::shared_ptr<shs::SmartGarden> shs::SmartGardenBuilder::build(const SmartGardenConfig& config)
{
    return std::make_shared<SmartGarden>(
        std::make_shared<shs::SmartGardenSensor>(
            std::make_shared<shs::SensorAnalogMapped>(config.soil_sensor_pin, config.soil_sensor_lower_bound, config.soil_sensor_upper_bound),
            std::make_shared<shs::SensorAnalogMapped>(config.light_sensor_pin, config.light_sensor_lower_bound, config.light_sensor_upper_bound),
            std::make_shared<shs::DHT>(config.dht_pin, config.dht_type),
            config.soil_sensor_en_high_pin,
            config.soil_sensor_en_low_pin,
            config.light_sensor_en_high_pin,
            config.light_sensor_en_low_pin,
            config.sensor_enabling_time_ms
        ),
        std::make_shared<shs::TimedLoad>(config.water_pump_pin),
        std::make_shared<shs::LoadSwitch>(config.light_pin),
        std::make_shared<shs::SmartGardenIndication>()
    );
}