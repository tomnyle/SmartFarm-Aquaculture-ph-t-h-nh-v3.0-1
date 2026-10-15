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
points**, not fabrication-ready boards. The project files were generated in KiCad 9
format; open them in KiCad 10 and keep a backup before accepting any format conversion.
Verify the exact DevKit dimensions and connector mating, route the copper, then run
schematic ERC and PCB DRC before manufacturing.

PCB-B is an interface/driver between the ESP32 and a separate purchased 8-channel relay
module; it does not switch mains loads itself. Its NPN open-collector outputs assume a
low-level-trigger module, as confirmed by the user. The supplied module photo shows
SRD-24VDC-SL-C relays and HIGH/COM/LOW selectors; set the selector(s) to LOW. The 24 V
marking refers to the relay coils and does not define the input voltage/current. Do not
connect module DC+ to either PCB. Verify the input circuit and whether module DC- must
share PCB-B ground before wiring, to avoid defeating isolation or overloading the NPN
drivers.

Firmware reads pH, DO, turbidity and CO₂ via ADS1115 A0–A3 at 0x48, with SDA/SCL on
GPIO21/GPIO22. PCB-A's fixed 12 kΩ/20 kΩ dividers scale a 0–5 V sensor output to a
maximum of about 3.125 V; verify sensor range and calibrate each conversion. The eight
relay outputs map to GPIO13, GPIO25, GPIO14, GPIO27, GPIO32, GPIO33, GPIO26 and GPIO23.
All eight pins initialize OFF; only the first four currently have application behaviors.

Keep mains switching on the purchased certified relay module. Have a qualified person
perform any mains wiring and commissioning.
