# PCB-A/PCB-B Matched Hardware

This repository defines a matched low-voltage controller and relay-interface pair for
the 30-pin ESP32 DevKit shown in the supplied photo. The signal-to-pin contract is in
[`../hardware/interconnect.csv`](../hardware/interconnect.csv); board-level circuits,
connector definitions, and BOMs are in:

- [`../hardware/pcb-a.md`](../hardware/pcb-a.md) — controller/sensor board
- [`../hardware/pcb-b.md`](../hardware/pcb-b.md) — eight-channel relay-interface board

PCB-B is an interface/driver between the ESP32 and a separate, purchased 8-channel
relay module; it does not switch mains loads itself. Its NPN open-collector outputs
assume a relay module configured for low-level trigger. The actual relay board must be
checked for input polarity, internal input current limiting, supply requirements, and
isolation topology before connection.

Firmware reads pH, DO, turbidity, and CO₂ via ADS1115 A0–A3 at 0x48, with SDA/SCL on
GPIO21/GPIO22. The eight relay outputs map to GPIO13, GPIO25, GPIO14, GPIO27, GPIO32,
GPIO33, GPIO26, and GPIO23. All eight pins initialize OFF; the first four have named
application behaviors and the remaining four stay OFF until firmware support is added.

**These are electrical design specifications, not manufacturing-ready KiCad files.**
Before PCB layout or fabrication, confirm the exact DevKit dimensions, relay-module
pinout, electrical characteristics, and sensor output ranges. Never connect raw 5 V
analog signals to ADS1115 inputs; use the specified divider configuration. Keep mains
switching on the certified relay module and have mains wiring done by a qualified person.
