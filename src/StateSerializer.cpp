#include "StateSerializer.h"

#include <Arduino.h>
#include <ArduinoJson.h>

#include "ScoreboardDisplay.h"
#include "ConfigManager.h"
#include "MqttManager.h"
#include "WifiManager.h"
#include "TimeManager.h"
#include "BrightnessManager.h"

extern ScoreboardDisplay scoreboard;
extern ConfigManager configManager;
extern MqttManager mqttManager;
extern WifiManager wifiManager;
extern TimeManager timeManager;
extern BrightnessManager brightnessManager;

String getStateJson()
{
    JsonDocument doc;

    auto& state =
        scoreboard.state();

    //
    // Spielstand
    //

    doc["atBat"] =
        state.batter;

    doc["balls"] =
        state.balls;

    doc["strikes"] =
        state.strikes;

    doc["outs"] =
        state.outs;

    //
    // Team A
    //

    JsonArray inningsA =
        doc["inningsA"]
            .to<JsonArray>();

    for(uint8_t i = 0;
        i < 10;
        i++)
    {
        inningsA.add(
            state.inningsA[i]
        );
    }

    doc["runsA"] =
        state.runsA;

    doc["hitsA"] =
        state.hitsA;

    doc["errorsA"] =
        state.errorsA;

    //
    // Team B
    //

    JsonArray inningsB =
        doc["inningsB"]
            .to<JsonArray>();

    for(uint8_t i = 0;
        i < 10;
        i++)
    {
        inningsB.add(
            state.inningsB[i]
        );
    }

    doc["runsB"] =
        state.runsB;

    doc["hitsB"] =
        state.hitsB;

    doc["errorsB"] =
        state.errorsB;

    //
    // Anzeige
    //

    doc["currentInning"] =
        scoreboard.getCurrentInning();

    //
    // Uhrzeit
    //

    doc["hour"] =
        timeManager.hour();

    doc["minute"] =
        timeManager.minute();

    doc["rtcAvailable"] =
        timeManager.rtcAvailable();

    //
    // Theme
    //

    doc["theme"] =
        configManager.config().theme;

    //
    // Farben
    //

    JsonObject colors =
        doc["colors"]
            .to<JsonObject>();

    colors["global"] =
        configManager.config().colorGlobal;

    colors["teamA"] =
        configManager.config().colorTeamA;

    colors["teamB"] =
        configManager.config().colorTeamB;

    colors["clock"] =
        configManager.config().colorClock;

    colors["balls"] =
        configManager.config().colorBalls;

    colors["strikes"] =
        configManager.config().colorStrikes;

    colors["outs"] =
        configManager.config().colorOuts;

    colors["currentInning"] =
        configManager.config()
            .highlightCurrentInning;

    //
    // WLAN
    //

    JsonObject wifi =
        doc["wifi"]
            .to<JsonObject>();

    wifi["connected"] =
        wifiManager.connected();

    wifi["ssid"] =
        wifiManager.ssid();

    wifi["ip"] =
        wifiManager.ipAddress();

    wifi["rssi"] =
        wifiManager.rssi();

    //
    // MQTT
    //

    JsonObject mqtt =
        doc["mqtt"]
            .to<JsonObject>();

    mqtt["connected"] =
        mqttManager.connected();

    //
    // Helligkeit
    //

    JsonObject brightness =
        doc["brightness"]
            .to<JsonObject>();

    brightness["enabled"] =
        brightnessManager.enabled();

    brightness["lux"] =
        brightnessManager.currentLux();

    brightness["value"] =
        brightnessManager.currentBrightness();

    //
    // Zeitkonfiguration
    //

    JsonObject timeConfig =
        doc["time"]
            .to<JsonObject>();

    timeConfig["ntpEnabled"] =
        configManager.config()
            .ntpEnabled;

    timeConfig["ntpServer"] =
        configManager.config()
            .ntpServer;

    //
    // System
    //

    JsonObject system =
        doc["system"]
            .to<JsonObject>();

    system["freeHeap"] =
        ESP.getFreeHeap();

    system["uptime"] =
        millis();

    //
    // JSON erzeugen
    //

    String json;

    serializeJson(
        doc,
        json
    );

    return json;
}