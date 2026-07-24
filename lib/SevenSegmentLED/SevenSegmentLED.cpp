
#include "SevenSegmentLED.h"

const bool SevenSegmentLED::digitMap[10][7] =
{
    {1,1,1,0,1,1,1}, // 0
    {0,0,1,0,0,0,1}, // 1
    {0,1,1,1,1,1,0}, // 2
    {0,1,1,1,0,1,1}, // 3
    {1,0,1,1,0,0,1}, // 4
    {1,1,0,1,0,1,1}, // 5
    {1,1,0,1,1,1,1}, // 6
    {0,1,1,0,0,0,1}, // 7
    {1,1,1,1,1,1,1}, // 8
    {1,1,1,1,0,1,1}  // 9
};

SevenSegmentLED::SevenSegmentLED(
    Adafruit_NeoPixel* strip
)
{
    _strip = strip;
}

void SevenSegmentLED::clearDigit(uint16_t firstLed)
{
    for(uint8_t i = 0; i < 7; i++)
    {
        _strip->setPixelColor(firstLed + i, 0);
    }
}

void SevenSegmentLED::drawDigit(
    uint16_t firstLed,
    uint8_t digit,
    uint32_t color
)
{
    if(digit > 9)
    {
        clearDigit(firstLed);
        return;
    }

    for(uint8_t segment = 0; segment < 7; segment++)
    {
        _strip->setPixelColor(
            firstLed + segment,
            digitMap[digit][segment] ? color : 0
        );
    }
}

void SevenSegmentLED::drawNumber(
    uint16_t firstLed,
    uint8_t digits,
    uint16_t value,
    uint32_t color,
    bool leadingZero
)
{
    for(int8_t pos = digits - 1; pos >= 0; pos--)
    {
        uint8_t digit = value % 10;
        value /= 10;

        uint16_t led =
            firstLed +
            (pos * 7);

        if(
            !leadingZero &&
            value == 0 &&
            digit == 0 &&
            pos != digits - 1
        )
        {
            clearDigit(led);
        }
        else
        {
            drawDigit(led, digit, color);
        }
    }
}

void SevenSegmentLED::drawTime(
    uint16_t firstLed,
    uint8_t hour,
    uint8_t minute,
    uint32_t color
)
{
    drawDigit(firstLed + 0, hour / 10, color);
    drawDigit(firstLed + 7, hour % 10, color);

    drawDigit(firstLed + 14, minute / 10, color);
    drawDigit(firstLed + 21, minute % 10, color);
}

void SevenSegmentLED::drawIndicator(
    uint16_t firstLed,
    uint8_t active,
    uint8_t count,
    uint32_t color,
    uint32_t offColor
)
{
    for(uint8_t i = 0; i < count; i++)
    {
        if(i < active)
        {
            _strip->setPixelColor(
                firstLed + i,
                color
            );
        }
        else
        {
            _strip->setPixelColor(
                firstLed + i,
                offColor
            );
        }
    }
}
