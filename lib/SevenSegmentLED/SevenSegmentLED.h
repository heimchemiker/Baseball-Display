
#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class SevenSegmentLED
{
public:

    enum Segment
    {
        TOP_LEFT = 0,
        TOP = 1,
        TOP_RIGHT = 2,
        MIDDLE = 3,
        BOTTOM_LEFT = 4,
        BOTTOM = 5,
        BOTTOM_RIGHT = 6
    };

    SevenSegmentLED(Adafruit_NeoPixel* strip);

    void drawDigit(
        uint16_t firstLed,
        uint8_t digit,
        uint32_t color
    );

    void drawNumber(
        uint16_t firstLed,
        uint8_t digits,
        uint16_t value,
        uint32_t color,
        bool leadingZero = true
    );

    void drawTime(
        uint16_t firstLed,
        uint8_t hour,
        uint8_t minute,
        uint32_t color
    );

    void drawIndicator(
        uint16_t firstLed,
        uint8_t active,
        uint8_t count,
        uint32_t color,
        uint32_t offColor = 0
    );

    void clearDigit(uint16_t firstLed);

private:

    Adafruit_NeoPixel* _strip;

    static const bool digitMap[10][7];
};
