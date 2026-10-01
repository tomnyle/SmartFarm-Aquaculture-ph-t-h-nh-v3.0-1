# PCB-A controller layout starter

Open `pcb_a_controller.kicad_pro` in **KiCad 9**, then open its schematic and
board (`pcb_a_controller.kicad_sch` / `.kicad_pcb`). This is a placement
template, **not a routed, ERC/DRC-approved or manufacture-ready design**.
It incorporates only the PCB-A schematic and signal references from the
directly related draft schematic work; it does not include PCB-B or mains.

The board has a provisional 130 × 100 mm outline (no mounting holes), 54
placed standard-library footprints grouped as power, ESP32/RF, analog, RS485
and board-edge headers, and assigned pad nets from the schematic. U1 faces
the top edge. Its antenna projects toward/outside that edge; the on-board
antenna courtyard strip (x=141–189 mm, y=115–121.2 mm) is a **both-copper-layer
rule-area keepout** prohibiting copper pours, tracks, vias and pads. It does
not prohibit footprints because it overlaps the ESP32 module itself: do not
place any *other* component there. Do not put an enclosure, cable or metal in
front of or under the antenna; check the ESP32-WROOM-32E module datasheet and
manufacturer clearance before fixing board dimensions. Neither the board
outline nor component locations are final mechanical specifications.

**Nothing is routed:** there are zero tracks and no copper pours. In
particular, do not route the TPS5430 switch/feedback loop, 12 V/5 V/3.3 V
rails, ground return, analog inputs or RS485 before selecting components and
reviewing current, EMI, transient protection and return paths. C12 (the
provisional 220 µF buck output capacitor) has **no selected package in the
source schematic** and is intentionally **not placed**. Choose a real part
with suitable voltage, ripple, ESR, footprint and polarity, assign its
footprint in the schematic, then use **Tools → Update PCB from Schematic**
with KiCad 9. Do not treat missing C12 as optional in a powered circuit.
The ADS1115 inputs are nominal 0–5 V via dividers, not field-protected.
The RS485 interface lacks verified field surge/bias design. USB-UART J4 pin
2 is a **3.3 V reference output**, never a power input.

## Connector orientation (viewed from the component/top side)

J1 is a vertical **shrouded 2×6 IDC header**, with pin 1 at the top-left
of its footprint near the board's top/right edge, pin 2 immediately to its
right, and successive rows numbered 3/4 through 11/12 toward the bottom.
The shroud notch is on the left. Use a keyed mating assembly with **pin 1
to PCB-B pin 1**; do not assume a straight ribbon or mirrored bottom-side
view preserves numbering. Check the mating connector, shroud and cable
orientation physically before powering. J1 supplies the controller only;
it must not carry PCB-B load return current.

| Header | Pin order (1 onward) |
| --- | --- |
| J1 | protected 12 V, GND, OUT1, OUT2, OUT3, OUT4, OUT5, OUT6, OUT7, OUT8, GND, GND |
| J2 DS18B20 | 3V3, DATA, GND |
| J3 bare DHT22 | 3V3, DATA, NC, GND |
| J4 external 3V3 UART | GND, 3V3 reference, ESP TX, ESP RX |
| J5–J8 A0–A3 | 5V, sensor signal, GND |
| J9 I2C | 3V3, SDA, SCL, GND |
| J19 RS485 | A, B, GND |

The single-row connectors' pin 1 is their square pad; check the physical
sensor's connector orientation and do not substitute a DHT22 module for the
bare four-pin device without checking its pinout. `../pin_map.csv` and
`../connector_summary.csv` define the intended GPIO and interboard mapping;
the existing firmware has DHT22/OUT2 and ADC-path differences and has **not**
been changed to match this board.

## Review before any fabrication

The imported schematic had its labels and no-connect markers vertically
reflected relative to actual KiCad symbol pin coordinates. Those positions
have been corrected in this copy (without changing the intended pin map);
the board's pad nets now match J1 pin 1 = protected 12 V, pins 2/11/12 =
GND, and OUT1–OUT8 = pins 3–10. Verify all pinouts, regulator feedback and
decoupling, component/package selections, antenna geometry, mating header,
clearances and mechanical fit in **KiCad 9**. Run schematic ERC, synchronize
the PCB, route/review it, then run PCB DRC. Do not generate fabrication
outputs until those checks and hardware safety review are complete.

The available environment had **KiCad CLI 7.0.11**, which loaded and exported
the board as SVG, and its board parser reloaded all 54 footprints, pad nets
and the keepout. KiCad 7 cannot perform ERC on this KiCad 9 schematic and
its CLI has no PCB DRC command. A KiCad 9 open/sync/ERC and final DRC
remain mandatory; no Gerbers or drill files are supplied.
