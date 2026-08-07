#include "MqttManager.h"

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#include "ConfigManager.h"
#include "ScoreboardDisplay.h"
#include "WebSocketManager.h"

extern ConfigManager configManager;
extern ScoreboardDisplay scoreboard;
extern WebSocketManager websocketManager;

static MqttManager* mqttInstance = nullptr;

static void mqttCallback(
    char* topic,
    byte* payload,
    unsigned int length)
{
    if(mqttInstance)
    {
        mqttInstance->handleMessage(
            topic,
            payload,
            length
        );
    }
}

MqttManager::MqttManager()
:
mqttClient(wifiClient)
{
}

void MqttManager::begin()
{
    mqttInstance = this;

    mqttClient.setServer(
        configManager.config().mqttHost.c_str(),
        configManager.config().mqttPort
    );

    mqttClient.setCallback(
        mqttCallback
    );
}

bool MqttManager::connected()
{
    return mqttClient.connected();
}

void MqttManager::loop()
{
    if(!mqttClient.connected())
    {
        reconnect();
    }

    mqttClient.loop();
}

void MqttManager::reconnect()
{
    if(mqttClient.connected())
    {
        return;
    }

    String clientId =
        "BaseballScoreboard-" +
        String(
            (uint64_t)ESP.getEfuseMac(),
            HEX
        );

    bool success = false;

    if(configManager.config().mqttUser.length())
    {
        success =
            mqttClient.connect(
                clientId.c_str(),
                configManager.config().mqttUser.c_str(),
                configManager.config().mqttPassword.c_str(),
                "baseball/status",
                0,
                true,
                "offline"
            );
    }
    else
    {
        success =
            mqttClient.connect(
                clientId.c_str(),
                nullptr,
                nullptr,
                "baseball/status",
                0,
                true,
                "offline"
            );
    }

    if(!success)
    {
        return;
    }

    mqttClient.publish(
        "baseball/status",
        "online",
        true
    );

    mqttClient.subscribe(
        "baseball/cmd"
    );

    mqttClient.subscribe(
        "baseball/batter/set"
    );

    mqttClient.subscribe(
        "baseball/balls/set"
    );

    mqttClient.subscribe(
        "baseball/strikes/set"
    );

    mqttClient.subscribe(
        "baseball/outs/set"
    );

    mqttClient.subscribe(
        "baseball/teama/hits/set"
    );

    mqttClient.subscribe(
        "baseball/teama/errors/set"
    );

    mqttClient.subscribe(
        "baseball/teamb/hits/set"
    );

    mqttClient.subscribe(
        "baseball/teamb/errors/set"
    );

    for(int inning = 1;
        inning <= 10;
        inning++)
    {
        mqttClient.subscribe(
            (
                "baseball/teama/inning/" +
                String(inning) +
                "/set"
            ).c_str()
        );

        mqttClient.subscribe(
            (
                "baseball/teamb/inning/" +
                String(inning) +
                "/set"
            ).c_str()
        );
    }

    publishDiscovery();
    publishState();
}

void MqttManager::publishDiscoveryEntity(
    const String& uniqueId,
    const String& name,
    const String& stateTopic,
    const String& commandTopic,
    const String& icon,
    int minValue,
    int maxValue)
{
    JsonDocument doc;

    doc["name"] = name;
    doc["unique_id"] = uniqueId;

    doc["state_topic"] = stateTopic;
    doc["command_topic"] = commandTopic;

    doc["availability_topic"] =
        "baseball/status";

    doc["icon"] = icon;

    doc["mode"] = "box";
    doc["min"] = minValue;
    doc["max"] = maxValue;
    doc["step"] = 1;

    JsonObject device =
        doc["device"].to<JsonObject>();

    device["name"] =
        "Baseball Scoreboard";

    device["manufacturer"] =
        "Hillbrandt";

    device["model"] =
        "ESP32 Baseball Scoreboard";

    JsonArray identifiers =
        device["identifiers"]
            .to<JsonArray>();

    identifiers.add(
        "baseball_scoreboard"
    );

    String payload;

    serializeJson(
        doc,
        payload
    );

    String topic =
        "homeassistant/number/" +
        uniqueId +
        "/config";

    mqttClient.publish(
        topic.c_str(),
        payload.c_str(),
        true
    );
}

void MqttManager::publishDiscovery()
{
    publishDiscoveryEntity(
        "scoreboard_batter",
        "At Bat",
        "baseball/state/batter",
        "baseball/batter/set",
        "mdi:baseball",
        0,
        99
    );

    publishDiscoveryEntity(
        "scoreboard_balls",
        "Balls",
        "baseball/state/balls",
        "baseball/balls/set",
        "mdi:circle",
        0,
        3
    );

    publishDiscoveryEntity(
        "scoreboard_strikes",
        "Strikes",
        "baseball/state/strikes",
        "baseball/strikes/set",
        "mdi:circle-outline",
        0,
        2
    );

    publishDiscoveryEntity(
        "scoreboard_outs",
        "Outs",
        "baseball/state/outs",
        "baseball/outs/set",
        "mdi:close-circle",
        0,
        2
    );

    for(int inning = 1;
        inning <= 10;
        inning++)
    {
        publishDiscoveryEntity(
            "teama_inning_" +
            String(inning),

            "Team A Inning " +
            String(inning),

            "baseball/state/teama/inning/" +
            String(inning),

            "baseball/teama/inning/" +
            String(inning) +
            "/set",

            "mdi:baseball-diamond",

            0,
            99
        );

        publishDiscoveryEntity(
            "teamb_inning_" +
            String(inning),

            "Team B Inning " +
            String(inning),

            "baseball/state/teamb/inning/" +
            String(inning),

            "baseball/teamb/inning/" +
            String(inning) +
            "/set",

            "mdi:baseball-diamond",

            0,
            99
        );
    }

    publishDiscoveryEntity(
        "teama_hits",
        "Team A Hits",
        "baseball/state/teama/hits",
        "baseball/teama/hits/set",
        "mdi:counter",
        0,
        99
    );

    publishDiscoveryEntity(
        "teama_errors",
        "Team A Errors",
        "baseball/state/teama/errors",
        "baseball/teama/errors/set",
        "mdi:alert-circle",
        0,
        99
    );

    publishDiscoveryEntity(
        "teamb_hits",
        "Team B Hits",
        "baseball/state/teamb/hits",
        "baseball/teamb/hits/set",
        "mdi:counter",
        0,
        99
    );

    publishDiscoveryEntity(
        "teamb_errors",
        "Team B Errors",
        "baseball/state/teamb/errors",
        "baseball/teamb/errors/set",
        "mdi:alert-circle",
        0,
        99
    );
}

void MqttManager::publishState()
{
    auto& state =
        scoreboard.state();

    auto publishNumber =
        [this](const char *topic, int value)
    {
        String payload(value);

        mqttClient.publish(
            topic,
            payload.c_str(),
            true
        );
    };

    publishNumber(
        "baseball/state/batter",
        state.batter
    );

    publishNumber(
        "baseball/state/balls",
        state.balls
    );

    publishNumber(
        "baseball/state/strikes",
        state.strikes
    );

    publishNumber(
        "baseball/state/outs",
        state.outs
    );

    publishNumber(
        "baseball/state/teama/hits",
        state.hitsA
    );

    publishNumber(
        "baseball/state/teama/errors",
        state.errorsA
    );

    publishNumber(
        "baseball/state/teamb/hits",
        state.hitsB
    );

    publishNumber(
        "baseball/state/teamb/errors",
        state.errorsB
    );

    for(int i = 0; i < 10; i++)
    {
        String topicA =
            "baseball/state/teama/inning/" +
            String(i + 1);

        String topicB =
            "baseball/state/teamb/inning/" +
            String(i + 1);

        String payloadA(
            state.inningsA[i]
        );

        String payloadB(
            state.inningsB[i]
        );

        mqttClient.publish(
            topicA.c_str(),
            payloadA.c_str(),
            true
        );

        mqttClient.publish(
            topicB.c_str(),
            payloadB.c_str(),
            true
        );
    }
}

void MqttManager::handleMessage(
    char* topic,
    byte* payload,
    unsigned int length)
{
    String value;

    for(unsigned int i = 0;
        i < length;
        i++)
    {
        value +=
            (char)payload[i];
    }

    auto& state =
        scoreboard.state();

    String t(topic);

    if(t == "baseball/balls/set")
    {
        state.balls =
            constrain(
                value.toInt(),
                0,
                3
            );
    }
    else if(t == "baseball/strikes/set")
    {
        state.strikes =
            constrain(
                value.toInt(),
                0,
                2
            );
    }
    else if(t == "baseball/outs/set")
    {
        state.outs =
            constrain(
                value.toInt(),
                0,
                2
            );
    }
    else if(t == "baseball/batter/set")
    {
        state.batter =
            constrain(
                value.toInt(),
                0,
                99
            );
    }
    else if(t == "baseball/teama/hits/set")
    {
        state.hitsA =
            constrain(
                value.toInt(),
                0,
                99
            );
    }
    else if(t == "baseball/teama/errors/set")
    {
        state.errorsA =
            constrain(
                value.toInt(),
                0,
                99
            );
    }
    else if(t == "baseball/teamb/hits/set")
    {
        state.hitsB =
            constrain(
                value.toInt(),
                0,
                99
            );
    }
    else if(t == "baseball/teamb/errors/set")
    {
        state.errorsB =
            constrain(
                value.toInt(),
                0,
                99
            );
    }
    else if(t.startsWith(
        "baseball/teama/inning/"))
    {
        int inning =
            t.substring(
                22,
                t.lastIndexOf('/')
            ).toInt();

        if(inning >= 1 &&
           inning <= 10)
        {
            state.inningsA[
                inning - 1
            ] =
            constrain(
                value.toInt(),
                0,
                99
            );
        }
    }
    else if(t.startsWith(
        "baseball/teamb/inning/"))
    {
        int inning =
            t.substring(
                22,
                t.lastIndexOf('/')
            ).toInt();

        if(inning >= 1 &&
           inning <= 10)
        {
            state.inningsB[
                inning - 1
            ] =
            constrain(
                value.toInt(),
                0,
                99
            );
        }
    }
    else if(t == "baseball/cmd")
    {
        JsonDocument cmd;

        if(deserializeJson(
            cmd,
            value) == DeserializationError::Ok)
        {
            String action =
                cmd["action"] | "";

            if(action == "resetCount")
            {
                state.balls = 0;
                state.strikes = 0;
                state.outs = 0;
            }
        }
    }

    scoreboard.render();

    websocketManager.broadcastState();

    publishState();
}