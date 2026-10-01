# Rev A1 review, bring-up and manufacturing gates

No gate below is complete merely because it appears in this document.
**Not certified, not production-ready, not approved for fabrication or
deployment.** Record reviewer, dated results and revisions for each stage.

## Before ordering prototype boards

- [ ] Electrical engineer creates and reviews actual PCB-A/PCB-B KiCad
  schematics against [net and pin tables](universal_controller_netlist.md),
  including startup, fault, load-current and connector-cable return paths.
- [ ] Check every final IC datasheet: ESP32-WROOM-32E antenna/boot/EN
  requirements, ADS1115 input/common-mode/absolute maximum ratings, USB
  bridge and 3V3 RS485 pinout, regulator power/thermal/reference layout,
  reverse protection, TVS and clamp injection current, MOSFET Rds(on) at
  3V3 Vgs and safe operating area.
- [ ] Verify real symbol pin numbers, footprint pads/thermal pad, mating
  connector pin order/polarity, fuse ratings and diode orientations against
  datasheets and purchased parts. Populate/DNP options explicitly.
- [ ] Run schematic ERC and resolve/document every warning; assign and
  inspect footprints. Place/route both boards with antenna keepout,
  current-rated copper and cable/connector constraints; run PCB DRC
  against the chosen board manufacturer's stack-up. Review plotted
  fabrication and assembly outputs, BOM and Gerbers **only after**
  source/layout validation; obtain independent sign-off.
- [ ] Confirm no 230 VAC footprint, wire or net on either board; AC
  switching equipment, if required, is external and properly rated.

## Current-limited prototype tests (no livestock or live AC)

- [ ] Inspect assembly orientation, shorts, connector keying, grounding
  and isolation with power off. Leave all DC loads disconnected and
  start from a current-limited 12 V bench supply; check input protection,
  F1/F2 and reverse-polarity behavior safely with appropriate fixtures.
- [ ] Measure +12V_PROT, +12V_LOAD, +5V and +3V3 at test points and
  endpoints for ripple, regulation, startup transients and no-load current.
  Check that USB VBUS cannot backfeed +5V or load supply.
- [ ] Confirm EN/BOOT pull-ups, GPIO2 LED strapping and manual USB/UART
  flashing; boot repeatedly with outputs off. Use only a verified firmware
  build mapped to this board, not the current DevKit build.
- [ ] With resistive dummy loads first, exercise each OUT1–OUT8 separately
  and together; verify gate default-off and switching polarity, fusing,
  MOSFET/connector temperatures and wiring at startup/inrush, rated load
  and fault conditions. Then validate flyback diode placement, clamp
  behavior and release times with the intended DC coils.
- [ ] Check analog A0–A3 with known safe voltages within verified input
  range, per-channel scaling/calibration, noise and cable faults; verify
  0–5 V sensors are conditioned **before** connection. Test DS18B20,
  DHT22 and I2C unplug/short/fault behavior.
- [ ] Test RS485 RX/TX/DE-RE, receive-at-boot default, polarity, bus-end
  termination, cable protection and communication under noisy loads.
- [ ] Test each sensor disconnect, out-of-range reading, power loss,
  ESP32 reset, stuck output command and external relay/contactor coil
  failure; verify safe behavior and firmware interlocks.
- [ ] Complete **24–72 hours of monitored no-livestock operation** with
  representative loads and thermal logging. Review results with qualified
  personnel before any **supervised deployment**. Neither endurance nor
  successful ERC/DRC substitutes for applicable safety/legal certification.
