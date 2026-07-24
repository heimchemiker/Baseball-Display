#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "ScoreboardState.h"
#include "SevenSegmentLED.h"

class ScoreboardDisplay
{
public:

    void begin();

    void render();

    void setState(const ScoreboardState& state);

    ScoreboardState& getState();

private:

    ScoreboardState state;
};