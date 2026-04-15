#pragma once

#include <Arduino.h>
#include <shs_types.h>
#include <shs_lib_DHT.h>

namespace shs
{
    struct SmartGardenConfig;
}

struct shs::SmartGardenConfig
{
    shs::t::shs_pin_t soil_sensor_pin = SOIL_SENSOR_PIN;
    uint16_t soil_sensor_lower_bound = SOIL_SENSOR_LOWER_BOUND;
    uint16_t soil_sensor_upper_bound = SOIL_SENSOR_UPPER_BOUND;
    shs::t::shs_pin_t soil_sensor_en_high_pin = SOIL_SENSOR_EN_HIGH_PIN;
    shs::t::shs_pin_t soil_sensor_en_low_pin = SOIL_SENSOR_EN_LOW_PIN;

    shs::t::shs_pin_t light_sensor_pin = LIGHT_SENSOR_PIN;
    uint16_t light_sensor_lower_bound = LIGHT_SENSOR_LOWER_BOUND;
    uint16_t light_sensor_upper_bound = LIGHT_SENSOR_UPPER_BOUND;
    shs::t::shs_pin_t light_sensor_en_high_pin = LIGHT_SENSOR_EN_HIGH_PIN;
    shs::t::shs_pin_t light_sensor_en_low_pin = LIGHT_SENSOR_EN_LOW_PIN;

    shs::t::shs_pin_t dht_pin = DHT_PIN;
    shs::DHT::DHTtype dht_type = DHT_TYPE;

    shs::t::shs_time_t sensor_enabling_time_ms = SENSOR_ENABLING_TIME_MS;

    shs::t::shs_pin_t button_pin = BUTTON_PIN;
    shs::t::shs_pin_t button_mode = BUTTON_MODE;
    shs::t::shs_pin_t button_level = BUTTON_LEVEL;

    shs::t::shs_pin_t water_pump_pin = WATER_PUMP_PIN;
    uint16_t water_pump_duration_ms = WATER_PUMP_DURATION_MS;

    shs::t::shs_pin_t light_pin = LIGHT_PIN;

    shs::t::shs_pin_t argb_pin = ARGB_PIN;
    uint8_t argb_count = ARGB_COUNT;

    shs::t::shs_moduleID_t module_id = MODULE_ID;


    static constexpr shs::t::shs_pin_t SOIL_SENSOR_PIN = A0;
    static constexpr uint16_t SOIL_SENSOR_LOWER_BOUND = 300;
    static constexpr uint16_t SOIL_SENSOR_UPPER_BOUND = 700;
    static constexpr shs::t::shs_pin_t SOIL_SENSOR_EN_HIGH_PIN = 12;  // D6
    static constexpr shs::t::shs_pin_t SOIL_SENSOR_EN_LOW_PIN = 15;   // D8

    static constexpr shs::t::shs_pin_t LIGHT_SENSOR_PIN = A0;
    static constexpr uint16_t LIGHT_SENSOR_LOWER_BOUND = 300;
    static constexpr uint16_t LIGHT_SENSOR_UPPER_BOUND = 700;
    static constexpr shs::t::shs_pin_t LIGHT_SENSOR_EN_HIGH_PIN = 13; // D7
    static constexpr shs::t::shs_pin_t LIGHT_SENSOR_EN_LOW_PIN = 2;   // D4

    static constexpr shs::t::shs_pin_t DHT_PIN = 14;
    static constexpr shs::DHT::DHTtype DHT_TYPE = shs::DHT::DHTtype::T_DHT22;

    static constexpr shs::t::shs_time_t SENSOR_ENABLING_TIME_MS = 1000;
    
    static constexpr shs::t::shs_pin_t BUTTON_PIN = 16; // D0
    static constexpr shs::t::shs_pin_t BUTTON_MODE = INPUT;
    static constexpr shs::t::shs_pin_t BUTTON_LEVEL = LOW;

    static constexpr shs::t::shs_pin_t WATER_PUMP_PIN = 5;
    static constexpr uint16_t WATER_PUMP_DURATION_MS = 2000;

    static constexpr shs::t::shs_pin_t LIGHT_PIN = 4;

    static constexpr shs::t::shs_pin_t ARGB_PIN = 0;
    static constexpr uint8_t ARGB_COUNT = 8;

    static constexpr shs::t::shs_moduleID_t MODULE_ID = 40;
};
