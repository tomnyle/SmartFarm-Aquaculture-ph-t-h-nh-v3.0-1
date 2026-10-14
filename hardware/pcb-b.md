# PCB-B — Eight-Channel Relay Interface

## Purpose

PCB-B connects PCB-A's 3.3 V GPIO signals to the inputs of a separate purchased
8-channel relay module. It provides one NPN open-collector sink and one logic-state LED
per channel. It is **not** a mains relay board and does not contain the relay contacts.

This design assumes the purchased module has eight **low-level-trigger** inputs with
internal current limiting/pull-up, configurable by its trigger jumper if applicable.
Confirm those details and the input current in that exact module's documentation before
connecting it. If it only supports high-level trigger, this PCB-B design is incompatible
without redesign.

## Circuit per channel

- Q1–Q8: BC817-40 NPN transistor (or equivalent small-signal NPN).
- R1–R8: 4.7 kΩ from PCB-A `RELAYn` to transistor base.
- R9–R16: 10 kΩ base-to-emitter pulldown; keeps each channel inactive while GPIO is
  floating at reset.
- D1–D8: green status LED from the `RELAYn` signal to GND through 1 kΩ. It lights when
  the corresponding GPIO is HIGH.
- Transistor emitter to GND; collector to the matching relay module `INn`.
- Relay-module logic GND to PCB-B GND. Relay-board supply goes to the relay module
  according to its manual, not through J_AB.

GPIO HIGH turns on the NPN and pulls the matching relay input LOW. This preserves
active-HIGH firmware semantics while supporting a low-trigger relay board. When ESP32
is reset or unpowered, the base pulldown turns the NPN off; the module's own input
circuit must hold its relay inactive. Validate this OFF state before connecting loads.

Do not add a flyback diode across the purchased module's relay coils if it already has
coil suppression. PCB-B's NPNs only drive module input pins and are not relay-coil
drivers. The purchased module's onboard transistor, coil diode, LEDs, optocouplers, and
power requirements must be checked independently.

## PCB-B BOM and connectors

| Ref | Part/value | Count | Notes |
|-----|------------|-------|-------|
| Q1–Q8 | BC817-40 NPN SOT-23 | 8 | Open-collector input sinks |
| R1–R8 | 4.7 kΩ | 8 | GPIO base resistors |
| R9–R16 | 10 kΩ | 8 | Base-emitter pulldowns |
| D1–D8 | Green LED, 0603/0805 | 8 | Channel command indicators |
| R17–R24 | 1 kΩ | 8 | LED current limiting |
| J_AB | 2×5 shrouded keyed header | 1 | Mates to PCB-A J_AB; exact pin order in CSV |
| J1 | 1×5 pluggable terminal | 2 | GND, IN1–IN4 and GND, IN5–IN8 |

Wire J1 signal outputs to the relay module's eight `IN` pins and tie J1 ground to the
module's logic ground. Connect the module's relay/coil supply separately, at its rated
voltage/current. Do not assume optocoupler isolation remains effective; follow its
specified jumper, logic-ground, and coil-supply arrangement.

## Bring-up checks

1. Disconnect the relay module and loads. Power PCB-A by USB-C and verify all eight
   PCB-B LEDs are OFF after reset.
2. Verify GPIO13/25/14/27 command only channels 1–4. Channels 5–8 remain OFF.
3. Connect the relay module logic side only, set its trigger selector to LOW, and verify
   that each relay remains OFF at boot and responds to the expected channel.
4. Verify relay-module power input and contact ratings separately. Test with no mains
   load first. Have a qualified person perform mains wiring and commissioning.
