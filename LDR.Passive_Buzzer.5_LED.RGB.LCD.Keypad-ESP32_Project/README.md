# LDR + Passive Buzzer + 5 LEDs + RGB + LCD + Keypad — ESP32

Compact ESP32 sketch combining: keypad access control, real-time clock (NTP),
an LDR light reader, a simple Dino game, multiple single LEDs, an RGB LED,
an I2C LCD, and a passive buzzer.

## Features
- Keypad-protected menu (default password: `A707`)
- Menu options:
  - `A` — Show time (uses WiFi + NTP)
  - `B` — Dino game (score stored in EEPROM)
  - `C` — Live LDR reading
  - `D` — Toggle 5 single LEDs and RGB channels
  - `#` and `*` — navigation/logout controls
- Automatic LCD backlight control via LDR reading

## Required hardware
- ESP32 development board
- 16x2 I2C LCD (address 0x27)
- 4x4 keypad
- LDR (photoresistor) with pull-down/voltage divider
- Passive buzzer
- 5 single (indicator) LEDs + resistors (220 Ω)
- Common RGB LED (3 resistors, 220 Ω each)
- Jumper wires, breadboard

## Pin wiring
The wiring below matches the sketch:

- LCD (I2C): VCC → 5V, GND → GND, SDA → 21, SCL → 22
- Keypad rows → 13, 12, 14, 27
- Keypad cols → 26, 25, 33, 32
- Single LEDs (through 220Ω): pins 5, 16, 17, 18, 19 (one LED per pin)
- RGB LED (through 220Ω): R → 23, G → 15, B → 2
- LDR analog → 34 (use voltage divider so analog values are stable)
- Passive buzzer + → 4

See the top comment block in `LDR.Passive_Buzzer.5_LED.RGB.LCD.Keypad-ESP32_Project.ino` for a diagram.

## Libraries / Software
Install these libraries (use Arduino Library Manager or include them via the IDE):

- `Keypad` (by Mark Stanley, Alexander Brevig)
- `LiquidCrystal_I2C` (common I2C LCD library; choose a compatible fork for 0x27)
- ESP32 core (provides `WiFi.h`, `time.h`, `EEPROM.h`)

Notes:
- The sketch uses `WiFi` and `configTime()` to fetch NTP time (IST offset +19800s).
- EEPROM is used to store the high score; EEPROM size is configured in the sketch.

## Configuration
1. Open `LDR.Passive_Buzzer.5_LED.RGB.LCD.Keypad-ESP32_Project.ino` in the Arduino IDE.
2. Edit the WiFi credentials near the top:

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* wifiPass = "WIFI_PASSWORD";
```

3. (Optional) Change the default password variable `password = "A707"`.

## Build & Upload
1. Install the ESP32 board support in Arduino IDE (Boards Manager → esp32).
2. Select the correct ESP32 board and COM port.
3. Install the required libraries via Library Manager.
4. Upload the sketch.

## Usage
- On power-up the LCD prompts for the password.
- Enter the password followed by `#` to unlock.
- Use the keypad keys described in the Features section to navigate.

## Troubleshooting
- If the LCD stays blank: check I2C address (0x27) and wiring (SDA/SCL).
- If WiFi/NTP time doesn't appear: confirm correct credentials and that board has WiFi connectivity.
- If LDR readings look incorrect: adjust your voltage divider and analog reference.

## License
This repository is ready for your preferred license. If unsure, consider the MIT License.

## Acknowledgements
This project combines common Arduino/ESP32 libraries and example patterns for hobby projects.

---
Created from `LDR.Passive_Buzzer.5_LED.RGB.LCD.Keypad-ESP32_Project.ino`.
