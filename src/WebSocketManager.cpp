#include "WebSocketManager.h"

#include <Arduino.h>
#include <ArduinoJson.h>

#include "StateSerializer.h"
#include "ScoreboardDisplay.h"
#include "MqttManager.h"

extern ScoreboardDisplay scoreboard;
extern MqttManager mqttManager;

WebSocketManager::WebSocketManager()
:
ws("/ws")
{
}

void WebSocketManager::begin(
    AsyncWebServer& server)
{
    ws.onEvent(
        [this]
        (
            AsyncWebSocket* server,
            AsyncWebSocketClient* client,
            AwsEventType type,
            void* arg,
            uint8_t* data,
            size_t len
        )
        {
            switch(type)
            {
                case WS_EVT_CONNECT:
                {
                    Serial.printf(
                        "[WS] Client #%u connected\n",
                        client->id()
                    );

                    client->text(
                        getStateJson()
                    );

                    break;
                }

                case WS_EVT_DISCONNECT:
                {
                    Serial.printf(
                        "[WS] Client #%u disconnected\n",
                        client->id()
                    );

                    break;
                }

                case WS_EVT_ERROR:
                {
                    Serial.printf(
                        "[WS] Client #%u error\n",
                        client->id()
                    );

                    break;
                }

                case WS_EVT_PONG:
                {
                    break;
                }

                case WS_EVT_DATA:
                {
                    AwsFrameInfo *info =
                        (AwsFrameInfo*)arg;

                    if(
                        info->final &&
                        info->index == 0 &&
                        info->opcode == WS_TEXT
                    )
                    {
                        Serial.printf(
                            "[WS] Received %u bytes from client #%u\n",
                            (unsigned)len,
                            client->id()
                        );

                        String message;

                        for(size_t i = 0;
                            i < len;
                            i++)
                        {
                            message +=
                                (char)data[i];
                        }

                        Serial.printf(
                            "[WS] Message from #%u: %s\n",
                            client->id(),
                            message.c_str()
                        );

                        handleMessage(
                            client,
                            message
                        );
                    }

                    break;
                }
            }
        });

    server.addHandler(&ws);
}

AsyncWebSocket&
WebSocketManager::socket()
{
    return ws;
}

void WebSocketManager::broadcastState()
{
    ws.textAll(
        getStateJson()
    );
}

void WebSocketManager::handleMessage(
    AsyncWebSocketClient* client,
    const String& message)
{
    JsonDocument doc;

    DeserializationError error =
        deserializeJson(
            doc,
            message
        );

    if(error != DeserializationError::Ok)
    {
        client->text(
            "{\"success\":false,\"error\":\"invalid json\"}"
        );

        return;
    }

    auto& state =
        scoreboard.state();

    if(doc["atBat"].is<int>())
    {
        state.batter =
            constrain(
                doc["atBat"].as<int>(),
                0,
                99
            );
    }

    if(doc["balls"].is<int>())
    {
        state.balls =
            constrain(
                doc["balls"].as<int>(),
                0,
                3
            );
    }

    if(doc["strikes"].is<int>())
    {
        state.strikes =
            constrain(
                doc["strikes"].as<int>(),
                0,
                2
            );
    }

    if(doc["outs"].is<int>())
    {
        state.outs =
            constrain(
                doc["outs"].as<int>(),
                0,
                2
            );
    }

    if(doc["hitsA"].is<int>())
    {
        state.hitsA =
            constrain(
                doc["hitsA"].as<int>(),
                0,
                99
            );
    }

    if(doc["hitsB"].is<int>())
    {
        state.hitsB =
            constrain(
                doc["hitsB"].as<int>(),
                0,
                99
            );
    }

    if(doc["errorsA"].is<int>())
    {
        state.errorsA =
            constrain(
                doc["errorsA"].as<int>(),
                0,
                99
            );
    }

    if(doc["errorsB"].is<int>())
    {
        state.errorsB =
            constrain(
                doc["errorsB"].as<int>(),
                0,
                99
            );
    }

    if(doc["currentInning"].is<int>())
    {
        scoreboard.setCurrentInning(
            constrain(
                doc["currentInning"].as<int>(),
                1,
                10
            )
        );
    }

    if(doc["inningsA"].is<JsonArray>())
    {
        JsonArray innings =
            doc["inningsA"].as<JsonArray>();

        for(size_t i = 0;
            i < innings.size() &&
            i < 10;
            i++)
        {
            state.inningsA[i] =
                constrain(
                    innings[i].as<int>(),
                    0,
                    99
                );
        }
    }

    if(doc["inningsB"].is<JsonArray>())
    {
        JsonArray innings =
            doc["inningsB"].as<JsonArray>();

        for(size_t i = 0;
            i < innings.size() &&
            i < 10;
            i++)
        {
            state.inningsB[i] =
                constrain(
                    innings[i].as<int>(),
                    0,
                    99
                );
        }
    }

    if(doc["cmd"].is<String>())
    {
        String cmd =
            doc["cmd"].as<String>();

        if(cmd == "resetCount")
        {
            state.balls = 0;
            state.strikes = 0;
            state.outs = 0;
        }

        if(cmd == "clearGame")
        {
            memset(
                state.inningsA,
                0,
                sizeof(state.inningsA)
            );

            memset(
                state.inningsB,
                0,
                sizeof(state.inningsB)
            );

            state.hitsA = 0;
            state.hitsB = 0;

            state.errorsA = 0;
            state.errorsB = 0;

            state.balls = 0;
            state.strikes = 0;
            state.outs = 0;
        }
    }

    scoreboard.render();

    mqttManager.publishState();

    broadcastState();

    client->text(
        "{\"success\":true}"
    );
}