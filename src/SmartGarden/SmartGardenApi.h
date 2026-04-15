#pragma once

#include <memory>

#include <shs_API.h>
#include <shs_ByteCollector.h>
#include <shs_ByteCollectorIterator.h>
#include <shs_DTPpacket.h>

#include "SmartGarden.h"
#include "SmartGardenData.h"


namespace shs
{
    class SmartGardenApi;
}

class shs::SmartGardenApi : public shs::API
{
public:
    enum class Commands : uint8_t
    {
        WATER_PLANTS = 1,
        LIGHT_ON,
        LIGHT_OFF,
        GET_DATA,
        DATA
    };

    explicit SmartGardenApi(std::shared_ptr<shs::SmartGarden> smart_garden, shs::t::shs_ID_t id)
        : API(id), m_smart_garden(std::move(smart_garden))
    {
    }


    [[nodiscard]] shs::DTPpacket handle(shs::ByteCollectorReadIterator<>& it) override
    {
        if (!m_smart_garden) return shs::DTPpacket();

        const auto command = it.read();

        switch (static_cast<Commands>(command))
        {
            case Commands::WATER_PLANTS: // Water plants
                m_smart_garden->waterPlants();
                break;

            case Commands::LIGHT_ON: // Switch light on
                m_smart_garden->onLight();
                break;

            case Commands::LIGHT_OFF: // Switch light off
                m_smart_garden->offLight();
                break;

            case Commands::GET_DATA: // Get data
                {
                    shs::ByteCollector<> bc(sizeof(SmartGardenData));
                    SmartGardenData data;
                    m_smart_garden->getSensorsData(data);

                    bc.push_back(Commands::DATA, 1);
                    bc.push_back(data);
                    return shs::DTPpacket(API_ID, shs::DTPpacket::get_senderID(it), bc);
                }
                break;

            default:
                break;
        }

        return shs::DTPpacket();
    }

private:
    std::shared_ptr<shs::SmartGarden> m_smart_garden;

};
