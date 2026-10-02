# Wiring and Hardware Compatibility

## Firmware pinout

The current PlatformIO firmware (`include/pins.h`) targets an ESP32 DevKit and
uses these connections:

| Function | ESP32 connection |
| --- | --- |
| DS18B20 data | GPIO4, with a 4.7 kΩ pull-up to the sensor supply |
| DHT22 data | GPIO15 |
| pH amplifier output | GPIO34 |
| Turbidity sensor output | GPIO35 |
| Dissolved-oxygen amplifier output | GPIO36 |
| CO₂ sensor output | GPIO39 |
| BH1750 SDA / SCL | GPIO21 / GPIO22 |
| Pump / aerator / circulation / feeder relay inputs | GPIO13 / GPIO12 / GPIO14 / GPIO27 |
| Status LED | GPIO2 |

Power sensors from the voltage specified by their manufacturer and connect
their ground to ESP32 ground. ESP32 analog inputs are **not 5 V tolerant**:
condition each analog output to stay within the ESP32 input limits before
connecting it. GPIO34–GPIO39 are input-only pins.

GPIO12 is an ESP32 boot-strapping pin. The aerator interface must not pull it
to an unsafe level during reset; verify boot behavior on the actual relay
module and board. The firmware drives relay inputs HIGH for ON and LOW for OFF,
so confirm the module's input polarity before connecting it.

## KiCad reference schematics

The separate KiCad 9 projects are under
`hardware/kicad/universal_smart_farm_controller/`. They are **not a drop-in
pinout match** for the current firmware:

- PCB-A uses GPIO18 for DHT22, GPIO19 for OUT2, GPIO25/26/32/33 for OUT5–OUT8,
  and an ADS1115 for analog channels.
- The current firmware uses GPIO15 for DHT22, GPIO12 for the aerator, and
  direct ESP32 ADC inputs GPIO34/35/36/39.
- PCB-A and PCB-B are schematic-only references. They contain no PCB layout,
  manufacturing outputs, or KiCad 9 ERC/DRC results.

Do not wire the firmware to these schematics or fabricate either board until
the firmware/hardware pin map is reconciled and the design is reviewed and
validated. The project README records additional limitations and review items.

## Load and commissioning safety

Relay GPIOs are logic signals, not load power outputs. Use an appropriately
rated, isolated driver/relay assembly and a separately protected supply for
loads. Never connect mains wiring to the ESP32 or these reference schematics.
Confirm load inrush, fault current, wire gauge, fuse ratings, grounding, and
enclosure/environmental ratings with a qualified hardware engineer.

`SENSOR_TEST_MODE` is enabled by default in `include/app_config.h`; keep outputs
locked while checking sensors. Sensor conversion/calibration and relay behavior
must be verified against real instruments and loads before livestock operation.
