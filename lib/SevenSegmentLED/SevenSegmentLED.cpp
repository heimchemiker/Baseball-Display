#include "SevenSegmentLED.h"

constexpr uint8_t DIGIT_SEGMENTS = 7;
constexpr uint8_t LEDS_PER_SEGMENT = 2;
constexpr uint8_t LEDS_PER_DIGIT =
    DIGIT_SEGMENTS *
    LEDS_PER_SEGMENT;

static const bool digitMap[10][7] =
{
    {1,1,1,1,1,1,0},
    {0,1,1,0,0,0,0},
    {1,1,0,1,1,0,1},
    {1,1,1,1,0,0,1},
    {0,1,1,0,0,1,1},
    {1,0,1,1,0,1,1},
    {1,0,1,1,1,1,1},
    {1,1,1,0,0,0,0},
    {1,1,1,1,1,1,1},
    {1,1,1,1,0,1,1}
};

SevenSegmentLED::SevenSegmentLED(
    Adafruit_NeoPixel* s)
:
strip(s)
{
}

void SevenSegmentLED::setSegment(
    uint16_t startPixel,
    uint8_t segment,
    uint32_t color)
{
    uint16_t offset =
        startPixel +
        segment * LEDS_PER_SEGMENT;

    for(uint8_t i=0;i<LEDS_PER_SEGMENT;i++)
    {
        strip->setPixelColor(
            offset + i,
            color
        );
    }
}

void SevenSegmentLED::drawDigit(
    uint16_t startPixel,
    uint8_t digit,
    uint32_t color)
{
    if(digit > 9)
    {
        return;
    }

    for(uint8_t seg=0; seg<7; seg++)
    {
        uint32_t c =
            digitMap[digit][seg]
            ? color
            : 0;

        setSegment(
            startPixel,
            seg,
            c
        );
    }
}

void SevenSegmentLED::drawNumber(
    uint16_t startPixel,
    uint8_t digits,
    uint16_t value,
    uint32_t color,
    bool leadingZeros)
{
    for(int i=digits-1;i>=0;i--)
    {
        uint8_t digit =
            value % 10;

        value /= 10;

        drawDigit(
            startPixel +
            i*LEDS_PER_DIGIT,
            digit,
            color
        );
    }
}

void SevenSegmentLED::drawIndicator(
    uint16_t startPixel,
    uint8_t active,
    uint8_t total,
    uint32_t color)
{
    for(uint8_t i=0;i<total;i++)
    {
        strip->setPixelColor(
            startPixel+i,
            i<active ? color : 0
        );
    }
}

void SevenSegmentLED::drawTime(
    uint16_t startPixel,
    uint8_t hour,
    uint8_t minute,
    uint32_t color)
{
    drawNumber(
        startPixel,
        2,
        hour,
        color
    );

    drawNumber(
        startPixel +
        2*LEDS_PER_DIGIT,
        2,
        minute,
        color
    );
}