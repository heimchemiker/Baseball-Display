#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "SevenSegmentLED.h"
#include "ScoreboardState.h"

uint8_t getHour() const;
uint8_t getMinute() const;
uint8_t getCurrentInning() const;

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

    void setColorTeamA(
        uint32_t color
    );

    void setColorTeamB(
        uint32_t color
    );

    void setColorClock(
        uint32_t color
    );

    void setColorIndicator(
        uint32_t balls,
        uint32_t strikes,
        uint32_t outs
    );

    void setHighlightColor(
        uint32_t color
    );

private:

    void calculateTotals();

    void renderTop();
    void renderTeamA();
    void renderTeamB();

    ScoreboardState scoreboardState;

    Adafruit_NeoPixel* top;
    Adafruit_NeoPixel* teamA;
    Adafruit_NeoPixel* teamB;

    SevenSegmentLED topDisplay;
    SevenSegmentLED displayA;
    SevenSegmentLED displayB;

    uint8_t currentHour = 0;
    uint8_t currentMinute = 0;

    uint8_t currentInning = 1;

    uint32_t colorTeamA;
    uint32_t colorTeamB;

    uint32_t colorClock;

    uint32_t colorBalls;
    uint32_t colorStrikes;
    uint32_t colorOuts;

    uint32_t colorHighlight;
};
