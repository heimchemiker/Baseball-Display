#include "RestApi.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

#include "ConfigManager.h"
#include "ScoreboardDisplay.h"
#include "StateSerializer.h"
#include "WebSocketManager.h"
#include "WifiManager.h"

extern ConfigManager configManager;
extern ScoreboardDisplay scoreboard;
extern WebSocketManager websocketManager;
extern WifiManager wifiManager;

void RestApi::begin(
    AsyncWebServer& server)
{
    //
    // GET /api/state
    //

    server.on(
        "/api/state",
        HTTP_GET,
        AsyncWebServerRequest* request
        {
            request->send(
                200,
                "application/json",
                getStateJson()
            );
        }
    );

    //
    // POST /api/state
    //

    server.on(
        "/api/state",
        HTTP_POST,

        AsyncWebServerRequest* request
        {
        },

        nullptr,

        AsyncWebServerRequest* request,
           uint8_t* data,
           size_t len,
           size_t index,
           size_t total
        {
            JsonDocument doc;

            DeserializationError error =
                deserializeJson(
                    doc,
                    data,
                    len
                );

            if(error)
            {
                request->send(
                    400,
                    "application/json",
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

            if(doc["inningsA"].is<JsonArray>())
            {
                JsonArray arr =
                    doc["inningsA"].as<JsonArray>();

                for(size_t i = 0;
                    i < arr.size() && i < 10;
                    i++)
                {
                    state.inningsA[i] =
                        constrain(
                            arr[i].as<int>(),
                            0,
                            99
                        );
                }
            }

            if(doc["inningsB"].is<JsonArray>())
            {
                JsonArray arr =
                    doc["inningsB"].as<JsonArray>();

                for(size_t i = 0;
                    i < arr.size() && i < 10;
                    i++)
                {
                    state.inningsB[i] =
                        constrain(
                            arr[i].as<int>(),
                            0,
                            99
                        );
                }
            }

            if(doc["team"].is<String>() &&
               doc["inning"].is<int>() &&
               doc["value"].is<int>())
            {
                String team =
                    doc["team"].as<String>();

                int inning =
                    doc["inning"].as<int>();

                int value =
                    constrain(
                        doc["value"].as<int>(),
                        0,
                        99
                    );

                if(inning >= 0 &&
                   inning < 10)
                {
                    if(team == "A")
                    {
                        state.inningsA[inning] =
                            value;
                    }

                    if(team == "B")
                    {
                        state.inningsB[inning] =
                            value;
                    }
                }
            }

            scoreboard.render();

            websocketManager.broadcastState();

            request->send(
                200,
                "application/json",
                getStateJson()
            );
        }
    );

    //
    // GET /api/config
    //

    server.on(
        "/api/config",
        HTTP_GET,
        AsyncWebServerRequest* request
        {
            request->send(
                200,
                "application/json",
                configManager.toJson()
            );
        }
    );

    //
    // POST /api/config
    //

    server.on(
        "/api/config",
        HTTP_POST,

        AsyncWebServerRequest* request
        {
        },

        nullptr,

        AsyncWebServerRequest* request,
           uint8_t* data,
           size_t len,
           size_t index,
           size_t total
        {
            String payload;

            payload.reserve(len);

            for(size_t i = 0;
                i < len;
                i++)
            {
                payload +=
                    (char)data[i];
            }

            bool success =
                configManager.fromJson(
                    payload
                );

            if(success)
            {
                configManager.save();

                request->send(
                    200,
                    "application/json",
                    "{\"success\":true}"
                );
            }
            else
            {
                request->send(
                    400,
                    "application/json",
                    "{\"success\":false}"
                );
            }
        }
    );

    //
    // POST /api/restart
    //

    server.on(
        "/api/restart",
        HTTP_POST,
        AsyncWebServerRequest* request
        {
            request->send(
                200,
                "application/json",
                "{\"restart\":true}"
            );

            delay(500);

            ESP.restart();
        }
    );

    //
    // GET /api/wifi/scan
    //

    server.on(
        "/api/wifi/scan",
        HTTP_GET,
        AsyncWebServerRequest* request
        {
            request->send(
                200,
                "application/json",
                wifiManager.scanNetworksJson()
            );
        }
    );
}