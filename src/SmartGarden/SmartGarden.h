#pragma once


#include <memory>

#include <EncButton.h>

#include <shs_Process.h>
#include <shs_Load.h>
#include <shs_types.h>
#include <shs_ProgramTimer.h>

#include <shs_debug.h>
#include "SmartGardenData.h"

namespace shs
{
    class SmartGardenSensor;
    class TimedLoad;
    class SmartGardenData;
    class SmartGardenIndication;

    class SmartGarden;
}


class shs::SmartGarden : public shs::Process
{
public:
    explicit SmartGarden(std::shared_ptr<shs::SmartGardenSensor> sensor,
        std::shared_ptr<Button> button,
        std::shared_ptr<shs::TimedLoad> water_pump, std::shared_ptr<shs::Load> light,
        std::shared_ptr<shs::SmartGardenIndication> indication = nullptr);

    ~SmartGarden() = default;

    void setWaterPumpDuration(const uint16_t duration_ms) { m_water_pump_duration_ms = duration_ms; }
    [[nodiscard]] uint16_t getWaterPumpDuration() const { return m_water_pump_duration_ms; }

    void waterPlants();
    void switchLight();
    void showIndication();

    bool isDataUpdated() const { return m_data_updated; }
    void getSensorsData(SmartGardenData& data) const { data = m_data; }

    void start() override;
    void tick() override;
    void stop() override;

protected:
    std::shared_ptr<shs::SmartGardenSensor> m_sensor;
    std::shared_ptr<shs::TimedLoad> m_water_pump;
    std::shared_ptr<shs::Load> m_light;
    std::shared_ptr<shs::SmartGardenIndication> m_indication;
    std::shared_ptr<Button> m_button;

    shs::SmartGardenData m_data;
    shs::ProgramTimer m_data_update_timer;
    shs::ProgramTimer m_manual_light_switch_timer;
    bool m_data_updated = false;

    uint16_t m_water_pump_duration_ms;

    void m_updateData();
    void m_autoLightControl();
    void m_buttonHandler();
};
