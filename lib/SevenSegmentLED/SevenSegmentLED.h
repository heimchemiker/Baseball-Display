#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class SevenSegmentLED
{
public:

    explicit SevenSegmentLED(
        Adafruit_NeoPixel* strip
    );

    void drawDigit(
        uint16_t startPixel,
        uint8_t digit,
        uint32_t color
    );

    void drawNumber(
        uint16_t startPixel,
        uint8_t digits,
        uint16_t value,
        uint32_t color,
        bool leadingZeros = true
    );

    void drawIndicator(
        uint16_t startPixel,
        uint8_t active,
        uint8_t total,
        uint32_t color
    );

    void drawTime(
        uint16_t startPixel,
        uint8_t hour,
        uint8_t minute,
        uint32_t color
    );

private:

    Adafruit_NeoPixel* strip;

    void setSegment(
        uint16_t startPixel,
        uint8_t segment,
        uint32_t color
    );
};