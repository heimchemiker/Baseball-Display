Dies soll ein Baseball Scoreboard werden, welches für die Darfstellung 7-segmentanzeigen nutzt, welche jedoch durch WS2812/SK812 LED beleuchtet werden. Hierzu entspricht das linke obere Segment LED1, das obere Segment LED2, das rechte obere Segment LED3, das mittlere Segment LED4, das rechte untere Segment LED5, das untere Segment LED6, und das rechte untere Segment LED7.
Für eine einfachere Verkabelung wird das dreizeilige Display durch 3 LED-Streifen beleuchtet.

#Unterstützte JSON-Updates
Balls setzen:
JSON
{
    "balls":23
}

Strikes setzen:
JSON
{
    "strikes":13
}

Outs setzen:
JSON
{
    "outs":23
}

Batter setzen:
JSON
{
    "atBat":233
}

Komplettes Team A aktualisieren:
JSON
{
    "inningsA":[1,0,2,0,3,0,0,1,0,0],
    "hitsA":12,
    "errorsA":17
}

Ein Inning ändern:
JSON
{
    "team":"A",
    "inning":3,
    "value":55
}

Hit erhöhen:
JSON
{
    "team":"A",
    "stat":"hits",
    "delta":15
}

Error verringern:
JSON
{
    "team":"B",
    "stat":"errors",
    "delta":-15
}