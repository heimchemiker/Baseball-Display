#include "RestApi.h"

#include <ArduinoJson.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#include "StateSerializer.h"
#include "ScoreboardDisplay.h"
#include "ConfigManager.h"
#include "WebSocketManager.h"

extern ScoreboardDisplay scoreboard;
extern ConfigManager configManager;
extern WebSocketManager websocketManager;
extern WifiManager wifiManager;

void RestApi::begin(
    AsyncWebServer& server)
{
    /*
     * GET /api/state
     */

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
        });

    /*
     * POST /api/state
     */

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

            DeserializationError err =
                deserializeJson(
                    doc,
                    data,
                    len);

            if(err)
            {
                request->send(
                    400,
                    "application/json",
                    "{\"error\":\"invalid json\"}"
                );
                return;
            }

            auto& state =
                scoreboard.state();

            if(doc["atBat"].is<int>())
                state.batter =
                    doc["atBat"];

            if(doc["balls"].is<int>())
                state.balls =
                    constrain(
                        doc["balls"],
                        0,
                        3);

            if(doc["strikes"].is<int>())
                state.strikes =
                    constrain(
                        doc["strikes"],
                        0,
                        2);

            if(doc["outs"].is<int>())
                state.outs =
                    constrain(
                        doc["outs"],
                        0,
                        2);

            if(doc["hitsA"].is<int>())
                state.hitsA =
                    doc["hitsA"];

            if(doc["hitsB"].is<int>())
                state.hitsB =
                    doc["hitsB"];

            if(doc["errorsA"].is<int>())
                state.errorsA =
                    doc["errorsA"];

            if(doc["errorsB"].is<int>())
                state.errorsB =
                    doc["errorsB"];

            if(doc["inningsA"].is<JsonArray>())
            {
                JsonArray arr =
                    doc["inningsA"];

                for(size_t i = 0;
                    i < arr.size() && i < 10;
                    i++)
                {
                    state.inningsA[i] =
                        arr[i];
                }
            }

            if(doc["inningsB"].is<JsonArray>())
            {
                JsonArray arr =
                    doc["inningsB"];

                for(size_t i = 0;
                    i < arr.size() && i < 10;
                    i++)
                {
                    state.inningsB[i] =
                        arr[i];
                }
            }

            /*
             * Einzelne Inning-Werte
             */

            if(doc["team"].is<String>() &&
               doc["inning"].is<int>() &&
               doc["value"].is<int>())
            {
                String team =
                    doc["team"];

                int inning =
                    doc["inning"];

                int value =
                    constrain(
                        doc["value"],
                        0,
                        99);

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

            /*
             * Hits / Errors +/- Updates
             */

            if(doc["team"].is<String>() &&
               doc["stat"].is<String>() &&
               doc["delta"].is<int>())
            {
                String team =
                    doc["team"];

                String stat =
                    doc["stat"];

                int delta =
                    doc["delta"];

                if(team == "A")
                {
                    if(stat == "hits")
                    {
                        state.hitsA =
                            max(
                                0,
                                state.hitsA + delta
                            );
                    }

                    if(stat == "errors")
                    {
                        state.errorsA =
                            max(
                                0,
                                state.errorsA + delta
                            );
                    }
                }

                if(team == "B")
                {
                    if(stat == "hits")
                    {
                        state.hitsB =
                            max(
                                0,
                                state.hitsB + delta
                            );
                    }

                    if(stat == "errors")
                    {
                        state.errorsB =
                            max(
                                0,
                                state.errorsB + delta
                            );
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
        });

    /*
     * GET /api/config
     */

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
        });

    /*
     * POST /api/config
     */

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
            bool result =
                configManager.fromJson(
                    (const char*)data
                );

            if(result)
            {
                configManager.save();

                websocketManager.broadcastState();

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
        });

    /*
     * POST /api/restart
     */

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

            delay(1000);

            ESP.restart();
        });

    /*
     * GET /api/wifi/scan
     */

    server.on(
        "/api/wifi/scan",
        HTTP_GET,
        AsyncWebServerRequest* request
        {
            JsonDocument doc;

            JsonArray networks =
                doc.to<JsonArray>();

            int count =
                WiFi.scanNetworks();

            for(int i = 0;
                i < count;
                i++)
            {
                JsonObject net =
                    networks.add<JsonObject>();

                net["ssid"] =
                    WiFi.SSID(i);

                net["rssi"] =
                    WiFi.RSSI(i);

                net["channel"] =
                    WiFi.channel(i);

                net["encryption"] =
                    WiFi.encryptionType(i);
            }

            String json;

            serializeJson(
                doc,
                json
            );

            request->send(
                200,
                "application/json",
                wifiManager.scanNetworksJson()
            );
        });
}