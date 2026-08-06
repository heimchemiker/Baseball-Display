#pragma once

#include <Adafruit_NeoPixel.h>

#include "ScoreboardState.h"
#include "SevenSegmentLED.h"

class ScoreboardDisplay
{
public:

    ScoreboardDisplay(
        Adafruit_NeoPixel* topStrip,
        Adafruit_NeoPixel* teamAStrip,
        Adafruit_NeoPixel* teamBStrip
    );

    void begin();

    void render();

    ScoreboardState& state();

    void setClock(
        uint8_t hour,
        uint8_t minute
    );

    void setCurrentInning(
        uint8_t inning
    );

    uint8_t getCurrentInning() const;

private:

    void calculateTotals();

    Adafruit_NeoPixel* top;
    Adafruit_NeoPixel* teamA;
    Adafruit_NeoPixel* teamB;

    SevenSegmentLED topDisplay;
    SevenSegmentLED displayA;
    SevenSegmentLED displayB;

    ScoreboardState scoreboardState;

    uint8_t currentHour = 0;
    uint8_t currentMinute = 0;
    uint8_t currentInning = 1;

    uint32_t colorTeamA;
    uint32_t colorTeamB;
    uint32_t colorClock;
};