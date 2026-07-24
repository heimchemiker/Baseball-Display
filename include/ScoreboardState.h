#pragma once

struct ScoreboardState
{
    uint8_t batter;

    uint8_t balls;
    uint8_t strikes;
    uint8_t outs;

    uint8_t inningsA[10];
    uint8_t inningsB[10];

    uint8_t hitsA;
    uint8_t hitsB;

    uint8_t errorsA;
    uint8_t errorsB;

    uint8_t runsA;
    uint8_t runsB;
};