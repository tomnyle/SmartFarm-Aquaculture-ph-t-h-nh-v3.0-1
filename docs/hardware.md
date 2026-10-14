# Hardware Design: PCB-A and PCB-B

## Board split

- **PCB-A (shared controller/sensor board):** socket for one specific 30-pin ESP32
  DevKit model, 3.3 V supply/decoupling, DS18B20 connector, ADS1115 and analog sensor
  connectors, I²C connector, status LEDs, and a keyed connector to PCB-B.
- **PCB-B (farm-specific output board):** eight protected output channels with a status
  LED per channel, or connectors for a compatible purchased 8-channel relay module.
  Select relay/contact ratings for the actual pump, aerator, feeder, valve, and lighting
  loads; the boards should not be assumed interchangeable across installations.

The DevKit is a replaceable module, not a bare ESP32 chip. Choose the exact 30-pin
variant first, then verify its two-row header spacing, board outline, USB connector
clearance, antenna keep-out, and pin labels against the manufacturer's drawing. Do not
infer footprint or pin order from the generic `esp32dev` PlatformIO target.

## Current signal and relay mapping

| Function | PCB-A connection | Notes |
|----------|------------------|-------|
| DS18B20 data | GPIO4 | 4.7 kΩ pull-up to 3.3 V |
| I²C SDA / SCL | GPIO21 / GPIO22 | 3.3 V pull-ups; ADS1115 and BH1750 |
| pH / DO / turbidity / CO₂ | ADS1115 A0 / A1 / A2 / A3 | Address 0x48; input range GND–3.3 V |
| Pump / aerator / circulation / feeder | GPIO13 / GPIO25 / GPIO14 / GPIO27 | Current firmware outputs |
| Reserved relay channels 5–8 | GPIO32 / GPIO33 / GPIO26 / GPIO23 | Hardware pin reservation only; firmware does not control these yet |

GPIO34–GPIO39 are input-only and are no longer the firmware's analog inputs. GPIO12 is
not assigned to a relay because its boot-strapping function can prevent reliable startup.
The firmware defaults to active-high relay inputs. For active-low modules, set
`RELAY_ACTIVE_LEVEL` to `LOW` in `include/pins.h`; confirm polarity and startup behavior
on the actual module before connecting loads.

## Using a purchased 8-channel relay module

Yes. The ESP32 can control a purchased module as an external PCB-B, provided the
module's input interface is compatible with 3.3 V GPIO and the power and contact ratings
match the loads. A module advertised as "5 V" may refer only to its relay-coil supply;
that does not prove that its input thresholds or optocouplers work reliably from 3.3 V.

Before wiring, check the module datasheet and verify:

1. Each `IN` accepts the ESP32's 3.3 V logic level, or add a transistor/logic-level
   interface on PCB-B. Never connect a 5 V logic output back to an ESP32 GPIO.
2. Whether inputs are active-high or active-low; configure `RELAY_ACTIVE_LEVEL`
   accordingly. Provide hardware biasing so all channels remain OFF while ESP32 GPIOs
   are floating during reset and boot.
3. Relay-coil supply voltage and total current. Use a separate appropriately rated
   supply if needed; do not power eight relay coils from the DevKit's 3.3 V regulator.
4. Whether the module already includes a transistor/driver, flyback diode, and one
   indicator LED per channel. If those are present and properly rated, an additional
   PCB-B driver/diode is not needed for the relay coils; otherwise provide them.
5. Whether opto-isolation is real and how it is wired. Isolation requires the module's
   isolation jumper/power arrangement and separate supplies to be used as specified;
   sharing grounds or leaving a jumper fitted may defeat it.
6. Contact voltage/current ratings, load inrush, and suitability for inductive loads.
   Relay contact ratings must cover the actual AC/DC load, not just its steady current.

The firmware currently controls only four named outputs. On an 8-channel module,
connect those four to channels 1–4; leave channels 5–8 disconnected or disabled until
their behavior is added and tested in firmware. Many modules already have channel
indicator LEDs, but confirm this before omitting per-output LEDs from PCB-B.

Keep low-voltage logic and mains wiring physically separated, use appropriate fusing,
enclosures, creepage/clearance, and strain relief. Mains wiring and commissioning should
be done by a qualified person. Test the board with no load and confirm every relay's
OFF state across power-up, reset, firmware upload, and Wi-Fi loss before connecting
aquaculture equipment.
