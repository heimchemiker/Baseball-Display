#pragma once

#include <BH1750.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

class BrightnessManager
{
public:

    BrightnessManager(
        Adafruit_NeoPixel* topStrip,
        Adafruit_NeoPixel* teamAStrip,
        Adafruit_NeoPixel* teamBStrip
    );

    void begin();

    void update();

    void setEnabled(
        bool enabled
    );

    bool available() const;

    bool enabled() const;

    void setBrightnessRange(
        uint8_t minBrightness,
        uint8_t maxBrightness
    );

    void setFixedBrightness(
        uint8_t value
    );

    void setLuxRange(
        float minLux,
        float maxLux
    );

    uint8_t currentBrightness() const;

    float currentLux() const;

private:

    uint8_t calculateBrightness(
        float lux
    );

private:

    BH1750 sensor;

    Adafruit_NeoPixel* top;
    Adafruit_NeoPixel* teamA;
    Adafruit_NeoPixel* teamB;

    bool sensorAvailable = false;

    bool sensorEnabled = true;

    float minLux = 0.0f;
    float maxLux = 1000.0f;

    uint8_t minBrightness = 5;
    uint8_t maxBrightness = 255;

    float lastLux = 0.0f;

    uint8_t brightness = 64;

    uint32_t lastUpdate = 0;
};