# Universal Smart Farm Controller — KiCad 8 reference starter

**This is a reference/template, not a finished schematic, verified PCB,
production-ready design, or validated Gerber output.** The only KiCad file
shipped here is a blank KiCad 8 project (`.kicad_pro`). No `.kicad_sch`,
`.kicad_pcb`, Gerbers, or drill files are supplied. The CSVs are build
references, **not** KiCad-native importable netlists. They cannot be opened as
a circuit or automatically converted into a wired schematic.

## Open and build it

1. Install **KiCad 8** with the standard symbol and footprint libraries. Clone
   or download and extract the entire repository.
2. In KiCad Project Manager choose **File → Open Project** and select
   `universal_smart_farm_controller.kicad_pro` in this directory. If prompted
   on first use, choose the default global symbol and footprint library tables.
   The project has no schematic yet: use Project Manager's Schematic Editor
   button to create/save a new blank schematic with the **same basename**,
   `universal_smart_farm_controller.kicad_sch`, here. Do not mistake the empty
   project for an already wired circuit.
3. Build PCB-A in that schematic using `sheet_plan.md`,
   `netlist_table.csv`, `pin_map.csv`, and `bom_starter.csv`. Place symbols with
   **A**, wire with **W**, add hierarchical sheets/pins and net labels with
   **L**, and assign references as in the CSVs. The table `terminals` column
   uses *logical pin names*, not guaranteed physical pin numbers. A row means
   all its semicolon-separated terminals share that net on the stated board.
4. For PCB-B, use **File → New Project** in KiCad to create a separate
   `power_output` project; build its schematic from the PCB-B rows of the
   same tables. Keep board outlines and manufacturing outputs separate.
   Interboard `JA`/`JB` pin numbers must match **1:1** via a keyed cable.
5. Use **Preferences → Manage Symbol Libraries** and **Manage Footprint
   Libraries** to check installed libraries. Prefer standard symbols for
   ESP32-WROOM-32E, ADS1115, RS485 transceiver, passives, switches and
   connectors; add manufacturer symbols only after checking their pin numbers
   against datasheets. Never assume a generic regulator or protection-stage
   placeholder is a complete circuit. Select actual buck, LDO, reverse
   protection, fuses, TVS and MOSFET parts and add all manufacturer-required
   surrounding circuitry. In **Tools → Assign Footprints**, verify package,
   pin-1 orientation, pinout, connector pitch/current rating and actual
   ESP32 module variant against ordering part numbers.
6. In each schematic use **Inspect → Electrical Rules Checker (ERC)**.
   Resolve errors, confirm intended no-connects and power flags; check
   connections against the CSVs manually. Then open the PCB Editor, use
   **Tools → Update PCB from Schematic**, place/route and run **Inspect →
   Design Rules Checker (DRC)**. Check clearances, fuse and trace ampacity,
   isolation, thermal rise, mounting and interboard orientation. Inspect
   generated plots/drill files with Gerber Viewer *only after* independently
   reviewing the actual finished layouts. None are validated here.

## Architecture and signals

**PCB-A Controller:** `JA` receives protected 12V and common GND from PCB-B;
`U4` bucks to +5V and `U5` regulates +3V3. `U1` is ESP32-WROOM-32E; `SW1`
pulls EN low for reset and `SW2` pulls GPIO0 low for boot. `D2` is the
GPIO2 status LED. `J4` is a **header for an external USB-to-3V3-UART
adapter**, not an onboard USB port: cross the adapter's TX/RX to J4 RX/TX,
connect ground, and treat J4.2 as a **3V3 reference output, not a power
input**. Power the boards from protected 12V; do not inject the adapter's
5V or simultaneously drive the 3V3 rail. UART0 is GPIO1 TX / GPIO3 RX;
use the manual BOOT/RESET buttons for flashing. `J2` is DS18B20,
`J3` DHT22, `U2` ADS1115 at I2C address 0x48, `J9` 3V3 I2C expansion
and `U3` a 3V3 RS485 transceiver with `J19` bus connector and optional
`JP1`/`R16` termination. Verify bus polarity, biasing and surge protection
for the actual installation. The I2C pull-ups are shared; don't duplicate
them on an expansion module without calculating total resistance.

**PCB-B Power & Output:** `J10` is **12V DC** input through main fuse `F0`,
reverse-polarity stage `Q0` and TVS `D1`. `F9` feeds the protected 12V
load rail; `F1`–`F8` individually feed `OUT1`–`OUT8`. Each channel
has a 100-ohm series gate resistor, 100k gate pulldown and low-side
N-MOSFET; the two-way output connector supplies fused +12V and the
switched return (not ground). `D11`–`D18` are flyback diode *provisions*
from switched return (anode) to that channel's fused +12V (cathode).
Use them where appropriate for DC inductive loads; check load suppression
requirements separately for electronic loads. `JA`/`JB` carry 12V on
pin 1, GND on pins 2/11/12 and `OUT1_CTRL`–`OUT8_CTRL` on pins 3–10.
An external **12V DC relay coil or rated relay module** can be driven from
an appropriately rated output channel; any contactor and its mains wiring
must be in separate certified hardware/enclosure, with isolation and
protection designed by a qualified engineer. **Do not place 230 VAC (or
any mains voltage) on either PCB.**

| Universal input | Connector | ADS1115 input | Conditioning |
| --- | --- | --- | --- |
| A0 | J5.2 | U2.A0 | R8/R12/C6 |
| A1 | J6.2 | U2.A1 | R9/R13/C7 |
| A2 | J7.2 | U2.A2 | R10/R14/C8 |
| A3 | J8.2 | U2.A3 | R11/R15/C9 |

Each nominal 0–5V sensor input uses 10k series/top and 15k bottom to
ground, giving 0–3V at the 3V3-powered ADS1115, plus 100nF to ground.
**Do not connect 5V directly to U2 or any ESP32 GPIO.** Divider values
are illustrative only: validate sensor source impedance, ADC settling,
accuracy/calibration, overvoltage and negative transients, ESD/clamping,
power-off backfeed and VDD limits before attaching a real sensor. Sensor
5V supply and total ESP32 Wi-Fi peak current must fit selected regulators.
The analog channel names here are universally **A0–A3**, not sensor-specific.

GPIO assignment: GPIO4 DS18B20; GPIO18 DHT22; GPIO21 SDA; GPIO22 SCL;
GPIO16 RX, GPIO17 TX and GPIO23 DE/RE for RS485; GPIO13/19/14/27/25/26/32/33
for **OUT1–OUT8** respectively; GPIO2 status LED; GPIO0 BOOT; EN RESET.
Strapping pins GPIO0 and GPIO2 require review at power-up (including the
LED circuit). Keep other strapping pins free from output loads. Place U1
at the PCB edge and follow the exact ESP32 module antenna keepout in its
datasheet: no copper, traces, ground plane, vias, components or metal
enclosure beneath/in front of the antenna keepout on either layer.

No output current, fuse rating, MOSFET, copper width, heat sinking or thermal
limit has been qualified. Establish continuous/stall/inrush current per
channel and in aggregate; calculate MOSFET dissipation at **3V3 gate drive**,
diode surge energy, fuse coordination, connector and cable ratings, board
temperature and failure behavior before powering any load. Boot and reset
must leave outputs safely off via the gate pulldowns. No ratings or
manufacturer approvals are implied by this template.

## Firmware alignment (not implemented)

The current firmware in `include/pins.h` defines DS18B20 GPIO4, I2C
GPIO21/22, status GPIO2, pump GPIO13, circulation GPIO14 and feeder
GPIO27, matching the corresponding template signals. **It defines DHT22
GPIO15**, while this template uses GPIO18. **It defines aerator GPIO12**,
while this template uses OUT2 GPIO19 (GPIO12 is a boot-strapping pin and
should not drive a load here). Firmware must update `DHTPIN` and
`AERATOR_PIN` before using this hardware; map PUMP_PIN to OUT1,
CIRCULATION_PIN to OUT3 and FEEDER_PIN to OUT4 only if that deployment
wants those roles. The firmware currently initializes **four** outputs,
not eight; OUT5–OUT8 and RS485 require additional firmware configuration.
`include/pins.h` also lists direct ESP32 analog GPIO34/35/36/39 while
this starter routes A0–A3 via a **single** ADS1115 U2. Select the intended
ADC path and channel addressing in firmware rather than assuming the old
direct-ADC or optional second-ADS1115 configuration matches this template.
Firmware is intentionally unchanged.
