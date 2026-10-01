# Universal connector labels and separate firmware profiles

The **physical PCB silkscreen must remain universal**: `A0`, `A1`, `A2`,
`A3`, `OUT1` … `OUT8`, never pond/plant/animal names. These are *example*
assignments for **separate** Aquaculture, Garden and Livestock firmware
builds, not simultaneous loads or validated sensor choices. All A inputs
are conditioned ADS1115 channels at 3V3; all outputs are low-side 12 V
DC switches, not AC contacts. Do not wire sensors or actuators until
interface voltage, power and failure states have been verified.

| Label | Aquaculture example | Garden example | Livestock example |
|---|---|---|---|
| A0 | pH amplifier | Soil moisture transmitter | Water level transmitter |
| A1 | Dissolved oxygen amplifier | Soil pH amplifier | Feed bin level transmitter |
| A2 | Turbidity transmitter | Light level transmitter | Air quality transmitter |
| A3 | Water level transmitter | Tank level transmitter | Spare conditioned analog sensor |
| OUT1 | Aerator DC coil | Irrigation valve DC coil | Ventilation DC coil |
| OUT2 | Pump DC coil | Water pump DC coil | Water pump DC coil |
| OUT3 | Circulation DC coil | Grow light external contactor DC coil | Feeder DC coil |
| OUT4 | Feeder DC coil | Nutrient valve DC coil | Heater external contactor DC coil |
| OUT5 | Valve DC coil | Fan DC coil | Lighting external contactor DC coil |
| OUT6 | Light external contactor DC coil | Spare DC load | Alarm DC coil |
| OUT7 | Spare DC load | Spare DC load | Spare DC load |
| OUT8 | Spare DC load | Spare DC load | Spare DC load |

DS18B20 GPIO4, DHT22 GPIO18, I2C GPIO21/22, RS485 RX/TX/DE-RE
GPIO16/17/23 and OUT1–8 GPIO13/19/14/27/25/26/32/33 are common to
all profile builds. A0–A3 are ADS1115 inputs at I2C address 0x48
(ADDR grounded); they are **not** ESP32 GPIO/ADC pins. External AC power
and switching contacts must remain outside these boards.

## Existing Aquaculture firmware migration (not performed)

The current `include/pins.h` and `src/main.cpp` target a DevKit/relay
prototype, not this board. DS18B20 GPIO4, I2C GPIO21/22 and LED GPIO2
already match; **DHT22 GPIO15 must become GPIO18** (GPIO15 is a strapping
pin). Current PUMP GPIO13, AERATOR GPIO12, CIRCULATION GPIO14 and FEEDER
GPIO27 do not match the illustrative Aquaculture OUT1 aerator/OUT2 pump
sequence above; GPIO12 is also a strapping pin. The firmware currently
drives four outputs, while this board provides eight; update pin definitions,
output names, initialization, output-state writes, failure defaults and
MQTT mapping **together**, then test boot with loads disconnected. Merely
editing macros would misassign pump/aerator and leave four outputs unused.

The current `read_sensors()` uses `analogRead(PH_PIN=34,
TURBIDITY_PIN=35, DO_PIN=36, CO2_PIN=39)` with placeholder conversions;
it does not sample ADS1115 A0–A3. Port sensor acquisition, scaling,
calibration, fault handling and profile-specific channel names to the
ADS1115 before using this PCB. Existing `docs/architecture.md` and
`docs/sensors.md` describe legacy/alternative wiring (including GPIO34
and possibly 5 V sensors); they are not instructions to connect 5 V to
this board's ADS1115. Check all interlocks against the new assignments.
Keep current firmware on its original prototype wiring until a separate,
tested hardware-target build is available; do not flash it to a loaded
Rev A1 board.
