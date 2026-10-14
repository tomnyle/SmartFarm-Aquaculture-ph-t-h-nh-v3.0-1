# PCB-A/PCB-B Matched Hardware

This repository contains native KiCad projects for a low-voltage controller and relay
input interface using the pictured 30-pin ESP32 DevKit:

- [Open PCB-A in KiCad](../hardware/pcb-a/pcb-a.kicad_pro) — controller/sensor board
- [Open PCB-B in KiCad](../hardware/pcb-b/pcb-b.kicad_pro) — eight-channel interface
- [PCB-A design and BOM](../hardware/pcb-a.md)
- [PCB-B design and BOM](../hardware/pcb-b.md)
- [Inter-board pin contract](../hardware/interconnect.csv)

The projects include schematics, board outlines, component placements, project-local
symbols/footprints and assigned PCB pad nets. Both PCB layouts are **unrouted starting
points**, not fabrication-ready boards. Open the project files with KiCad 9 or newer,
verify the exact DevKit dimensions and connector mating, route the copper, then run
schematic ERC and PCB DRC before manufacturing.

PCB-B is an interface/driver between the ESP32 and a separate purchased 8-channel relay
module; it does not switch mains loads itself. Its NPN open-collector outputs assume a
low-level-trigger module with compatible input current. Verify that exact module's
polarity, input current, supply requirements, and isolation topology before connection.

Firmware reads pH, DO, turbidity and CO₂ via ADS1115 A0–A3 at 0x48, with SDA/SCL on
GPIO21/GPIO22. PCB-A's fixed 12 kΩ/20 kΩ dividers scale a 0–5 V sensor output to a
maximum of about 3.125 V; verify sensor range and calibrate each conversion. The eight
relay outputs map to GPIO13, GPIO25, GPIO14, GPIO27, GPIO32, GPIO33, GPIO26 and GPIO23.
All eight pins initialize OFF; only the first four currently have application behaviors.

Keep mains switching on the purchased certified relay module. Have a qualified person
perform any mains wiring and commissioning.
