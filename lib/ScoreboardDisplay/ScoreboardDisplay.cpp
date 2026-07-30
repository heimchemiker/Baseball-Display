#include "ScoreboardDisplay.h"

//
// Oberer Streifen
//

#define BATTER_START       0

#define BALLS_START       14
#define STRIKES_START     17
#define OUTS_START        19

#define CLOCK_START       21

//
// Team-Streifen
//

#define INNING1_START       0
#define INNING2_START      14
#define INNING3_START      28
#define INNING4_START      42
#define INNING5_START      56
#define INNING6_START      70
#define INNING7_START      84
#define INNING8_START      98
#define INNING9_START     112
#define INNING10_START    126

#define RUNS_START        140
#define HITS_START        154
#define ERRORS_START      168

static const uint16_t inningMap[10] =
{
    INNING1_START,
    INNING2_START,
    INNING3_START,
    INNING4_START,
    INNING5_START,
    INNING6_START,
    INNING7_START,
    INNING8_START,
    INNING9_START,
    INNING10_START
};

ScoreboardDisplay::ScoreboardDisplay(
    Adafruit_NeoPixel* topStrip,
    Adafruit_NeoPixel* teamAStrip,
    Adafruit_NeoPixel* teamBStrip)
:
top(topStrip),
teamA(teamAStrip),
teamB(teamBStrip),
topDisplay(topStrip),
displayA(teamAStrip),
displayB(teamBStrip)
{
}

void ScoreboardDisplay::begin()
{
    top->begin();
    teamA->begin();
    teamB->begin();

    top->clear();
    teamA->clear();
    teamB->clear();

    top->show();
    teamA->show();
    teamB->show();

    colorTeamA = top->Color(255,0,0);
    colorTeamB = top->Color(0,255,0);

    colorClock = top->Color(255,255,255);

    colorBalls   = top->Color(0,255,0);
    colorStrikes = top->Color(255,255,0);
    colorOuts    = top->Color(255,0,0);

    colorHighlight =
        top->Color(
            255,
            255,
            255
        );
}

ScoreboardState&
ScoreboardDisplay::state()
{
    return scoreboardState;
}

void ScoreboardDisplay::setClock(
    uint8_t hour,
    uint8_t minute)
{
    currentHour   = hour;
    currentMinute = minute;
}

uint8_t
ScoreboardDisplay::getHour() const
{
    return currentHour;
}

uint8_t
ScoreboardDisplay::getMinute() const
{
    return currentMinute;
}

void ScoreboardDisplay::setCurrentInning(
    uint8_t inning)
{
    if(inning < 1)
    {
        inning = 1;
    }

    if(inning > 10)
    {
        inning = 10;
    }

    currentInning = inning;
}

uint8_t
ScoreboardDisplay::getCurrentInning() const
{
    return currentInning;
}

void ScoreboardDisplay::setColorTeamA(
    uint32_t color)
{
    colorTeamA = color;
}

void ScoreboardDisplay::setColorTeamB(
    uint32_t color)
{
    colorTeamB = color;
}

void ScoreboardDisplay::setColorClock(
    uint32_t color)
{
    colorClock = color;
}

void ScoreboardDisplay::setColorIndicator(
    uint32_t balls,
    uint32_t strikes,
    uint32_t outs)
{
    colorBalls   = balls;
    colorStrikes = strikes;
    colorOuts    = outs;
}

void ScoreboardDisplay::setHighlightColor(
    uint32_t color)
{
    colorHighlight = color;
}

void ScoreboardDisplay::calculateTotals()
{
    scoreboardState.runsA = 0;
    scoreboardState.runsB = 0;

    for(uint8_t i = 0; i < 10; i++)
    {
        scoreboardState.runsA +=
            scoreboardState.inningsA[i];

        scoreboardState.runsB +=
            scoreboardState.inningsB[i];
    }
}

void ScoreboardDisplay::renderTop()
{
    topDisplay.drawNumber(
        BATTER_START,
        2,
        scoreboardState.batter,
        colorClock,
        true
    );

    topDisplay.drawIndicator(
        BALLS_START,
        scoreboardState.balls,
        3,
        colorBalls
    );

    topDisplay.drawIndicator(
        STRIKES_START,
        scoreboardState.strikes,
        2,
        colorStrikes
    );

    topDisplay.drawIndicator(
        OUTS_START,
        scoreboardState.outs,
        2,
        colorOuts
    );

    topDisplay.drawTime(
        CLOCK_START,
        currentHour,
        currentMinute,
        colorClock
    );
}

void ScoreboardDisplay::renderTeamA()
{
    for(uint8_t inning = 0;
        inning < 10;
        inning++)
    {
        uint32_t color =
            (inning + 1 ==
             currentInning)
                ? colorHighlight
                : colorTeamA;

        displayA.drawNumber(
            inningMap[inning],
            2,
            scoreboardState
                .inningsA[inning],
            color
        );
    }

    displayA.drawNumber(
        RUNS_START,
        2,
        scoreboardState.runsA,
        colorTeamA
    );

    displayA.drawNumber(
        HITS_START,
        2,
        scoreboardState.hitsA,
        colorTeamA
    );

    displayA.drawNumber(
        ERRORS_START,
        2,
        scoreboardState.errorsA,
        colorTeamA
    );
}

void ScoreboardDisplay::renderTeamB()
{
    for(uint8_t inning = 0;
        inning < 10;
        inning++)
    {
        uint32_t color =
            (inning + 1 ==
             currentInning)
                ? colorHighlight
                : colorTeamB;

        displayB.drawNumber(
            inningMap[inning],
            2,
            scoreboardState
                .inningsB[inning],
            color
        );
    }

    displayB.drawNumber(
        RUNS_START,
        2,
        scoreboardState.runsB,
        colorTeamB
    );

    displayB.drawNumber(
        HITS_START,
        2,
        scoreboardState.hitsB,
        colorTeamB
    );

    displayB.drawNumber(
        ERRORS_START,
        2,
        scoreboardState.errorsB,
        colorTeamB
    );
}

void ScoreboardDisplay::render()
{
    calculateTotals();

    top->clear();
    teamA->clear();
    teamB->clear();

    renderTop();

    renderTeamA();

    renderTeamB();

    top->show();
    teamA->show();
    teamB->show();
}