# Baseball-Display

## Projekt-Übersicht
Dies ist ein ESP32-basiertes Scoreboard-Projekt für Baseball. Die Anzeige verwendet adressierbare RGB-LEDs (z. B. WS2812 / Adafruit NeoPixel), welche zu 7-Segment-Ziffern gruppiert werden, um Ziffern, Count-Indikatoren und Team-Innings darzustellen.

### Funktionen
- Anzeige von AtBat (Batter), Balls, Strikes, Outs
- Anzeige von Innings (je Team 10 Innings, 2 Ziffern pro Inning)
- Anzeige von Hits und Errors je Team
- Web UI: Vorschau (index.html), Steuerung (control.html), Konfiguration (config.html)
- Echtzeit-Kommunikation per WebSocket (JSON) zwischen Browser und ESP32

## Benötigte Hardware
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

## GPIO / Pins (Konfigurationsdatei)
(Stand: `src/Config.h`)
- `TOP_STRIP_PIN` = GPIO 5
- `TEAM_A_STRIP_PIN` = GPIO 18
- `TEAM_B_STRIP_PIN` = GPIO 19
- I2C: `SDA` = GPIO 21, `SCL` = GPIO 22

LED-Anzahlen (ebenfalls in `src/Config.h`)
- `TOP_STRIP_LEDS` = 49
- `TEAM_A_LEDS` = 182
- `TEAM_B_LEDS` = 182

## Verdrahtung (Kurzfassung)
1. **Stromversorgung**
   - 5 V Netzteil → 5 V+ der NeoPixel-Streifen
   - Gemeinsame Masse: 5 V GND ↔ ESP32 GND
   - 1000 µF Kondensator zwischen 5 V und GND in der Nähe der Streifen
2. **Datenleitungen**
   - Top strip DIN → GPIO5 (TOP_STRIP_PIN) über Pegelwandler (falls verwendet)
   - Team A DIN → GPIO18
   - Team B DIN → GPIO19
   - Empfohlen: 300–500 Ω Serienwiderstand in der Datenleitung nahe dem ersten Pixel
3. **Sensoren (I2C)**
   - BH1750 SDA → GPIO21, SCL → GPIO22
   - RTC (falls vorhanden) ebenfalls an SDA/SCL

## Strombedarf & Netzteil-Auswahl
- NeoPixel können bis zu ca. 60 mA pro Pixel bei voller Helligkeit (weiß) ziehen.
- Gesamt-LEDs: 49 + 182 + 182 = 413
- Max. theoretischer Strom: 413 × 0.06 A ≈ 24.8 A

**Empfehlung:**
- Netzteil: 5 V, mindestens 30 A (bei voller Helligkeit)
- In der Praxis senkt reduzierte Helligkeit (DEFAULT_BRIGHTNESS in `Config.h`) den Strombedarf stark.
- Bei langen Streifen Power Injection (Mehrere Einspeisepunkte) verwenden.

## 7-Segment LED Mapping
Die 7-Segment-Darstellung wird softwareseitig auf den NeoPixeln abgebildet (`lib/SevenSegmentLED/SevenSegmentLED.cpp`).

- Jeder Segmentziffer verwendet 7 LEDs (`LEDS_PER_DIGIT = 7`), 1 LED pro Segment (`LEDS_PER_SEGMENT = 1`).
- Segment-Indizes 0..6 entsprechen A..G:
  - 0 = A (oben)
  - 1 = B (oben rechts)
  - 2 = C (unten rechts)
  - 3 = D (unten)
  - 4 = E (unten links)
  - 5 = F (oben links)
  - 6 = G (Mitte)

**Ziffernbelegung:** (digitMap in `SevenSegmentLED.cpp` entspricht Standard)
- 0 → A,B,C,D,E,F = AN, G = AUS
- 1 → B,C = AN
- 2 → A,B,D,E,G = AN
- 3 → A,B,C,D,G = AN
- 4 → B,C,F,G = AN
- 5 → A,C,D,F,G = AN
- 6 → A,C,D,E,F,G = AN
- 7 → A,B,C = AN
- 8 → alle Segmente AN
- 9 → A,B,C,D,F,G = AN

**Pixel-Indexierung einer Ziffer:**
- Für startPixel = S gilt Segment A → Pixel S+0, B → S+1, ..., G → S+6

**Konkrete Startpixel im aktuellen Code:**
- Top-Strip:
  - AtBat (2 Ziffern): `startPixel = 0`
  - Ball-Indikator (3 LEDs): `startPixel = 28`
  - Strike-Indikator (2 LEDs): `startPixel = 32`
  - Outs-Indikator (2 LEDs): `startPixel = 35`
  - Uhrzeit (4 Ziffern): `startPixel = 40`
- Team A / Team B Strips:
  - Innings: für Inning i (0..9) wird `startPixel = i * 14` (14 Pixel pro Inning = 2 Ziffern × 7 LEDs)

## WebSocket / JSON API
Die Web-Clients (z. B. `data/control.html`) senden JSON-Nachrichten an den WebSocket-Endpunkt (`/ws`). Der ESP32 verarbeitet diese in `src/WebSocketManager.cpp`.

**Unterstützte Felder (Beispiele):**
- `atBat` (int)
- `balls` (int)
- `strikes` (int)
- `outs` (int)
- `hitsA`, `hitsB` (int)
- `errorsA`, `errorsB` (int)
- `inningsA` (Array[int] mit 10 Werten), `inningsB` (Array[int])
- `cmd` (String), z. B. "resetCount" oder "clearGame"

**Beispiele:**
- `{ "atBat": 23 }`
- `{ "balls": 2, "strikes": 1, "outs": 1 }`
- `{ "inningsA": [2,0,1,3,0,0,0,0,0,0], "hitsA": 12, "errorsA": 1 }`
- `{ "cmd": "resetCount" }`

## Relevante Projektdateien
- Web UI: `data/index.html`, `data/control.html`, `data/config.html`
- Client-Logic: `data/app.js`, `data/config.js`
- Display-Logic: `lib/SevenSegmentLED/*`, `lib/ScoreboardDisplay/*`
- WebSocket-Handling: `src/WebSocketManager.cpp`
- Konfiguration: `src/Config.h`
- Hauptprogramm: `src/main.cpp`

## Build / Flash
- Öffne das Projekt in der Arduino IDE oder PlatformIO.
- Wähle das passende ESP32-Board und den COM/Port.
- Kompiliere und flashe das Projekt auf das ESP32.

## Sicherheit / Hinweise
- Gemeinsame Masse zwischen ESP32 und NeoPixel-Stromquelle ist zwingend.
- Pegelwandler empfohlen, um Datenzuverlässigkeit zu erhöhen.
- Verwende angemessene Absicherung (Sicherung/Polyfuse) und passende Kabelquerschnitte.

## Fehlerbehebung
- Flackern / falsche Farben → prüfe GND-Verbindung, Pegel und Netzteil.
- Netzteil schaltet ab → Netzteil zu schwach oder Vollweiß-Last; reduzierte Helligkeit einstellen.
- WebSocket-Verbindung bricht ab → check WLAN / Serial-Logs des ESP32.

## Haftungsausschluss
Hobbyprojekt. Vor Einsatz in sicherheitskritischen Umgebungen alle Vorschriften (Brandschutz, elektrische Sicherheit) beachten. Keine Haftung für Schäden durch unsachgemäße Verdrahtung oder ungeeignete Stromversorgung.
