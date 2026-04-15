

#include <memory>

#include <shs_debug.h>

#include <FastLED.h>

#include "../SmartGarden/SmartGarden.h"
#include "../SmartGarden/SmartGardenConfig.h"
#include "../SmartGarden/SmartGardenBuilder.h"

std::shared_ptr<shs::SmartGarden> smart_garden;
CRGB leds[shs::SmartGardenConfig::ARGB_COUNT];
void setup()
{
    dinit();
    shs::SmartGardenConfig config;
    smart_garden = shs::SmartGardenBuilder::build(config);
    smart_garden->start();
}

void loop()
{
    smart_garden->tick();
}
