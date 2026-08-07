#include "TimeManager.h"

#include <Wire.h>
#include <time.h>
#include <WiFi.h>

#include "ConfigManager.h"
#include "ScoreboardDisplay.h"

extern ConfigManager configManager;
extern ScoreboardDisplay scoreboard;

bool TimeManager::begin()
{
    Wire.begin();

    rtcPresent = rtc.begin();

    if(rtcPresent)
    {
        if(rtc.lostPower())
        {
            rtc.adjust(
                DateTime(
                    F(__DATE__),
                    F(__TIME__)
                )
            );
        }
    }

    if(configManager.config().ntpEnabled &&
        configManager.config().ntpServer.length()
    ){
    configTime(
        3600, // UTC+1
        3600, // Sommerzeit
        configManager.config()
            .ntpServer.c_str()
    );
    }
    
    if(configManager.config().ntpEnabled)
    {
        updateFromNtp();
    }
    else
    {
        updateFromRtc();
    }

    scoreboard.setClock(
        currentHour,
        currentMinute
    );

    return true;
}

void TimeManager::setManualTime(
    uint8_t hour,
    uint8_t minute)
{
    manualMode = true;

    currentHour = hour;
    currentMinute = minute;

    scoreboard.setClock(
        currentHour,
        currentMinute
    );

    if(rtcPresent)
    {
        DateTime now =
            rtc.now();

        rtc.adjust(
            DateTime(
                now.year(),
                now.month(),
                now.day(),
                hour,
                minute,
                0
            )
        );
    }
}

void TimeManager::update()
{
    uint32_t now =
        millis();

    if(now - lastUpdate < 1000)
    {
        return;
    }

    lastUpdate = now;

    if(manualMode)
    {
        scoreboard.setClock(
            currentHour,
            currentMinute
        );

        return;
    }

    if(configManager.config().ntpEnabled)
    {
        if(now - lastNtpSync > 300000)
        {
            updateFromNtp();

            lastNtpSync = now;
        }
        else
        {
            updateFromRtc();
        }
    }
    else
    {
        updateFromRtc();
    }

    scoreboard.setClock(
        currentHour,
        currentMinute
    );
}

void TimeManager::updateFromRtc()
{
    if(!rtcPresent)
    {
        return;
    }

    DateTime now =
        rtc.now();

    currentHour =
        now.hour();

    currentMinute =
        now.minute();
}

void TimeManager::updateFromNtp()
{
    struct tm timeinfo;

    if(!getLocalTime(&timeinfo))
    {
        return;
    }

    currentHour =
        timeinfo.tm_hour;

    currentMinute =
        timeinfo.tm_min;

    if(rtcPresent)
    {
        rtc.adjust(
            DateTime(
                timeinfo.tm_year + 1900,
                timeinfo.tm_mon + 1,
                timeinfo.tm_mday,
                timeinfo.tm_hour,
                timeinfo.tm_min,
                timeinfo.tm_sec
            )
        );
    }
}

uint8_t TimeManager::hour() const
{
    return currentHour;
}

uint8_t TimeManager::minute() const
{
    return currentMinute;
}

bool TimeManager::rtcAvailable() const
{
    return rtcPresent;
}