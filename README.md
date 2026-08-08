# Baseball-Display

Projekt-Übersicht
-----------------
Dies ist ein ESP32-basiertes Scoreboard-Projekt für Baseball. Die Anzeige verwendet adressierbare RGB-LEDs (z. B. WS2812 / Adafruit NeoPixel), welche zu 7-Segment-Ziffern gruppiert werden, um Ziffern, Count-Indikatoren und Team-Innings darzustellen.

Funktionen
---------
- Anzeige von AtBat (Batter), Balls, Strikes, Outs
- Anzeige von Innings (je Team 10 Innings, 2 Ziffern pro Inning)
- Anzeige von Hits und Errors je Team
- Web UI: Vorschau (index.html), Steuerung (control.html), Konfiguration (config.html)
- Echtzeit-Kommunikation per WebSocket (JSON) zwischen Browser und ESP32

Benötigte Hardware
------------------
- ESP32-Entwicklungsboard (z. B. DevKit)
- 3 × NeoPixel / WS2812b Streifen (oder Ketten):
  - Top-Strip (für AtBat, Count, Uhr): 49 LEDs (TOP_STRIP_LEDS)
  - Team A Strip: 182 LEDs (TEAM_A_LEDS)
  - Team B Strip: 182 LEDs (TEAM_B_LEDS)
- BH1750 Helligkeitssensor (I2C)
- Optional: RTC (I2C)
- 5 V Netzteil mit ausreichender Stromstärke (siehe unten)
- Pegelwandler 3.3V → 5V (z. B. 74HCT125) empfohlen
- Großer Elektrolyt-Kondensator (z. B. 1000 µF) nahe dem ersten NeoPixel
- Serienwiderstand 300–500 Ω in der Datenleitung (vorerst optional, empfohlen)

GPIO / Pins
-----------
(Stand: src/Config.h)
- TOP_STRIP_PIN: GPIO 5
- TEAM_A_STRIP_PIN: GPIO 18
- TEAM_B_STRIP_PIN: GPIO 19
- I2C: SDA = GPIO 21, SCL = GPIO 22

LED-Anzahlen (ebenfalls in src/Config.h)
- TOP_STRIP_LEDS = 49
- TEAM_A_LEDS = 182
- TEAM_B_LEDS = 182

Verdrahtung (Kurz)
------------------
1. Stromversorgung:
   - 5 V Netzteil → 5 V+ der NeoPixel-Streifen
   - Gemeinsame Masse: 5 V GND ↔ ESP32 GND
   - 1000 µF Kondensator zwischen 5 V und GND in der Nähe der Streifen
2. Datenleitungen:
   - Top strip DIN → GPIO5 (TOP_STRIP_PIN) über Pegelwandler (falls verwendet)
   - Team A DIN → GPIO18
   - Team B DIN → GPIO19
   - Empfohlen: 300–500 Ω Serienwiderstand in der Datenleitung nahe dem ersten Pixel
3. Sensoren (I2C):
   - BH1750 SDA → GPIO21, SCL → GPIO22
   - RTC (falls vorhanden) ebenfalls an SDA/SCL

Strombedarf & Netzteil-Auswahl
------------------------------
- NeoPixel können bis zu ca. 60 mA pro Pixel bei voller Helligkeit (weiß) ziehen.
- Gesamt-LEDs: 49 + 182 + 182 = 413
- Max. theoretischer Strom: 413 × 0.06 A ≈ 24.8 A

Empfehlung:
- Netzteil: 5 V, mindestens 30 A (bei voller Helligkeit)
- In der Praxis kann die Helligkeit (DEFAULT_BRIGHTNESS in Config.h) reduziert werden, wodurch der Strombedarf stark sinkt.
- Bei langen Streifen Power Injection (Mehrere Einspeisepunkte) verwenden.

LED-Mapping (aktuell in lib/SevenSegmentLED/SevenSegmentLED.cpp)
----------------------------------------------------------------
Das Projekt verwendet software-mapping, um jeweils 7 NeoPixel als ein 7-Segment-Digit zu betreiben. Die konkrete Implementierung in `lib/SevenSegmentLED/SevenSegmentLED.cpp` definiert die Anordnung und Belegung wie folgt:

- Konstanten:
  - DIGIT_SEGMENTS = 7
  - LEDS_PER_SEGMENT = 1 (also 1 LED pro Segment)
  - LEDS_PER_DIGIT = DIGIT_SEGMENTS * LEDS_PER_SEGMENT = 7

- Segment-Indizes (Index → Segment):
  - 1 → Segment A (oben)
  - 2 → Segment B (oben rechts)
  - 3 → Segment C (unten rechts)
  - 4 → Segment D (unten)
  - 5 → Segment E (unten links)
  - 0 → Segment F (oben links)
  - 6 → Segment G (Mitte)

- Digit-Mapping (`digitMap` Array):
  - Das Array `digitMap[10][7]` legt fest, welche Segmente für jede Ziffer 0-9 aktiviert werden. Beispiel: `{1,1,1,1,1,1,0}` für 0 bedeutet, Segmente A..F an, G aus — also normale "0"-Darstellung.

Pixel-Indexierung einer Ziffer
- Für eine Ziffer mit `startPixel = S` werden die Segment-LEDs in aufsteigender Reihenfolge belegt:
  - Segment A → Pixel S + 1
  - Segment B → Pixel S + 2
  - Segment C → Pixel S + 3
  - Segment D → Pixel S + 4
  - Segment E → Pixel S + 5
  - Segment F → Pixel S + 0
  - Segment G → Pixel S + 6

Beispiel: `drawDigit(startPixel=7, digit=5, color)` setzt Pixel 7..13 entsprechend den Segmenten für die Ziffer "5".

Konkrete Startpixel-Verwendung im Code
- Top-Strip (`topDisplay` in ScoreboardDisplay):
  - AtBat (2 Ziffern): `startPixel = 0` (zwei Ziffern belegen Pixel 0..13)
  - Ball-Indikatoren: `startPixel = 28` (3 LEDs für Balls)
  - Strike-Indikatoren: `startPixel = 32` (2 LEDs)
  - Outs-Indikatoren: `startPixel = 35` (2 LEDs)
  - Uhrzeit (4 Ziffern): `startPixel = 40` (Pixel 40..67 für 4 Ziffern)

- Team A / Team B Strips (`displayA` / `displayB`):
  - Innings: `displayA.drawNumber(i*14, 2, value, color)` → für Inning i (0..9) wird `startPixel = i * 14` verwendet. Jeder Inning-Block belegt somit 14 Pixel (2 Ziffern × 7 LEDs).
  - Beispiel: Inning 1 (i=0) → Pixel 0..13, Inning 2 (i=1) → Pixel 14..27, usw.

Hinweis zur physikalischen Montage
- Da die Segment-LEDs in einer Software-Logik gruppiert sind, ist es sinnvoll, die Pixel physisch so zu verlegen, dass die Reihenfolge der Pixel einem Ziffernblock entspricht (z. B. 7 LEDs pro Ziffer, in A..G Reihenfolge). Falls die physikalische Anordnung abweicht, kann das Mapping in `SevenSegmentLED.cpp` angepasst werden (LEDS_PER_SEGMENT oder die setSegment-Logik erweitern).

WebSocket / JSON API
--------------------
Die Web-Clients (z. B. `data/control.html`) senden JSON-Nachrichten an den WebSocket-Endpunkt (`/ws`). Der ESP32 verarbeitet diese in `src/WebSocketManager.cpp`.

Unterstützte Felder (Beispiele):
- `atBat` (int)
- `balls` (int)
- `strikes` (int)
- `outs` (int)
- `hitsA`, `hitsB` (int)
- `errorsA`, `errorsB` (int)
- `inningsA` (Array[int] mit 10 Werten), `inningsB` (Array[int])
- `cmd` (String), z. B. "resetCount" oder "clearGame"

Beispiele:
- `{ "atBat": 23 }`
- `{ "balls": 2, "strikes": 1, "outs": 1 }`
- `{ "inningsA": [2,0,1,3,0,0,0,0,0,0], "hitsA": 12, "errorsA": 1 }`

Relevante Quellcode-Dateien
---------------------------
- Web UI: `data/index.html`, `data/control.html`, `data/config.html`
- Client-Logic: `data/app.js`, `data/config.js`
- Display-Logic: `lib/SevenSegmentLED/*`, `lib/ScoreboardDisplay/*`
- WebSocket-Handling: `src/WebSocketManager.cpp`
- Konfiguration: `src/Config.h`
- Hauptprogramm: `src/main.cpp`

Build / Flash
-------------
- Öffne das Projekt in der Arduino IDE oder PlatformIO.
- Wähle das passende ESP32-Board und den COM/Port.
- Kompiliere und flashe das Projekt auf das ESP32.

Hinweis / Sicherheit
--------------------
- Gemeinsame Masse zwischen ESP32 und NeoPixel-Stromquelle ist zwingend.
- Pegelwandler empfohlen, um Datenzuverlässigkeit zu erhöhen.
- Verwende angemessene Absicherung (Sicherung/Polyfuse) und passende Kabelquerschnitte.

Fehlerbehebung
---------------
- Lichter flackern / falsche Farben: Ursache oft Masse-Problem, fehlender Pegelwandler oder zu schwaches Netzteil.
- Netzteil schaltet ab: Netzteil zu schwach oder Vollweiß-Last; reduzierte Helligkeit einstellen.
- WebSocket-Verbindung bricht ab: check WLAN / Serial-Logs des ESP32.

Haftungsausschluss
------------------
Hobbyprojekt. Vor Einsatz in sicherheitskritischen Umgebungen alle Vorschriften (Brandschutz, elektrische Sicherheit) beachten. Keine Haftung für Schäden durch unsachgemäße Verdrahtung oder ungeeignete Stromversorgung.
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