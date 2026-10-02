# Removable ESP32 Carrier Board

This wiring concept keeps the ESP32 DevKit removable while leaving sensor and
load wiring on a fixed carrier board. It is a design reference, not a
manufacturing-ready schematic or PCB.

## Block diagram

```text
12 V input
  ├── Fuse/protection ──> 12 V load supply, if required by the loads
  ├── DC/DC converter ──> 5 V rail ──> ESP32 DevKit 5V/VIN socket pin
  └── DC/DC converter ──> relay module supply, only if its rated voltage is 5 V

ESP32 DevKit in two female socket rows
  ├── GPIO ──> sensor terminal blocks
  ├── GPIO ──> relay-driver inputs ──> relay contacts ──> external loads
  └── I²C ──> optional I²C sensor/ADC connector
```

The DevKit plugs into two female headers on the carrier. Sensor and load
terminal blocks remain wired to the carrier, so the module can be unplugged
without removing those field wires.

## Module and carrier connectors

Choose and freeze the exact ESP32 DevKit model before making the footprint.
Use its manufacturer pinout and mechanical drawing to set the two socket-row
positions, pin count, board outline, USB connector clearance, and mounting
holes. The firmware selects `esp32dev`, but that does not define a unique
physical DevKit footprint. Do not assume a different ESP32 DevKit will fit or
have the same socket pin order.

Provide clearly labeled, removable terminal blocks on the carrier:

| Connector | Signals |
| --- | --- |
| Power input | `12V_IN`, `GND` |
| Temperature sensor | `3V3` or sensor-rated supply, `GND`, `DATA` |
| Analog sensors | Per channel: `SENSOR_VCC`, `GND`, `OUT` |
| I²C expansion | `3V3`, `GND`, `SDA`, `SCL` |
| Relay control | One `IN` per output plus logic `GND` |
| Load contacts | `COM`, `NO`, `NC` per relay, rated for the intended load |

Use terminal blocks appropriate for the wire gauge and maximum voltage/current.
Keep mains/high-voltage terminals physically separated from the low-voltage
ESP32, sensor, and USB areas; use suitable enclosure, spacing, fusing, and
qualified review for mains wiring.

## Firmware pin map

The current mapping comes from `include/pins.h`. The GPIO numbers below are
the module pins to route from the selected DevKit sockets to the carrier
connectors; verify the actual socket positions against that board's pinout.

| Function | ESP32 GPIO | Carrier connection | Notes |
| --- | ---: | --- | --- |
| pH analog input | 34 | Analog sensor `OUT` | Firmware currently uses `analogRead()` |
| Turbidity analog input | 35 | Analog sensor `OUT` | Firmware currently uses `analogRead()` |
| Dissolved oxygen analog input | 36 | Analog sensor `OUT` | Firmware currently uses `analogRead()` |
| CO₂ analog input | 39 | Analog sensor `OUT` | Firmware currently uses `analogRead()` |
| DS18B20 1-Wire | 4 | Temperature `DATA` | Add a 4.7 kΩ pull-up to the sensor supply |
| DHT22 data | 15 | DHT `DATA` | GPIO15 is a boot-strapping pin |
| I²C SDA / SCL | 21 / 22 | I²C expansion | Reserved by firmware setup; optional peripherals need firmware support |
| Pump relay input | 13 | Relay driver `IN` | Do not drive a pump directly from GPIO |
| Aerator relay input | 12 | Relay driver `IN` | GPIO12 is a boot-strapping pin; verify its startup loading |
| Circulation relay input | 14 | Relay driver `IN` | Do not drive a pump directly from GPIO |
| Feeder relay input | 27 | Relay driver `IN` | Do not drive a motor directly from GPIO |
| Status LED | 2 | Optional LED | Confirm the selected DevKit's onboard LED wiring |

GPIO34, GPIO35, GPIO36, and GPIO39 are input-only. The ADC signals must be
conditioned so their voltage remains within the ESP32 input limits under normal
operation and fault conditions. Do not connect a sensor output directly until
its voltage range has been checked.

The existing documentation also describes ADS1115-based pH/DO sensing, but
`src/main.cpp` currently reads pH, turbidity, DO, and CO₂ from the internal ADC
GPIOs. The ADS1115 and optional BH1750 connections are therefore expansion
interfaces only until the firmware is updated to use them. Confirm and
calibrate the actual sensor/ADC design before assigning these terminals.

GPIO12 and GPIO15 have boot-strapping functions. The listed firmware uses them
for relay and DHT signals, respectively; a carrier design must ensure connected
modules do not force invalid levels during reset. Prefer non-strapping pins for
externally loaded signals in a revised pin map, and update `include/pins.h` and
the firmware together before changing the wiring.

## Power and output handling

- Feed the carrier from a protected 12 V input and use a correctly rated
  DC/DC converter to supply the DevKit through its documented `5V`/`VIN` input.
  Never feed 12 V to the DevKit 5 V pin.
- Size the converter for the DevKit, attached sensors, and any logic loads.
  Keep relay-coil or motor current off the DevKit's 3.3 V rail.
- Use a relay module or transistor/MOSFET driver rated for the load. GPIO pins
  provide logic signals only. Add suitable flyback/surge suppression and
  isolation as required by the selected driver and load.
- Route load current through relay contacts and separate power traces/connectors,
  not through the ESP32 socket or sensor ground wiring.
- Establish a logic-ground reference between the ESP32 and non-isolated driver
  inputs. Follow the relay module datasheet if it provides genuine galvanic
  isolation; do not assume that an optocoupler automatically isolates the
  complete module.
- Choose relay contact ratings, fuses, wire gauge, creepage/clearance, and
  enclosure based on the actual DC/AC loads. Verify the relay's active level
  and the outputs' power-up state during commissioning.

## Replacement and validation

For a replacement with the same DevKit model, unplug the module and fit another
board of that exact model. For another ESP32 model, verify mechanical fit,
socket pin order, GPIO availability, ADC characteristics, boot-strapping pins,
and supply requirements; use a model-specific adapter/carrier and revise the
firmware pin map as needed.

Before connecting probes or loads, check the unpowered board for shorts and
continuity, then power it from a current-limited supply. Validate the 5 V and
3.3 V rails, each signal voltage, reset/boot behavior, relay startup state, and
load-side isolation. This guide does not replace schematic capture, PCB ERC/DRC,
or electrical safety review.
