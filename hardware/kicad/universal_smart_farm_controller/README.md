# Universal Smart Farm Controller — KiCad hardware

## Approved Rev.A.1 target

The approved hardware target is one universal **mainboard** shared by Garden,
Aquaculture, and Livestock controllers. Egg Incubator uses this mainboard with
a dedicated expansion board. Requirements for the mainboard:

| Area | Rev.A.1 requirement |
| --- | --- |
| PCB | 120 × 100 mm, 2 copper layers |
| Controller | Removable ESP32 module using a 2 × 15 socket |
| Indicators | Six system LEDs: PWR, 3V3, STATUS, WiFi, MQTT, ERROR; plus one indicator for each of OUT1–OUT8 |
| Analog inputs | ADS1115 A0–A3, general-purpose inputs |
| Interfaces | DS18B20, DHT22, I²C, RS485 |
| Outputs | Eight MOSFET-switched outputs |
| Power | 12 V DC input, converted to 5 V and then 3.3 V |
| Controls | BOOT and RESET buttons |
| Mains | 220 VAC is kept off the PCB; use external relays/contactors |

### Status of the checked-in KiCad files

The projects below are an earlier, split-board reference design, **not the
approved Rev.A.1 implementation**. They must not be ordered or treated as
compliant with the target above:

* They use a solder-down ESP32-WROOM-32E rather than a removable 2 × 15 ESP32
  module.
* They split the controller and output stages across PCB-A and PCB-B instead
  of one 120 × 100 mm mainboard.
* They do not define the complete six-system/eight-output LED indicator set.
* No PCB layout is present; board dimensions, two-layer stackup, placement,
  routing, and fabrication outputs have not been created.
* The existing schematic pin map and firmware are not reconciled for the
  ADS1115 analog channels and all eight outputs.

The approved feature list does not yet specify the exact ESP32 module and
socket footprint, connector pinouts, sensor voltage ranges, per-output
continuous/inrush current limits, protection/fuse ratings, or the mounting
hole and edge geometry. Those electrical and mechanical details must be
confirmed before completing a manufacturable layout. Keep all 220 VAC wiring
external to the PCB.

## Earlier reference schematics

This directory currently contains two separate, native **KiCad 9** schematic
projects from the earlier split-board concept:

* `pcb_a_controller/pcb_a_controller.kicad_pro` and `.kicad_sch`
* `pcb_b_power_outputs/pcb_b_power_outputs.kicad_pro` and `.kicad_sch`

Open each `.kicad_pro` in KiCad 9, then open Schematic Editor. The schematics
contain placed symbols, references, values, footprints, UUIDs, instances and
electrical connections made with named local labels. There are no PCB layouts,
Gerbers or fabrication files.

## Architecture and supply ownership

PCB-B owns the **only 12 V DC input**. Its main fuse, series SS54
reverse-polarity diode and correctly polarized SMBJ15A TVS create
`+12V_PROTECTED`. Inter-board connector J1 sends that rail, three ground pins
and eight logic controls to matching J1 on PCB-A. PCB-A generates 5 V with a
TPS5430DDA reference circuit and 3.3 V with AP2112K-3.3. There is no USB power
input: PCB-A J4 is only a 3.3 V UART programming interface and its 3.3 V pin is
a **reference output**. Do not inject adapter power or power either board from
a second supply.

The small inter-board header never carries load current. PCB-B load and
flyback currents must return directly through its input/load ground copper;
the inter-board ground pins are only the controller supply and signal
reference return. See `connector_summary.csv` for the 1:1 mating table.

## PCB-A contents

* ESP32-WROOM-32E with 10 uF/100 nF decoupling, EN RC/reset, GPIO0 BOOT
  pull-up/button and external 3.3 V UART header.
* DS18B20 3-pin header with 4.7 k pull-up. DHT22 is explicitly the bare
  **4-pin** device: pin 3 is no-connect and data has a 10 k pull-up.
* ADS1115IDGS at address 0x48, decoupling, I2C pull-ups and distinct A0-A3
  connectors. Each nominal 0–5 V signal is divided by 10 k/15 k to 0–3.0 V
  and filtered by 100 nF. These inputs have no negative-voltage, surge or ESD
  protection; they are provisional/external-conditioning-required for harsh
  or unknown sensors. Never apply 5 V directly to the ADS1115.
* 3.3 V MAX3485, default driver-disable pull-down, bus connector and
  jumper-selectable 120 ohm termination.
* Matching J1 controls: GPIO13/19/14/27/25/26/32/33 for OUT1–OUT8.
* D4/R22 indicate that the 3.3 V rail is present. D2/R7 is the existing
  GPIO2-driven STATUS indicator.
* U6 is an MCP23008 at I²C address 0x20 (A0–A2 grounded), sharing SDA/SCL with
  the ADS1115. D5/R23, D6/R24, and D7/R25 indicate WiFi connected, MQTT
  connected, and sensor-fault/critical-condition states. They are active-low
  sink outputs, initialized OFF; C16 is local 100 nF decoupling.

The TPS5430 values implement the datasheet topology (bootstrap capacitor,
catch diode, inductor, input/output capacitors and 10 k/3.24 k feedback).
Inductor saturation/current, capacitor ESR, loop behavior, EMI and regulator
thermal performance remain engineering review items.

## PCB-B contents

Each of eight channels has an individual provisional fuse, 100 ohm gate
resistor, 100 k default-off pull-down, AO3400A low-side MOSFET, two-pin
12 V/load-return connector and SS34 flyback diode. Every flyback diode has
its **anode (pin 2) at `OUTx_SW`** and **cathode (pin 1) at the individually
fused `OUTx_V+`**. Fit that suppression only for appropriate external
inductive 12 V DC loads, such as relay coils. Do not route relay contact
wiring or any 220/230 V AC/mains voltage onto either board.

D19/R17 indicate protected 12 V power. D20–D27, each with a 3.3 kΩ series
resistor, are connected across the corresponding fused output supply and
MOSFET-switched return. They indicate that a channel is switched on, not that
an external load is connected or functioning. These circuits add about 4 mA
per active channel at nominal 12 V.

The six requested system indicators are now represented across both reference
boards: 12 V PWR (PCB-B), 3V3, STATUS, WiFi, MQTT, and ERROR (PCB-A). WiFi and
MQTT LEDs show live network connections. ERROR lights for required sensor
faults or critical conditions; ordinary noncritical alerts do not light it.
Keep the I²C address and expander GPIO mapping consistent with firmware.

Output current ratings are intentionally TBD. AO3400A has an Rds(on)
specification at 2.5 V gate drive, but its SOT-23 thermal limit, connector,
diode, fuse, wiring and copper limits may dominate. Before manufacture,
measure each load's continuous, starting/stall and fault currents and size
all of those items plus the main/load-rail fuses and return paths.

## Firmware compatibility conflict

The required schematic map is not compatible with the current Rev.A firmware
in `include/pins.h`:

* firmware DHT22 is GPIO15; PCB-A uses GPIO18;
* firmware aerator is GPIO12; OUT2 uses GPIO19 to avoid GPIO12 strapping;
* firmware initializes only four named outputs, not OUT1–OUT8;
* firmware analog inputs are direct ESP32 GPIO34/35/36/39, while PCB-A uses
  one ADS1115 and A0–A3.

DS18B20 GPIO4, SDA/SCL GPIO21/22, status GPIO2, OUT1 GPIO13, OUT3 GPIO14 and
OUT4 GPIO27 do align. Firmware must be deliberately remapped and tested; it
is **not** claimed compatible by this hardware package.

## Validation status

The files were generated and round-trip parsed with `kicad-sch-api 0.5.6`.
Automated checks verify unique references, valid library identifiers, every
symbol pin accounted for by a net label or no-connect marker, the critical
GPIO-to-net map, flyback polarity and exact J1 mating order.

**Real KiCad ERC and KiCad SVG/PDF/netlist export were NOT run.** The sandbox
provided KiCad CLI 7.0.11, which correctly rejects the newer KiCad 9 file
format, and a KiCad 9 CLI could not be obtained. Consequently these sources
are reviewable reference schematics, not electrically validated or
fabrication-ready designs. Open and re-save both projects with KiCad 9,
resolve libraries, run **Inspect → Electrical Rules Checker**, and export
SVG/PDF/netlists before relying on them.

A qualified hardware engineer must review datasheet pinouts, footprints,
power and ground budget, protection, EMC, antenna keepout, analog
conditioning, RS485 field protection, fuse coordination and output
current/thermal design before PCB layout or manufacture.

## Component references used

Pin numbering and the reference circuits were checked against the standard
KiCad symbols and these manufacturer documents:

* [ESP32-WROOM-32E/32UE datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32e_esp32-wroom-32ue_datasheet_en.pdf)
* [TPS5430 datasheet](https://www.ti.com/lit/ds/symlink/tps5430.pdf)
* [ADS1115 datasheet](https://www.ti.com/lit/ds/symlink/ads1115.pdf)
* [MAX3485 family datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/MAX1487-MAX491.pdf)
* [AP2112 datasheet](https://www.diodes.com/assets/Datasheets/AP2112.pdf)
* [AO3400A datasheet](https://www.aosmd.com/sites/default/files/res/datasheets/AO3400A.pdf)

The symbol IDs embedded in the schematics are
`RF_Module:ESP32-WROOM-32E`, `Analog_ADC:ADS1115IDGS`,
`Regulator_Switching:TPS5430DDA`, `Regulator_Linear:AP2112K-3.3`,
`Interface_UART:MAX3485` and `Transistor_FET:AO3400A`. Generic diode
graphics use explicit part values; their pin 1 is the cathode and pin 2 the
anode. Re-check ordering codes and package drawings before procurement.
