# Universal Smart Farm Controller — Rev A1 design package

**Engineering reference for production preparation, not a verified schematic or PCB.**
There are no checked KiCad schematic/PCB sources or Gerber manufacturing files in this
repository. Do not manufacture or deploy from these tables alone. A qualified electrical
review, final component datasheets and footprint checks, ERC/DRC, prototype bring-up and
load testing are required before any fabrication decision.

## Architecture

The target is two separate low-voltage DC boards within an approximately 120 × 100 mm
combined enclosure footprint (final mechanical dimensions are TBD):

- **PCB-A Controller:** J1 12 V input, fuse/TVS/reverse-polarity protection, 12 V to
  5 V buck and 5 V to 3V3 regulation; ESP32-WROOM-32E, 3V3 USB/UART (USB for
  programming, not for powering the load rail), BOOT/RESET, resistor-limited status
  LED, DS18B20 and DHT22 connectors, I2C expansion, ADS1115 with A0–A3
  conditioned analog inputs, and protected 3V3 RS485.
- **PCB-B Power & Output:** protected/fused +12V_LOAD rail from PCB-A, eight
  low-side DC MOSFET channels with individual series gate resistors and gate
  pulldowns, provision for flyback diodes across inductive DC loads, and eight
  DC-only load connectors. An output may energize an **external** relay/contactor
  coil rated for this low-voltage DC supply; any mains wiring and switching must
  remain outside both PCBs in appropriately rated equipment.

J18 carries protected 12 V and return to PCB-B; J19 carries eight gate-control
signals and return. Size and key the inter-board cables/connectors for the
aggregate load and ensure a common return. Hardware labels stay `A0`–`A3` and
`OUT1`–`OUT8`; separate Aquaculture, Garden and Livestock firmware builds assign
their own meanings to those labels. The existing firmware is an Aquaculture
DevKit/relay implementation, **not** a validated build for these boards; see
[profile maps and migration notes](profile_io_maps.md).

| Document | Purpose |
|---|---|
| [Net and pin tables](universal_controller_netlist.md) | KiCad-oriented connectivity proposal, not an exported netlist |
| [Rev A1 BOM](bom_rev_a1.csv) | Candidate parts/footprints; verify before schematic capture |
| [PCB layout rules](pcb_layout_rules.md) | Placement/routing starting points and design limits |
| [Profile I/O maps](profile_io_maps.md) | Firmware-specific usage and existing firmware conflicts |
| [Bring-up checklist](bringup_and_manufacturing_checklist.md) | Human verification and manufacturing gates |

No 230 VAC is routed, placed or connected to either PCB. Do not connect
unconditioned 5 V analog signals to the ADS1115 on 3V3; select and validate
external conditioning and input protection for each sensor. MOSFET current and
thermal limits are assumptions pending datasheet, copper and load validation.
