#include "BrightnessManager.h"

BrightnessManager::BrightnessManager(
    Adafruit_NeoPixel* topStrip,
    Adafruit_NeoPixel* teamAStrip,
    Adafruit_NeoPixel* teamBStrip)
:
top(topStrip),
teamA(teamAStrip),
teamB(teamBStrip)
{
}

void BrightnessManager::begin()
{
    Wire.begin();

    sensorAvailable =
    sensor.begin(
        BH1750::CONTINUOUS_HIGH_RES_MODE
    );

    if(sensorAvailable)
    {
        Serial.println(
            "BH1750 erkannt"
        );
    }
    else
    {
        Serial.println(
            "BH1750 nicht vorhanden, feste Helligkeit aktiv"
        );

        sensorEnabled = false;
    }

    brightness = 64;

    top->setBrightness(
        brightness
    );

    teamA->setBrightness(
        brightness
    );

    teamB->setBrightness(
        brightness
    );
}

void BrightnessManager::setEnabled(
    bool enabled)
{
    sensorEnabled = enabled;
}

bool BrightnessManager::available() const
{
    return sensorAvailable;

}

bool BrightnessManager::enabled() const
{
    return sensorEnabled;
}

void BrightnessManager::setFixedBrightness(
    uint8_t value)
{
    brightness = value;

    top->setBrightness(
        brightness
    );

    teamA->setBrightness(
        brightness
    );

    teamB->setBrightness(
        brightness
    );

    top->show();
    teamA->show();
    teamB->show();
}

void BrightnessManager::setBrightnessRange(
    uint8_t minB,
    uint8_t maxB)
{
    minBrightness = minB;
    maxBrightness = maxB;
}

void BrightnessManager::setLuxRange(
    float minL,
    float maxL)
{
    minLux = minL;
    maxLux = maxL;
}

uint8_t BrightnessManager::currentBrightness() const
{
    return brightness;
}

float BrightnessManager::currentLux() const
{
    return lastLux;
}

uint8_t BrightnessManager::calculateBrightness(
    float lux)
{
    if(lux <= minLux)
    {
        return minBrightness;
    }

    if(lux >= maxLux)
    {
        return maxBrightness;
    }

    float factor =
        (lux - minLux) /
        (maxLux - minLux);

    return
        minBrightness +
        (
            (
                maxBrightness -
                minBrightness
            ) * factor
        );
}

void BrightnessManager::update()
{
    if(!sensorAvailable)
    {
        return;
    }
    
    if(!sensorEnabled)
    {
        return;
    }

    uint32_t now =
        millis();

    if(
        now - lastUpdate < 1000
    )
    {
        return;
    }

    lastUpdate = now;

    float lux =
        sensor.readLightLevel();

    if(
        lux < 0 ||
        isnan(lux)
    )
    {
        return;
    }

    lastLux = lux;

    uint8_t newBrightness =
        calculateBrightness(
            lux
        );

    if(
        newBrightness ==
        brightness
    )
    {
        return;
    }

    brightness =
        newBrightness;

    top->setBrightness(
        brightness
    );

    teamA->setBrightness(
        brightness
    );

    teamB->setBrightness(
        brightness
    );

    top->show();
    teamA->show();
    teamB->show();
}