# PCB-A — ESP32 Controller and Sensor Board

## Purpose

PCB-A is the shared controller. It accepts the pictured 30-pin ESP32 DevKit as a
replaceable module, connects sensors to the firmware's existing interfaces, and routes
eight logic outputs to PCB-B through J_AB. It does not carry relay-coil or mains current.

## DevKit socket

- Use two 1×15 female sockets, 2.54 mm pitch, matching the two header rows of the
  photographed ESP32-WROOM-32 USB-C DevKit.
- Mount the antenna end over a PCB edge or RF keep-out; do not place copper, sensor
  cables, or metal enclosure parts under/in front of the antenna.
- The photo does not establish exact board dimensions or row spacing. Verify the
  purchased DevKit's drawing and physical sample before fixing the socket spacing,
  outline, mounting holes, or USB-C cutout.
- Power the DevKit through its USB-C connector during initial testing. Do not connect
  another 5 V source to its 5V/VIN header at the same time unless the exact DevKit has
  verified power-path isolation.

## Circuit blocks and PCB-A BOM

| Ref | Part/value | Count | Notes |
|-----|------------|-------|-------|
| U1 | 30-pin ESP32 DevKit, ESP-WROOM-32 | 1 | Exact model/footprint must be verified |
| J1, J2 | 1×15 female socket, 2.54 mm | 2 | DevKit headers |
| U2 | ADS1115, 16-bit ADC | 1 | I²C address 0x48; power at 3.3 V |
| C1 | 100 nF ceramic | 1 | At ADS1115 VDD |
| C2 | 10 µF ceramic | 1 | Local 3.3 V decoupling |
| R1 | 4.7 kΩ | 1 | DS18B20 DATA pull-up to 3.3 V |
| R2, R3 | 4.7 kΩ | 2 | I²C pull-ups to 3.3 V; omit if already fitted on modules |
| J3 | 1×3, 2.54 mm | 1 | DS18B20: 3V3, DATA, GND |
| J4 | 1×4, 2.54 mm | 1 | I²C expansion: 3V3, GND, SDA, SCL |
| J5–J8 | 1×3, 2.54 mm | 4 | Analog sensors: VCC, GND, SIGNAL |
| JP1–JP4 | 3-pad solder jumper | 4 | Select direct 0–3.3 V or 2:1 divider for 0–5 V signals |
| R4–R11 | 10 kΩ / 20 kΩ divider pairs | 4 pairs | 10 kΩ upper, 20 kΩ lower; selected output is 2/3 of sensor input |
| J_AB | 2×5 shrouded keyed header | 1 | Board-to-board connector; pinout in `interconnect.csv` |
| TP1–TP4 | Test pads | 4 | 3V3, 5V/VIN, GND, SDA/SCL as space permits |

### Sensor channel wiring

| Signal | ADS1115 | ESP32 interface |
|--------|---------|-----------------|
| pH amplifier | A0 | SDA GPIO21, SCL GPIO22 |
| DO amplifier | A1 | Same shared I²C bus |
| Turbidity | A2 | Same shared I²C bus |
| CO₂ module | A3 | Same shared I²C bus |

Wire the analog outputs through JP1–JP4. Set each jumper to **DIRECT** only for a
0–3.3 V maximum output. Set it to the **2:1 divider** for a 0–5 V maximum output;
the 10 kΩ/20 kΩ pair scales 5 V to about 3.33 V. Never short both jumper positions.
The 2:1 divider changes the transfer scale and the firmware's current linear estimates
are placeholders; calibrate each sensor in firmware/application settings before use.
Do not exceed the ADS1115 supply voltage on any input.

The DS18B20 pull-up connects DATA to 3.3 V. BH1750 can share the 3.3 V I²C bus. Keep
the I²C pull-ups to 3.3 V only; do not use 5 V pull-ups.

## PCB-A pin assignment

| Signal | ESP32 pin |
|--------|-----------|
| DS18B20 DATA | GPIO4 |
| I²C SDA / SCL | GPIO21 / GPIO22 |
| Relay outputs 1–8 | GPIO13, GPIO25, GPIO14, GPIO27, GPIO32, GPIO33, GPIO26, GPIO23 |
| Built-in status LED | GPIO2 |

GPIO12 is intentionally unused as a relay signal because it is a boot-strapping pin.
GPIO34–GPIO39 are not connected to the analog sensors; the external ADS1115 handles
those signals.
