# PCB-A — ESP32 Controller and Sensor Board

## KiCad files

Open [`pcb-a/pcb-a.kicad_pro`](pcb-a/pcb-a.kicad_pro) in KiCad 10. The
schematic is `pcb-a/pcb-a.kicad_sch`; the provisional two-layer layout is
`pcb-a/pcb-a.kicad_pcb`. The project uses the shared symbols and footprints in
`SmartFarm.kicad_sym` and `SmartFarm.pretty`.

The files were generated in KiCad 9 format. KiCad 10 may offer to update the project
when first opened; keep a backup before accepting format conversion.

The layout currently has component placement, board outline, and assigned pad nets,
but no copper routing. It is a KiCad starting point, not a fabrication-ready board.
The 30-pin DevKit socket spacing and antenna keep-out must be checked against the
exact purchased module before routing or fabrication.

## Purpose and connectors

PCB-A is the shared controller. It accepts a 30-pin ESP32 DevKit on two 1×15,
2.54 mm-pitch female sockets, connects sensors to the firmware interfaces, and sends
eight logic outputs to PCB-B. It does not carry relay-coil or mains current.

| Reference | Footprint / pin order | Use |
|-----------|-----------------------|-----|
| J1, J2 | 1×15 female sockets | ESP32 DevKit rows; confirm row spacing on the actual module |
| J3–J6 | 1×3 header: VCC, GND, SIGNAL | pH, DO, turbidity, CO₂ sensor outputs |
| J7 | 1×3 header: 3V3, DATA, GND | DS18B20, DATA on GPIO4 |
| J8 | 1×4 header: 3V3, GND, SDA, SCL | I²C expansion/BH1750 |
| J9 | 2×5 header, 2.54 mm pitch | PCB-B relay interface; logical connector `J_AB` in `interconnect.csv` |
| J10 | 1×3 header: 3V3, DATA, GND | DHT22, DATA on GPIO15 |

The custom 2×5 footprint is an unshrouded through-hole header pattern. Check the
selected mating connector and cable orientation; pin 1 is identified by a square pad.
Keep the DevKit antenna beyond the PCB edge or over a verified copper keep-out. The
photo does not establish exact board dimensions, socket spacing, mounting holes, or
USB-C clearance.

## Circuit and BOM

| Ref | Part/value | Count | Notes |
|-----|------------|-------|-------|
| U1 | ADS1115, SOIC-10 | 1 | Address 0x48; ADDR tied to GND; ALERT/RDY not used |
| C1 | 100 nF ceramic | 1 | ADS1115 local bypass |
| C2 | 10 µF ceramic | 1 | 3.3 V local decoupling |
| R1, R3, R5, R7 | 12 kΩ, 1% | 4 | Upper legs of fixed analog dividers |
| R2, R4, R6, R8 | 20 kΩ, 1% | 4 | Lower legs of fixed analog dividers |
| R9 | 4.7 kΩ | 1 | DS18B20 DATA pull-up to 3.3 V |
| R10, R11 | 4.7 kΩ | 2 | I²C pull-ups to 3.3 V; omit if bus modules already have pull-ups |
| R12, D1 | 1 kΩ, green LED | 1 each | GPIO2 status indicator |
| R13 | 4.7 kΩ | 1 | DHT22 DATA pull-up; omit if the sensor module includes one |
| JP1–JP4 | 3-pin solder-jumper footprint | 4 | Select each sensor supply: 3V3 or 5V/VIN |
| J1, J2 | 1×15 female sockets | 2 | 30-pin ESP32 DevKit |
| J3–J8, J10 | Through-hole headers | 7 | Sensor and I²C connectors listed above |
| J9 | 2×5 through-hole header | 1 | PCB-A-to-PCB-B signals and ground |

### Analog input path

Each sensor SIGNAL enters a fixed divider: 12 kΩ from SIGNAL to the ADS1115 input,
then 20 kΩ from that input to GND. The ratio is 20/(12+20), so a 5.0 V input becomes
about 3.125 V. This circuit is intended for positive 0–5 V sensor outputs; verify each
sensor's maximum output and common ground before connection. Do not use these inputs
for negative voltages or signals above 5 V. Calibrate the firmware conversion for the
actual sensor and divider.

JP1–JP4 select **sensor power only**: bridge the center pad to either 3V3 or 5V/VIN,
never both. The signal divider is always connected; it has no bypass/direct setting.
Use 3V3 for 3.3 V-only modules. The 5V/VIN rail depends on the DevKit power source.

| Sensor | Connector | ADS1115 input |
|--------|-----------|---------------|
| pH amplifier | J3 | A0 |
| DO amplifier | J4 | A1 |
| Turbidity | J5 | A2 |
| CO₂ module | J6 | A3 |

The ADS1115 runs at 3.3 V on shared I²C GPIO21/GPIO22. Keep all I²C pull-ups at 3.3 V;
do not connect 5 V pull-ups. BH1750 may share J8's I²C bus.

## ESP32 and inter-board mapping

| Function | ESP32 pin |
|----------|-----------|
| DHT22 DATA | GPIO15 |
| DS18B20 DATA | GPIO4 |
| I²C SDA / SCL | GPIO21 / GPIO22 |
| Relay outputs 1–8 | GPIO13, GPIO25, GPIO14, GPIO27, GPIO32, GPIO33, GPIO26, GPIO23 |
| Status LED | GPIO2 |

J9 and PCB-B J1 use the pin order in [`interconnect.csv`](interconnect.csv): pins 1–8
are RELAY1–RELAY8 and pins 9–10 are GND. GPIO12 is not used for a relay because it is
a boot-strapping pin. GPIO34–GPIO39 are not used for the sensor inputs; the ADS1115
handles the four analog channels.

During initial testing, power the DevKit through USB-C. Do not connect another 5 V
source to its 5V/VIN pin at the same time unless the exact module's power-path isolation
has been verified.
