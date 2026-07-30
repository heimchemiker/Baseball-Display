#include "TimeManager.h"

#include <Wire.h>

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

    ntpClient =
        new NTPClient(
            ntpUdp,
            configManager.config()
                .ntpServer.c_str(),
            0,
            60000
        );

    ntpClient->begin();

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
    if(!ntpClient)
    {
        return;
    }

    ntpClient->update();

    unsigned long epoch =
        ntpClient->getEpochTime();

    uint32_t offsetSeconds = 0;

    /*
     * Optional:
     * UTC-Offset aus Config übernehmen
     */

    epoch += offsetSeconds;

    currentHour =
        (epoch % 86400L) / 3600;

    currentMinute =
        (epoch % 3600) / 60;

    if(rtcPresent)
    {
        rtc.adjust(
            DateTime(epoch)
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