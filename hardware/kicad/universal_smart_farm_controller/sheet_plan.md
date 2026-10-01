# Hierarchical schematic plan (not schematic source)

Make **two separate KiCad projects/board files** from this reference. The supplied
project is the blank PCB-A starting point; use KiCad's New Project command for
PCB-B. Do not put both boards on one PCB outline.

| Project | Sheet | Contents |
| --- | --- | --- |
| PCB-A Controller | Root | Title, reference-only notice, hierarchical sheets, interboard JA |
| PCB-A Controller | 01_power_core | JA, buck +5V, LDO +3V3, ESP32 U1, EN/BOOT, status LED |
| PCB-A Controller | 02_sensor_analog | DS18B20, DHT22, ADS1115 U2, four conditioned A0-A3 inputs |
| PCB-A Controller | 03_comms | UART programming header J4, RS485 U3, I2C expansion J9 |
| PCB-B Power & Output | Root | Input protection J10/F0/D1, load fuse F9, interboard JB |
| PCB-B Power & Output | 01_outputs | Eight repeated independently fused MOSFET channels OUT1-OUT8 |

Use hierarchical pins for inter-sheet nets and matching net labels on both
sides of the keyed interboard cable. The CSV tables describe **intended**
connectivity; they are not KiCad-native netlists and cannot be imported as
wires. Assign real symbols/footprints and electrical pin numbers before
running ERC and updating either PCB.
