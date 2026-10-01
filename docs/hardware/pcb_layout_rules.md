# Rev A1 placement and routing rules (starting points)

These rules are design assumptions, **not** manufacturing sign-off. Plan two
physically separate boards (PCB-A controller and PCB-B DC power/output) in a
target **120 × 100 mm combined enclosure envelope**; allocate each board's
actual outline only after connector and mounting measurements. Do not assume
120 × 100 mm per board. Confirm fab stack-up and assembly limitations.

## Placement and zones

- Place the ESP32-WROOM-32E antenna at an outside board edge with the
  manufacturer's required antenna keepout on all copper layers, components
  and nearby metal/enclosure. Verify the current module integration guide,
  not a guessed clearance.
- On PCB-A group J1, F1, D1, Q1 and buck near the power entry, then isolate
  switching power from the quiet ADS1115/analog connector zone. Keep
  high-current PCB-B switches and cables away from analog and RF.
- Put J2–J9 on accessible edges; group J5–J8 with identical universal `A0`–`A3`
  silk. Put J18/J19 at a keyed inter-board cable edge; align pin 1 markings on
  both boards. On PCB-B put F2 at J18B, the MOSFETs near J10–J17, and label
  every output `OUTn DC LOAD ONLY` with polarity. Keep BOOT/RESET/USB and test
  points accessible with the enclosure open.
- Use continuous ground reference for logic and analog wherever practical,
  keeping high di/dt return loops localized on PCB-B. Route PCB-B load return
  through **J18**, not J19; join sensor, digital and power returns deliberately
  at low impedance without cutting return paths under sensitive signals.
  Verify noise and ground potential with real loads and sensor cables.
- Protect RS485 at J9; route A/B together, maintain controlled return, verify
  common-mode and external cable ESD/surge environment. Fit JP1/R8 termination
  only at the bus ends; review bias/fail-safe and shielding before deployment.

## Copper and limits

Start signal traces at ~0.20–0.25 mm and low-current rails at ~0.5 mm;
calculate **all** widths/clearances, vias and copper area for actual layer
weight, temperature rise, peak/continuous current, fab capabilities and
appropriate IPC guidance. These figures do **not** authorize any load
current. Size J1/F1/Q1/J18/F2, board planes, return path and each J10–J17
connector for their real loads and fault conditions. Branch protection may be
needed. PCB-B channel design target is **up to 1 A continuous per channel**
and **up to 4 A aggregate** as an *unverified planning assumption*, not a
rating; the lower of connector, cable, fuse, MOSFET SOA/Rds(on) at 3V3 gate
drive, flyback path, copper and thermal capacity governs. Reduce limits if
testing or component selection demands it. Check startup/inrush and locked
load faults; prototype and measure MOSFET, diode, connector, trace and fuse
temperatures at worst-case ambient.

Use flyback only for appropriate inductive **DC** loads, with the diode
cathode at +12V_LOAD; confirm release-time requirements for contactor
coils. Scale/buffer any analog sensor output exceeding ADS1115's limits on
3V3, including 0–5 V probes; clamp leakage, rail injection, powered-off
behavior and ADC error require engineering verification. USB VBUS must not
backfeed board rails or supply outputs. Finish component datasheet, footprint
and polarity checks, schematic ERC and PCB DRC before ordering a prototype.

**No 230 VAC or other mains-voltage trace, connector, relay contact or
clearance design is included on either PCB.** Only low-voltage DC loads or
external DC relay/contactor **coils** can attach to outputs; arrange any AC
power switching separately in certified/rated equipment under qualified
supervision.
