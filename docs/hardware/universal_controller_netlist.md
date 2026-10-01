# Rev A1 connectivity proposal (KiCad-oriented)

This is a **symbolic pin/net table**, not a KiCad-generated netlist or verified
schematic. Pin **names** are used rather than fabricated package pin numbers:
resolve exact symbol/footprint pin numbers, orientation and passive values
against final datasheets in KiCad. All GND names below denote a common
low-voltage return; do not connect mains to either board.

## PCB-A power and programming

| Ref / pin | Net | Connection / requirement |
|---|---|---|
| J1.1, J1.2 | +12V_IN, GND | DC input, keyed/polarized |
| F1 input/output | +12V_IN, +12V_FUSED | Input fuse, sized for entire downstream load and wiring |
| D1 | +12V_FUSED, GND | Unidirectional input TVS, cathode to positive rail; select for supply/transients |
| Q1 input/output | +12V_FUSED, +12V_PROT | Reverse-polarity protection stage; choose controller/topology and verify orientation, dissipation and fault behavior |
| C1, C2 | +12V_PROT, GND | Bulk and local bypass, ratings per final power design |
| U1 VIN/GND/OUT/EN | +12V_PROT/GND/+5V/EN_BUCK | 12-to-5 V buck *function*; bias EN_BUCK from +12V_PROT only through datasheet-approved circuitry (or use an always-on module); inductor, input/output capacitors and layout per its datasheet |
| U2 IN/GND/OUT/EN | +5V/GND/+3V3/EN_3V3 | 3V3 regulator *function*; bias EN_3V3 from +5V only if permitted by the selected regulator (or use a device without EN); power/thermal budget includes Wi-Fi peaks and peripherals |
| JUSB VBUS/GND/D+/D- | VBUS_USB/GND/USB_D+/USB_D- | USB connector for programming; ESD/protection and USB data routing per selected bridge; **do not join VBUS_USB to +5V** without designed power mux/backfeed protection |
| U6 USB data/VIO/GND/TX/RX | USB_D±/+3V3/GND/UART_RX/UART_TX | USB-to-3V3 UART bridge; cross TX/RX, use manual BOOT/RESET flashing unless validated auto-program circuit is added |
| TP1–TP4 | +12V_PROT, +5V, +3V3, GND | Power test pads |
| J18A.1/.2 | +12V_PROT, GND | Rated inter-board power connector to J18B; **not** a USB supply |

Use local decouplers for each IC; determine values and ratings from final
datasheets. Size F1, Q1, J18 and input wiring for the sum of logic and
allowed loads. PCB-B adds its own fuse; that fuse does not replace F1.

## PCB-A controller and sensors

| Ref / pin | Net | Connection / requirement |
|---|---|---|
| U3 ESP32-WROOM-32E 3V3/GND | +3V3/GND | Local decoupling; module antenna keepout |
| U3 EN, GPIO0 | EN_NET, BOOT_NET | R1 EN pull-up and R2 BOOT pull-up to +3V3; SW1 EN-to-GND reset, SW2 GPIO0-to-GND boot |
| U3 GPIO2 | STATUS_LED_NET | R3 in series to LED1 anode; LED1 cathode to GND; check boot strapping at reset |
| U3 GPIO4, GPIO18 | DS18B20_DATA, DHT22_DATA | R4/R5 pull-ups to +3V3 respectively; optional appropriately rated ESD near connectors; **not GPIO15** |
| U3 GPIO21, GPIO22 | I2C_SDA, I2C_SCL | R6/R7 pull-ups to +3V3; budget bus capacitance and expansion pull-ups |
| U3 GPIO16, GPIO17, GPIO23 | RS485_RX, RS485_TX, RS485_DE_RE | UART RX from U5.RO; TX to U5.DI (optional R12 series); GPIO23 to U5.DE and U5./RE; DE/RE pulldown R13 defaults receive at boot |
| U3 U0TXD, U0RXD | UART_TX, UART_RX | To U6 RX/TX, respectively; USB flashing is manual BOOT/RESET |
| J2.1/.2/.3 | +3V3, DS18B20_DATA, GND | 3-pin sensor connector; R4 pull-up |
| J3.1/.2/.3/.4 | +3V3, DHT22_DATA, NC, GND | Four-pin DHT22 connector; R5 pull-up; verify sensor pinout |
| J4.1/.2/.3/.4 | +3V3, GND, I2C_SDA, I2C_SCL | 3V3 I2C expansion; no 5 V signals |
| U4 ADS1115 VDD/GND/SDA/SCL/ADDR | +3V3/GND/I2C_SDA/I2C_SCL/GND | ADDR to GND selects 0x48; local bypass; ALERT/RDY NC or test pad |
| J5–J8 pins 1/2/3 | +3V3, SENSOR_Ax_IN, GND | x = 0–3 respectively; supply pin is **3V3 only**, not a sensor VREF or 5 V supply |
| R_Ax series | SENSOR_Ax_IN → Ax_NET | Current limiting and optional scaling network as dictated by sensor output |
| C_Ax filter | Ax_NET → GND | ADC input filtering; select with source impedance/sampling requirements |
| D_Ax_H / D_Ax_L | Ax_NET → +3V3 / GND | Optional clamp provisions (DNP pending design); ensure rail injection, powered-off and fault-current limits |
| U4 A0/A1/A2/A3 | A0_NET/A1_NET/A2_NET/A3_NET | Single-ended analog channels; voltage must remain within final ADS1115 input limits relative to GND/VDD |

An external 0–5 V or industrial sensor **requires** a verified divider/buffer,
common-mode and fault protection design before connecting J5–J8. Clamp diodes
alone do not make a 5 V input safe. Sensor electrode signals must use their
appropriate amplifier/interface, never wire a raw electrode directly to A0–A3.

## PCB-A RS485

| Ref / pin | Net | Connection / requirement |
|---|---|---|
| U5 MAX3485-compatible VCC/GND/DI/RO/DE,/RE | +3V3/GND/RS485_TX_SER/RS485_RX/RS485_DE_RE | Select a genuine 3V3-rated compatible transceiver, verify pinout and fail-safe behavior |
| R12 | RS485_TX → RS485_TX_SER | Optional TX series resistor (DNP if unnecessary) |
| U5 A/B | RS485_A/RS485_B | Short protected differential pair to J9 |
| D2 | RS485_A/RS485_B/GND | Suitable RS485 TVS array at connector; verify topology and common-mode range |
| JP1 + R8 | RS485_A ↔ RS485_B | Selectable 120 Ω termination only at a bus end; DNP/open for intermediate nodes |
| J9.1/.2/.3/.4 | GND, RS485_A, RS485_B, SHIELD_NC | SHIELD_NC unconnected until a reviewed chassis/shield strategy exists; no 3V3 exported |

## Inter-board and PCB-B outputs

| Ref / pin | Net | Connection / requirement |
|---|---|---|
| J18B.1/.2 | +12V_PROT, GND | Mates J18A; connector/cable rated for aggregate current |
| F2 input/output | +12V_PROT, +12V_LOAD | PCB-B replaceable fuse/polyfuse chosen for load, traces and supply fault behavior; protect each branch if required by actual wiring |
| C3 | +12V_LOAD, GND | Load-rail bulk capacitor; check surge and ripple rating |
| J19A/B.1–.8 | OUT1_CTRL–OUT8_CTRL | Keyed inter-board control harness, same pin order on both boards |
| J19A/B.9/.10 | GND, GND | Common low-voltage return, not load-current return through signal cable |

| Channel | ESP32 pin → J19.n → R_GATE → MOSFET gate | MOSFET drain → connector pin 2 |
|---|---|---|
| OUT1 | GPIO13 → 1 → R_G1 → Q2.G | Q2.D → J10.2 (`OUT1_SW`) |
| OUT2 | GPIO19 → 2 → R_G2 → Q3.G | Q3.D → J11.2 (`OUT2_SW`) |
| OUT3 | GPIO14 → 3 → R_G3 → Q4.G | Q4.D → J12.2 (`OUT3_SW`) |
| OUT4 | GPIO27 → 4 → R_G4 → Q5.G | Q5.D → J13.2 (`OUT4_SW`) |
| OUT5 | GPIO25 → 5 → R_G5 → Q6.G | Q6.D → J14.2 (`OUT5_SW`) |
| OUT6 | GPIO26 → 6 → R_G6 → Q7.G | Q7.D → J15.2 (`OUT6_SW`) |
| OUT7 | GPIO32 → 7 → R_G7 → Q8.G | Q8.D → J16.2 (`OUT7_SW`) |
| OUT8 | GPIO33 → 8 → R_G8 → Q9.G | Q9.D → J17.2 (`OUT8_SW`) |

For each x = 1–8: `R_Gx` is a series gate resistor (100 Ω initial
candidate); `R_PDx` connects the **MOSFET-side** gate to GND (100 kΩ
initial candidate). `Q(1+x).S` connects to the PCB-B load return GND.
`J(9+x).1` connects to +12V_LOAD; `J(9+x).2` connects to OUTx_SW.
`D_Fx` is an inductive-load flyback diode provision with **cathode at
+12V_LOAD and anode at OUTx_SW**, installed only when appropriate for
the actual DC load and switching requirements. Clearly mark each output
connector `DC LOAD ONLY`; neither board switches or routes 230 VAC.
For a contactor, only its appropriately rated DC coil connects here;
its mains contacts and wiring remain external.
