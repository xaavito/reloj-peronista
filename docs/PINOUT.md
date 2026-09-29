# Pinout — Reloj Peronista (authoritative)

This is the real, current wiring, extracted from `src/main.cpp` + `platformio.ini`
(the source of truth). It supersedes `wiring.md` / `wiring2.md`, which are stale
(they show a single "optional" button on GPIO 15, which is actually the TFT CS).

Board: **ESP32 DevKit V1** — `esp32doit-devkit-v1`.

## Pin map

| Component | Signal | ESP32 pin | Notes |
|---|---|---|---|
| TFT ILI9341 240x320 | MISO | GPIO 12 | HSPI bus (`USE_HSPI_PORT=1`) |
| | MOSI | GPIO 13 | |
| | SCLK | GPIO 14 | |
| | CS   | GPIO 15 | |
| | DC   | GPIO 2  | |
| | RST  | — | not wired, software reset (`TFT_RST=-1`) |
| | VCC/GND | 3.3V / GND | |
| AHT10 (temp/humidity) | SDA | GPIO 21 | I2C default bus, shared |
| | SCL | GPIO 22 | |
| BMP180 (pressure) | SDA | GPIO 21 | same I2C bus as AHT10 |
| | SCL | GPIO 22 | |
| Buzzer (active 5V) | signal | GPIO 25 | driven with `tone()`/`noTone()` |
| Button 1 - mode | signal | GPIO 27 | `INPUT_PULLUP` (other leg to GND) |
| Button 2 - alarm | signal | GPIO 4 | `INPUT_PULLUP` (other leg to GND) |

Both sensors sit on the default I2C bus (`aht.begin()` / `bmp.begin()` with no
custom pins), so they share SDA=21 / SCL=22. Both buttons use the internal
pull-up, so the free leg goes to GND - no external resistor needed.

## Button behavior (from `main.cpp`)

- **GPIO 27 (mode):** short press cycles display mode / snooze; hold ~10 s resets WiFi.
- **GPIO 4 (alarm):** short press dismisses the alarm; long press (~2 s) enters alarm config.

## ASCII pinout

```
                          ESP32 DevKit V1
                       +-------------------+
   TFT MISO  -- GPIO12 |                   | GPIO21 -- SDA  (AHT10 + BMP180)
   TFT MOSI  -- GPIO13 |                   | GPIO22 -- SCL  (AHT10 + BMP180)
   TFT SCLK  -- GPIO14 |                   | GPIO25 -- Buzzer (+)
   TFT CS    -- GPIO15 |                   | GPIO27 -- Button 1 (mode) -> GND
   TFT DC    -- GPIO 2 |                   | GPIO 4 -- Button 2 (alarm) -> GND
   TFT RST   -- (n/c)  |                   |
                       |      [ USB ]      |  3.3V -- TFT VCC, AHT10, BMP180
                       +---------+---------+  GND  -- common ground (all)
                                 |
                              USB 5V
```

Power: everything on 3.3V except the active buzzer (5V). Common ground for all.
