# Wiring guide — verify module labels before connecting

This wiring version assumes **both** an MCP23017 and a PCA9685 are installed. If you do not have these boards, stop and request a design for your actual parts; the sketch will not run without them.

## A. NodeMCU to L298N

| NodeMCU | L298N |
|---|---|
| D5 / GPIO14 | IN1 |
| D6 / GPIO12 | IN2 |
| D7 / GPIO13 | IN3 |
| D0 / GPIO16 | IN4 |
| GND | GND |

- Install ENA/ENB jumpers for fixed full speed, if your board supports this.
- OUT1/OUT2: left motor pair; OUT3/OUT4: right motor pair.
- Battery positive to L298N motor supply input; battery negative to GND.
- Verify combined motor stall current. L298N may not safely drive four motors in parallel.

## B. NodeMCU I2C bus

| NodeMCU | MCP23017 | PCA9685 |
|---|---|---|
| D2 / GPIO4 (SDA) | SDA | SDA |
| D1 / GPIO5 (SCL) | SCL | SCL |
| 3V3 | VDD | VCC (logic) |
| GND | VSS/GND | GND |

- MCP23017 address must be 0x20 for this sketch (A0/A1/A2 address pins low).
- PCA9685 address must be 0x40 (address pins default).
- Ensure I2C pull-ups go to 3.3 V, not 5 V. Avoid stacking multiple pull-ups if boards already include them.

## C. Flame sensors to MCP23017

| Sensor | DO | VCC | GND |
|---|---|---|---|
| Left | GPA0 | 3.3 V only if module supports it | Common GND |
| Centre | GPA1 | 3.3 V only if module supports it | Common GND |
| Right | GPA2 | 3.3 V only if module supports it | Common GND |

The sketch assumes active-LOW fire detection. Confirm with Serial Monitor or a meter. Never allow a 5 V DO signal onto the 3.3 V MCP23017/ESP8266 logic bus; use level shifting if needed.

## D. Servo via PCA9685

- Servo signal: PCA9685 channel 0 signal pin.
- Servo red: external regulated 5 V servo supply.
- Servo brown/black: servo supply GND.
- Servo supply GND must be common with NodeMCU and PCA9685 GND.
- PCA9685 V+ is servo power, not logic VCC. Size the 5 V supply for servo stall current.

## E. Pump via MOSFET module

- NodeMCU D8 / GPIO15 to MOSFET module signal input.
- NodeMCU GND to MOSFET module logic GND.
- Pump positive to +12 V.
- Pump negative to the MOSFET switched output (often labelled OUT- or LOAD-).
- MOSFET power terminals to battery as specified by the module manufacturer.
- Use a 3.3 V logic-compatible, active-HIGH module rated for the pump's startup/stall current.
- D8/GPIO15 must remain LOW during boot. Do not use a module that pulls its input HIGH or otherwise violates this.
- Add a flyback diode across the pump if the module does not provide suitable inductive-load suppression.

## F. Power and safety

- Never power motors or pump from NodeMCU 3V3.
- Use the 12 V battery only for loads rated for 12 V; use a regulated 5 V supply for servo and board power as required.
- All non-isolated grounds must be common.
- Add suitable fuse protection close to the battery.
- Test with pump disconnected and wheels lifted. Then test the pump switching with the wheels still lifted.
- Do not test near flammable materials or use this hobby robot to fight a real fire.

## Boot pin warning

D8 is GPIO15 and must be LOW during reset. The pump output is configured active-HIGH so LOW means OFF, but the external MOSFET module must not pull this pin HIGH during boot. D0/GPIO16 has limitations compared with ordinary ESP8266 GPIOs; it is used here only as a motor direction output.
