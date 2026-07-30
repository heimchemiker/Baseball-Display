#include <Arduino.h>

#include <WiFi.h>
#include <Wire.h>
#include <LittleFS.h>

#include <Adafruit_NeoPixel.h>
#include <ESPAsyncWebServer.h>

#include "Config.h"

#include "ScoreboardDisplay.h"
#include "ConfigManager.h"
#include "WifiManager.h"
#include "TimeManager.h"
#include "BrightnessManager.h"
#include "MqttManager.h"
#include "RestApi.h"
#include "WebSocketManager.h"

//
// Webserver
//

AsyncWebServer server(80);

//
// LED Streifen
//

Adafruit_NeoPixel topStrip(
    TOP_STRIP_LEDS,
    TOP_STRIP_PIN,
    NEO_GRB + NEO_KHZ800
);

Adafruit_NeoPixel teamAStrip(
    TEAM_A_LEDS,
    TEAM_A_STRIP_PIN,
    NEO_GRB + NEO_KHZ800
);

Adafruit_NeoPixel teamBStrip(
    TEAM_B_LEDS,
    TEAM_B_STRIP_PIN,
    NEO_GRB + NEO_KHZ800
);

//
// Zentrale Instanzen
//

ConfigManager configManager;

WifiManager wifiManager;

ScoreboardDisplay scoreboard(
    &topStrip,
    &teamAStrip,
    &teamBStrip
);

BrightnessManager brightnessManager(
    &topStrip,
    &teamAStrip,
    &teamBStrip
);

TimeManager timeManager;

WebSocketManager websocketManager;

RestApi restApi;

MqttManager mqttManager;

//
// LittleFS Dateien bereitstellen
//

void configureFileServer()
{
    server.serveStatic(
        "/",
        LittleFS,
        "/"
    ).setDefaultFile(
        "index.html"
    );

    server.serveStatic(
        "/index.html",
        LittleFS,
        "/index.html"
    );

    server.serveStatic(
        "/control.html",
        LittleFS,
        "/control.html"
    );

    server.serveStatic(
        "/config.html",
        LittleFS,
        "/config.html"
    );

    server.serveStatic(
        "/app.js",
        LittleFS,
        "/app.js"
    );

    server.serveStatic(
        "/control.js",
        LittleFS,
        "/control.js"
    );

    server.serveStatic(
        "/config.js",
        LittleFS,
        "/config.js"
    );

    server.serveStatic(
        "/style.css",
        LittleFS,
        "/style.css"
    );
}

//
// Beispielwerte beim Erststart
//

void initializeScoreboard()
{
    auto& state =
        scoreboard.state();

    state.batter = 23;

    state.balls   = 2;
    state.strikes = 1;
    state.outs    = 1;

    state.inningsA[0] = 2;
    state.inningsA[1] = 0;
    state.inningsA[2] = 1;
    state.inningsA[3] = 3;
    state.inningsA[4] = 0;
    state.inningsA[5] = 2;
    state.inningsA[6] = 1;
    state.inningsA[7] = 0;
    state.inningsA[8] = 0;
    state.inningsA[9] = 0;

    state.inningsB[0] = 0;
    state.inningsB[1] = 1;
    state.inningsB[2] = 0;
    state.inningsB[3] = 0;
    state.inningsB[4] = 2;
    state.inningsB[5] = 0;
    state.inningsB[6] = 3;
    state.inningsB[7] = 0;
    state.inningsB[8] = 1;
    state.inningsB[9] = 0;

    state.hitsA   = 12;
    state.errorsA = 1;

    state.hitsB   = 11;
    state.errorsB = 2;

    scoreboard.setCurrentInning(4);

    scoreboard.render();
}

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("=====================================");
    Serial.println(" Baseball Scoreboard");
    Serial.println("=====================================");

    //
    // Konfiguration laden
    //

    if(!configManager.begin())
    {
        Serial.println(
            "ConfigManager Fehler"
        );
    }

    //
    // WLAN starten
    //

    wifiManager.begin();

    //
    // Display initialisieren
    //

    scoreboard.begin();

    //
    // Helligkeit
    //

    brightnessManager.begin();

    brightnessManager.setEnabled(
        configManager.config()
            .brightnessSensor
    );

    brightnessManager.setLuxRange(
        configManager.config().luxMin,
        configManager.config().luxMax
    );

    brightnessManager.setBrightnessRange(
        configManager.config().brightnessMin,
        configManager.config().brightnessMax
    );

    //
    // Uhr
    //

    timeManager.begin();

    //
    // Testdaten
    //

    initializeScoreboard();

    //
    // Webserver-Dateien
    //

    configureFileServer();

    //
    // REST API
    //

    restApi.begin(server);

    //
    // WebSocket
    //

    websocketManager.begin(server);

    //
    // MQTT
    //

    mqttManager.begin();

    //
    // Webserver starten
    //

    server.begin();

    Serial.println();
    Serial.println("System gestartet");
    Serial.println();

    Serial.print("IP: ");

    if(wifiManager.connected())
    {
        Serial.println(
            WiFi.localIP()
        );
    }
    else
    {
        Serial.println(
            WiFi.softAPIP()
        );
    }
}

void loop()
{
    //
    // WLAN überwachen
    //

    wifiManager.loop();

    //
    // MQTT Verbindungen
    //

    mqttManager.loop();

    //
    // Zeit aktualisieren
    //

    timeManager.update();

    //
    // Automatische Helligkeit
    //

    brightnessManager.update();

    //
    // Anzeige aktualisieren
    //

    scoreboard.render();

    delay(10);
}