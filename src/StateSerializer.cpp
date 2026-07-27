#include "StateSerializer.h"

#include <ArduinoJson.h>
#include <WiFi.h>

#include "ScoreboardDisplay.h"
#include "ConfigManager.h"
#include "MqttManager.h"

extern ScoreboardDisplay scoreboard;
extern ConfigManager configManager;
extern MqttManager mqttManager;

String getStateJson()
{
    JsonDocument doc;

    auto& state =
        scoreboard.state();

    //
    // Allgemein
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
        doc["inningsA"].to<JsonArray>();

    for(uint8_t i = 0; i < 10; i++)
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
        doc["inningsB"].to<JsonArray>();

    for(uint8_t i = 0; i < 10; i++)
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

    doc["hour"] =
        scoreboard.getHour();

    doc["minute"] =
        scoreboard.getMinute();

    //
    // Theme
    //

    doc["theme"] =
        configManager.config().theme;

    //
    // Farben
    //

    JsonObject colors =
        doc["colors"].to<JsonObject>();

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
        configManager.config().highlightCurrentInning;

    //
    // WLAN
    //

    JsonObject wifi =
        doc["wifi"].to<JsonObject>();

    wifi["connected"] =
        (WiFi.status() == WL_CONNECTED);

    wifi["ssid"] =
        WiFi.SSID();

    wifi["ip"] =
        WiFi.localIP().toString();

    wifi["rssi"] =
        WiFi.RSSI();

    //
    // MQTT
    //

    JsonObject mqtt =
        doc["mqtt"].to<JsonObject>();

    mqtt["connected"] =
        mqttManager.connected();

    //
    // Helligkeit
    //

    JsonObject brightness =
        doc["brightness"]
            .to<JsonObject>();

    brightness["sensorEnabled"] =
        configManager.config()
            .brightnessSensor;

    brightness["current"] =
        configManager.currentBrightness();

    //
    // Zeitquelle
    //

    JsonObject time =
        doc["time"].to<JsonObject>();

    time["source"] =
        configManager.config()
            .ntpEnabled
                ? "ntp"
                : "manual";

    time["ntpServer"] =
        configManager.config()
            .ntpServer;

    //
    // Speicherstatus
    //

    JsonObject system =
        doc["system"].to<JsonObject>();

    system["freeHeap"] =
        ESP.getFreeHeap();

    system["uptime"] =
        millis();

    String json;

    serializeJson(
        doc,
        json
    );

    return json;
}