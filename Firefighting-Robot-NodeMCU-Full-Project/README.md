# Firefighting Robot using NodeMCU ESP8266

Starter project package containing Arduino source, wiring notes, component list, and wiring reference images.

## Hardware architecture used by this version

- NodeMCU ESP8266
- L298N motor driver and four DC motors (two motors per side)
- Three digital flame sensor modules
- MCP23017 I2C GPIO expander (reads the three flame sensors)
- PCA9685 I2C PWM driver (controls the nozzle servo)
- Servo motor
- 12 V DC pump switched through a 3.3 V-compatible logic-level MOSFET driver

**This version requires both MCP23017 and PCA9685 modules.** If you do not own them, do not wire or upload this version as-is; ask for a pin map using only your existing modules first.

## Files

- `firefighting_robot.ino` — Arduino sketch
- `WIRING.md` — pin-to-pin wiring and safety notes
- `COMPONENTS.md` — parts list
- `images/system_block_diagram.png` — overall system diagram
- `images/pin_connection_diagram.png` — connection reference
- `.gitignore` — files to exclude from Git
- `LICENSE` — MIT license for this starter code

## Arduino IDE setup

1. Install the ESP8266 board package.
2. Select **NodeMCU 1.0 (ESP-12E Module)**.
3. Install **Adafruit MCP23017 Arduino Library** and **Adafruit PWM Servo Driver Library** from Library Manager.
4. Open `firefighting_robot.ino`.
5. Verify the module addresses, actual board pin labels, power ratings, and sensor logic.
6. Test with wheels lifted and pump power disconnected first.

## Important

This is a student prototype, not certified fire-protection equipment. Never use it to approach or fight a real fire. Do not connect the pump or motors to NodeMCU GPIO pins. Verify the MOSFET module's input polarity and current rating. Use a separate regulated 5 V servo supply and a suitable 12 V pump supply. All non-isolated circuit grounds must be common.
