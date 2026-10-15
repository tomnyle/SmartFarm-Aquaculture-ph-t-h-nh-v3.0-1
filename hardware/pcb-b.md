# PCB-B — Eight-Channel Relay-Input Interface

## KiCad files

Open [`pcb-b/pcb-b.kicad_pro`](pcb-b/pcb-b.kicad_pro) in KiCad 10. The
schematic is `pcb-b/pcb-b.kicad_sch`; the provisional two-layer layout is
`pcb-b/pcb-b.kicad_pcb`. Both projects use the shared library in `../SmartFarm`.

The files were generated in KiCad 9 format. KiCad 10 may offer to update the project
when first opened; keep a backup before accepting format conversion.

The layout has component placement, a board outline, and assigned pad nets, but no
copper routing. It is not ready for fabrication. Route and run KiCad DRC after
confirming the relay module, connector/cable orientation, and PCB-A mating geometry.

## Purpose and relay-module compatibility

PCB-B converts PCB-A's 3.3 V GPIO commands into eight NPN open-collector sinks for a
separate purchased 8-channel relay module. PCB-B does not contain relay contacts and
does not switch mains.

The supplied photo shows an eight-channel module marked SRD-24VDC-SL-C, with DC+/DC-
power terminals, IN1–IN8 inputs, and HIGH/COM/LOW trigger selectors. The relay marking
indicates 24 VDC coils; it does not establish the input voltage or input current. You
confirmed that this module is to be operated in LOW-trigger mode. Set its trigger
selector(s) to LOW, not HIGH.

PCB-B does not generate or switch 24 V. Never connect the relay module's DC+ terminal
to PCB-A or PCB-B. Connect only its IN1–IN8 lines to the matching PCB-B outputs. The
PCB-B ground must provide the NPN emitters' return; connect it to the module's DC-
control reference only after confirming from the module documentation that this
connection is required and safe. Verify input current, input-voltage limits, selector
wiring, and isolation before applying power. The 2.2 kΩ base drive and open-collector
interface must be checked against the module's input circuit.

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
IN6, IN7, IN8. Connect each signal to the matching input on the relay module. Power the
module separately according to its documentation; its 24 V relay-coil supply and
contacts are not routed through PCB-A or PCB-B. Do not assume that PCB-B's GND should
be tied to module DC- until its input/isolator circuit is verified.

Do not add flyback diodes across relay coils on PCB-B: its transistors drive module
inputs, not coils. Check the purchased module's onboard suppression, optocoupler
configuration, logic ground, and supply arrangements independently.

## Bring-up checks

1. Disconnect the relay module and all loads. Power PCB-A by USB-C; confirm all eight
   PCB-B LEDs remain OFF after reset.
2. Verify that GPIO13/25/14/27 control only channels 1–4. Channels 5–8 should remain
   OFF with current firmware.
3. With the module disconnected from loads, set its trigger selector(s) to LOW. Verify
   the exact input circuit and required ground reference, then connect and test each
   channel. Confirm the OFF state at boot and when the ESP32 is reset.
4. Verify relay-module power and contact ratings separately. Keep mains off these
   boards; have a qualified person handle any mains wiring and commissioning.
