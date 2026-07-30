#include <Arduino.h>

#include <ESPAsyncWebServer.h>

#include "WebSocketManager.h"
#include "ScoreboardDisplay.h"
#include "RestApi.h"
#include "StateSerializer.h"
#include "MqttManager.h"
#include "BrightnessManager.h"

MqttManager mqttManager;


AsyncWebServer server(80);

WebSocketManager websocketManager;

RestApi restApi;

ScoreboardDisplay scoreboard(
    &topStrip,
    &teamAStrip,
    &teamBStrip
);

void setup()
{
    Serial.begin(115200);
    scoreboard.begin();
    websocketManager.begin(server);
    server.begin();
    mqttManager.begin();    
}

void loop()
{
    mqttManager.loop();
}