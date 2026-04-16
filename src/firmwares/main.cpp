

#include <memory>

#include <shs_debug.h>

#include <shs_ControlWiFi.h>
#include <shs_DTP.h>
#include <shs_TcpServer.h>
#include <shs_settings_private.h>

#include <FastLED.h>

#include "../SmartGarden/SmartGarden.h"
#include "../SmartGarden/SmartGardenConfig.h"
#include "../SmartGarden/SmartGardenBuilder.h"
#include "../SmartGarden/SmartGardenIndication.h"
#include "../SmartGarden/SmartGardenApi.h"

std::shared_ptr<shs::SmartGarden> smart_garden;
std::shared_ptr<shs::TcpServer> tcp_server;
CRGB leds[shs::SmartGardenConfig::ARGB_COUNT];
bool connected = false;

std::shared_ptr<shs::DTP> dtp;

void setup()
{
    dinit();
    shs::SmartGardenConfig config;
    smart_garden = shs::SmartGardenBuilder::build(config);
    smart_garden->start();

    shs::ControlWiFi::connectWiFiMulti();
    smart_garden->getIndication()->showLoading(CRGB::Cyan, 60'000);


    dtp = std::make_shared<shs::DTP>(config.module_id);
    dtp->attachAPI(std::make_unique<shs::SmartGardenApi>(smart_garden, config.module_id));

    tcp_server = std::make_shared<shs::TcpServer>(shs::settings::DEFAULT_TCP_PORT, *dtp);

    dtp->start();
    tcp_server->start();
}

void loop()
{
    smart_garden->tick();
    dtp->tick();
    tcp_server->tick();

    if (!connected && shs::ControlWiFi::WiFiConnected())
    {
        connected = true;
        smart_garden->getIndication()->showLoading(CRGB::Green, 2000);
    }

}
