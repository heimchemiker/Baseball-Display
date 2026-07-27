#include "StateSerializer.h"
#include <ArduinoJson.h>
#include "ScoreboardDisplay.h"

extern ScoreboardDisplay scoreboard;

String getStateJson()
{
    JsonDocument doc;

    auto& state =
        scoreboard.state();

    doc["atBat"]   = state.batter;
    doc["balls"]   = state.balls;
    doc["strikes"] = state.strikes;
    doc["outs"]    = state.outs;

    JsonArray inningsA =
        doc["inningsA"].to<JsonArray>();

    JsonArray inningsB =
        doc["inningsB"].to<JsonArray>();

    for(int i = 0; i < 10; i++)
    {
        inningsA.add(
            state.inningsA[i]
        );

        inningsB.add(
            state.inningsB[i]
        );
    }

    doc["hitsA"] = state.hitsA;
    doc["hitsB"] = state.hitsB;

    doc["errorsA"] = state.errorsA;
    doc["errorsB"] = state.errorsB;

    doc["runsA"] = state.runsA;
    doc["runsB"] = state.runsB;

    String json;

    serializeJson(
        doc,
        json
    );

    return json;
}