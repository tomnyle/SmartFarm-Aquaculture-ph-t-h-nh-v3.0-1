# PCB-B — Eight-Channel Relay-Input Interface

## KiCad files

Open [`pcb-b/pcb-b.kicad_pro`](pcb-b/pcb-b.kicad_pro) in KiCad 9 or newer. The
schematic is `pcb-b/pcb-b.kicad_sch`; the provisional two-layer layout is
`pcb-b/pcb-b.kicad_pcb`. Both projects use the shared library in `../SmartFarm`.

The layout has component placement, a board outline, and assigned pad nets, but no
copper routing. It is not ready for fabrication. Route and run KiCad DRC after
confirming the relay module, connector/cable orientation, and PCB-A mating geometry.

## Purpose and relay-module compatibility

PCB-B converts PCB-A's 3.3 V GPIO commands into eight NPN open-collector sinks for a
separate purchased 8-channel relay module. PCB-B does not contain relay contacts and
does not switch mains.

This circuit assumes a low-level-trigger relay module whose inputs have suitable
current limiting/pull-up. The actual module from the purchase listing has not been
electrically verified. Confirm its trigger polarity, input current, supply and isolation
requirements before connection; do not assume the 2.2 kΩ base drive is suitable for an
unknown input load.

## Circuit and BOM

Each channel uses:

- Q1–Q8: BC817-40 NPN, SOT-23; emitter to GND, collector to matching relay input.
- R1–R8: 2.2 kΩ from `RELAYn` to the transistor base.
- R9–R16: 10 kΩ base-to-emitter pulldowns to hold the driver off while GPIO is floating.
- R17–R24 and D1–D8: 1 kΩ LED limiters and green channel-command LEDs from `RELAYn` to GND.

GPIO HIGH turns on the NPN and pulls the matching relay-module input LOW. The LED
indicates the GPIO command, not confirmation that a relay coil energized. Check that
the relay module keeps each relay OFF when PCB-B is disconnected or unpowered.

| Ref | Part/value | Count | Notes |
|-----|------------|-------|-------|
| J1 | 2×5 header, 2.54 mm pitch | 1 | Mates with PCB-A J9; pin order in `interconnect.csv` |
| Q1–Q8 | BC817-40 NPN, SOT-23 | 8 | Low-side input sinks |
| R1–R8 | 2.2 kΩ | 8 | GPIO base resistors |
| R9–R16 | 10 kΩ | 8 | Base-emitter pulldowns |
| D1–D8 | Green LED, 0603 | 8 | Command indicators |
| R17–R24 | 1 kΩ | 8 | LED current limiters |
| J2, J3 | 1×5 through-hole headers | 2 | GND plus four relay-module input signals each |

The board's 2×5 J1 uses an unshrouded header footprint; verify pin 1 and cable
orientation before power-up. J2 pin order is GND, IN1, IN2, IN3, IN4; J3 is GND, IN5,
IN6, IN7, IN8. Connect these signals to the purchased module's logic-input pins and
connect its logic GND to PCB-B GND. Power the relay module separately as specified by
its manufacturer. Its coil supply and contacts are not routed through PCB-A or PCB-B.

Do not add flyback diodes across relay coils on PCB-B: its transistors drive module
inputs, not coils. Check the purchased module's onboard suppression, optocoupler
configuration, logic ground, and supply arrangements independently.

## Bring-up checks

1. Disconnect the relay module and all loads. Power PCB-A by USB-C; confirm all eight
   PCB-B LEDs remain OFF after reset.
2. Verify that GPIO13/25/14/27 control only channels 1–4. Channels 5–8 should remain
   OFF with current firmware.
3. Connect only the relay module logic side, set its trigger selector to LOW, and test
   each channel with no load. Confirm the OFF state at boot and when the ESP32 is reset.
4. Verify relay-module power and contact ratings separately. Keep mains off these
   boards; have a qualified person handle any mains wiring and commissioning.
