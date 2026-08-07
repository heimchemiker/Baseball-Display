#pragma once

#include <Arduino.h>
#include <RTClib.h>

class TimeManager
{
public:

    bool begin();

    void update();

    void setManualTime(
        uint8_t hour,
        uint8_t minute
    );

    uint8_t hour() const;

    uint8_t minute() const;

    bool rtcAvailable() const;

private:

    void updateFromRtc();

    void updateFromNtp();

private:

    RTC_DS3231 rtc;

    bool rtcPresent = false;

    uint32_t lastUpdate = 0;

    uint32_t lastNtpSync = 0;

    uint8_t currentHour = 0;
    uint8_t currentMinute = 0;

    bool manualMode = false;
};
