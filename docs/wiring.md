# Hardware Wiring & PCB Schematic Guide — Pin Map Rev.A

This document describes the **Rev.A** controller board so it can be drawn in
KiCad / EasyEDA / Altium and routed without ambiguity.

> **Single source of truth:** the GPIO / ADC channel assignment lives only in
> [`include/pins.h`](../include/pins.h). The PCB schematic **must** use exactly
> the same map. If a pin ever changes, change `pins.h` first, then this document
> and the schematic in the same pull request.

---

## 1. Final pin map (Rev.A)

### 1.1 ESP32 (ESP32-WROOM-32 / DevKitC V4)

| Net name          | GPIO | Direction | Function                              | `pins.h` symbol   | Boot notes |
|-------------------|------|-----------|---------------------------------------|-------------------|------------|
| `I2C_SDA`         | 21   | I/O       | I2C data (ADS1115 + BH1750)           | `I2C_SDA`         | —          |
| `I2C_SCL`         | 22   | O         | I2C clock (ADS1115 + BH1750)          | `I2C_SCL`         | —          |
| `ONEWIRE`         | 4    | I/O       | DS18B20 water temperature             | `ONE_WIRE_BUS`    | —          |
| `DHT_DATA`        | 15   | I/O       | DHT22 air temperature / humidity      | `DHTPIN`          | Strapping (MTDO). Pull-up is fine (HIGH = normal boot log). |
| `WATER_LEVEL`     | 33   | I         | Float switch – pump safety interlock  | `WATER_LEVEL_PIN` | —          |
| `RLY_PUMP`        | 13   | O         | Relay 1 – main pump                   | `PUMP_PIN`        | JTAG TCK; high‑Z at reset. |
| `RLY_AERATOR`     | 25   | O         | Relay 2 – aerator                     | `AERATOR_PIN`     | —          |
| `RLY_CIRC`        | 14   | O         | Relay 3 – circulation pump            | `CIRCULATION_PIN` | JTAG TMS; may toggle briefly at reset – pull-down required. |
| `RLY_FEEDER`      | 27   | O         | Relay 4 – feeder                      | `FEEDER_PIN`      | —          |
| `RLY_VALVE`       | 26   | O         | Relay 5 – valve                       | `VALVE_PIN`       | —          |
| `RLY_LIGHT`       | 32   | O         | Relay 6 – light                       | `LIGHT_PIN`       | —          |
| `RLY_SPARE1`      | 12   | O         | Relay 7 – spare 1                     | `SPARE1_PIN`      | **Strapping (MTDI). Must be LOW at reset** or flash voltage becomes 1.8 V and the board will not boot. 10 kΩ pull-down mandatory; never use an active-LOW relay input with pull-up here. |
| `RLY_SPARE2`      | 16   | O         | Relay 8 – spare 2                     | `SPARE2_PIN`      | Use a **WROOM** module (on WROVER GPIO16 is used by PSRAM). |
| `LED_STATUS`      | 2    | O         | Status LED (ON = water level low)     | `LED_PIN`         | Strapping. LED + resistor to GND keeps it LOW – OK. |

Unused / reserved: GPIO34, 35, 36 (VP), 39 (VN) are input-only and are left free
for future expansion (e.g. second float switch or current sense). GPIO0, 1, 3, 5,
6–11 must not be used (boot / UART / flash).

### 1.2 ADS1115 (I2C address `0x48`)

| ADS1115 pin | Net           | Sensor              | `pins.h` symbol |
|-------------|---------------|---------------------|-----------------|
| A0          | `AIN_PH`      | pH amplifier output | `PH_CHANNEL 0`  |
| A1          | `AIN_DO`      | DO amplifier output | `DO_CHANNEL 1`  |
| A2          | `AIN_TURB`    | Turbidity output    | `TURB_CHANNEL 2`|
| A3          | `AIN_CO2`     | CO2 analog output   | `CO2_CHANNEL 3` |
| ADDR        | GND           | Selects `0x48`      | `ADS1115_ADDRESS` |
| VDD         | `+3V3`        |                     |                 |
| SDA / SCL   | `I2C_SDA` / `I2C_SCL` |             |                 |
| ALERT/RDY   | NC (or 10 kΩ to `+3V3`) |           |                 |

Firmware uses single-ended reads with `GAIN_ONE` (±4.096 V range) and scales
0–3.3 V (`ADS1115_FULL_SCALE_VOLTAGE`). **No ADS1115 input may exceed VDD
(3.3 V) + 0.3 V.**

---

## 2. Schematic blocks

### 2.1 Power

```
J1 (12 V DC in, 2-pin 5.08 mm terminal)
  +12V_IN ──[F1 PTC/fuse 3 A]──[D1 SS54 reverse-polarity]──┬── +12V (relay coils / DC loads)
                                                           ├── TVS1 SMBJ15A ── GND
                                                           ├── C1 470 µF/25 V ── GND
                                                           └── U1 Buck 12 V → 5 V (≥2 A, e.g. MP1584 / LM2596)
                                                                   └── +5V ──┬── C2 100 µF ── GND
                                                                             └── U2 LDO 3.3 V (≥800 mA, e.g. AMS1117-3.3)
                                                                                     └── +3V3 ── C3 22 µF + C4 100 nF ── GND
```

- ESP32 DevKit is powered from `+5V` (VIN pin) **or** the module from `+3V3` – not both.
- All GNDs (logic, relay, sensor) are common, joined at one star point near U1.
- Heavy pump/motor currents never flow through the logic ground pour.

### 2.2 I2C bus

```
+3V3 ──[R1 4.7 kΩ]── I2C_SDA (GPIO21) ── ADS1115 SDA ── BH1750 SDA (J5)
+3V3 ──[R2 4.7 kΩ]── I2C_SCL (GPIO22) ── ADS1115 SCL ── BH1750 SCL (J5)
```

Fit only one set of pull-ups (remove/omit pull-ups on plug-in modules).

### 2.3 Analog front end (repeat for A0–A3)

```
Sensor OUT (0–5 V) ──[Rtop 10 kΩ]──┬──[Rs 1 kΩ]──┬── ADS1115 Ax
                                   │             │
                                [Rbot 18 kΩ]   [Cf 100 nF]
                                   │             │
                                  GND           GND
```

- 5 V sensors: Rtop 10 kΩ / Rbot 18 kΩ → max 3.21 V.
- 3.3 V sensors: fit 0 Ω for Rtop and DNP Rbot.
- Optional BAT54S clamp to `+3V3`/GND on each input for ESD.
- pH and DO probe amplifier boards are supplied from `+5V` (or `+3V3` if supported).

### 2.4 Digital sensors

```
DS18B20 (J3):  1=+3V3  2=ONEWIRE (GPIO4)  3=GND      R3 4.7 kΩ ONEWIRE→+3V3
DHT22   (J4):  1=+3V3  2=DHT_DATA (GPIO15) 3=GND     R4 10 kΩ DHT_DATA→+3V3, C5 100 nF at connector
BH1750  (J5):  1=+3V3  2=GND  3=I2C_SCL  4=I2C_SDA
```

### 2.5 Float switch – water level safety interlock

```
J6 pin1 ── WATER_LEVEL ──[R5 1 kΩ]──┬── GPIO33 (INPUT_PULLUP)
                                    ├── C6 100 nF ── GND
                                    └── R6 10 kΩ ── +3V3 (external pull-up, recommended)
J6 pin2 ── GND
```

- Switch closed (to GND) = `LOW` = **water low** (`WATER_LEVEL_LOW_STATE LOW`).
- Firmware forces the pump OFF and rejects every pump ON command while low water
  is active; the pump is also locked at boot until the switch reads normal for
  `WATER_LEVEL_CLEAR_DELAY` ms.
- For a hardware backstop, the float switch contact may additionally be wired in
  series with the pump contactor coil.

### 2.6 Relay drivers (8 identical channels)

Active-HIGH low-side driver so that every output is OFF while the ESP32 is in
reset (all GPIOs high-Z):

```
GPIOx ──[Rg 100 Ω]──┬── Q (AO3400 / 2N7002) gate
                    └──[Rpd 10 kΩ]── GND           (mandatory pull-down)

+12V ── K (relay coil, 12 V) ──┬── Q drain          Q source ── GND
                               └── D (1N4148/SS14) flyback across coil (cathode to +12V)

Optional indicator: +12V ──[2.2 kΩ]── LED ── Q drain
```

A ULN2803A (inputs 1–8 from the eight GPIOs, COM to +12V, plus 10 kΩ pull-downs
on each input) is an acceptable alternative.

| Relay | GPIO | Net          | Load terminal |
|-------|------|--------------|---------------|
| K1    | 13   | `RLY_PUMP`   | J7            |
| K2    | 25   | `RLY_AERATOR`| J8            |
| K3    | 14   | `RLY_CIRC`   | J9            |
| K4    | 27   | `RLY_FEEDER` | J10           |
| K5    | 26   | `RLY_VALVE`  | J11           |
| K6    | 32   | `RLY_LIGHT`  | J12           |
| K7    | 12   | `RLY_SPARE1` | J13           |
| K8    | 16   | `RLY_SPARE2` | J14           |

Load terminals J7–J14: 3-pin 5.08 mm (COM / NO / NC). For mains loads use
relays rated ≥10 A 250 VAC, ≥6 mm creepage between coil side and contact side
(slot the PCB), a fuse per load, and a contactor for motors/pumps > 0.5 kW.

### 2.7 Status LED

```
GPIO2 ──[R7 1 kΩ]── LED_STATUS (green) ── GND
```

---

## 3. Net list summary

| Net           | Connected pins |
|---------------|----------------|
| `+12V`        | D1.K, TVS1, C1, U1.VIN, K1–K8 coil(+), flyback D cathodes |
| `+5V`         | U1.VOUT, U2.VIN, ESP32 VIN, sensor amplifier VCC |
| `+3V3`        | U2.VOUT, ADS1115.VDD, R1, R2, R3, R4, R6, J3.1, J4.1, J5.1 |
| `GND`         | All grounds (star point at U1) , ADS1115.ADDR |
| `I2C_SDA`     | ESP32 GPIO21, ADS1115.SDA, J5.4, R1 |
| `I2C_SCL`     | ESP32 GPIO22, ADS1115.SCL, J5.3, R2 |
| `ONEWIRE`     | ESP32 GPIO4, J3.2, R3 |
| `DHT_DATA`    | ESP32 GPIO15, J4.2, R4 |
| `WATER_LEVEL` | ESP32 GPIO33 (via R5), J6.1, C6, R6 |
| `AIN_PH`      | ADS1115.A0, pH divider/filter |
| `AIN_DO`      | ADS1115.A1, DO divider/filter |
| `AIN_TURB`    | ADS1115.A2, turbidity divider/filter |
| `AIN_CO2`     | ADS1115.A3, CO2 divider/filter |
| `RLY_PUMP` … `RLY_SPARE2` | ESP32 GPIO13/25/14/27/26/32/12/16 → driver gates (see 2.6) |
| `LED_STATUS`  | ESP32 GPIO2, R7 |

---

## 4. Layout recommendations

1. Split the board into three zones: **power/relay** (12 V, contacts),
   **logic** (ESP32, regulators) and **analog** (ADS1115, probe connectors).
2. Keep the ADS1115 and its RC filters close together, away from relay coils and
   the buck converter; route analog traces over an unbroken ground plane.
3. Keep the ESP32 antenna area free of copper and components on all layers.
4. Mains/contact tracks: ≥3 mm wide for 10 A (2 oz copper), with a routed slot
   between contact and logic sides.
5. Place flyback diodes directly at each relay coil.
6. Put all field connectors on one board edge; label every terminal with its net
   name from this document (e.g. `PUMP`, `AERATOR`, `FLOAT`, `pH`).

---

## 5. Bill of materials (core)

| Ref       | Part                          | Qty |
|-----------|-------------------------------|-----|
| U0        | ESP32-WROOM-32E DevKitC V4    | 1   |
| U1        | Buck 12 V → 5 V ≥2 A          | 1   |
| U2        | AMS1117-3.3 (or equivalent)   | 1   |
| U3        | ADS1115 (module or TSSOP-10)  | 1   |
| K1–K8     | Relay 12 V coil, 10 A contacts| 8   |
| Q1–Q8     | AO3400 / 2N7002 (or 1× ULN2803A) | 8 |
| D2–D9     | 1N4148 / SS14 flyback         | 8   |
| F1        | Fuse / PTC 3 A                | 1   |
| D1        | SS54 Schottky                 | 1   |
| TVS1      | SMBJ15A                       | 1   |
| R1–R4, R6 | 4.7 kΩ / 10 kΩ pull-ups       | 5   |
| R5, R7    | 1 kΩ                          | 2   |
| Rpd ×8    | 10 kΩ gate pull-down          | 8   |
| Rg ×8     | 100 Ω gate resistor           | 8   |
| Analog RC | 10 kΩ, 18 kΩ, 1 kΩ, 100 nF    | 4 sets |
| J1–J14    | Screw terminals 5.08 mm / JST-XH | — |

---

## 6. Bring-up checklist

1. Power the board without the ESP32 and verify `+5V` and `+3V3`.
2. Flash firmware over USB with `SENSOR_TEST_MODE true` (outputs locked OFF).
3. Confirm the serial log shows `[OK] ADS1115 initialized` and plausible readings.
4. Reset the board repeatedly and confirm no relay clicks on at boot.
5. Close the float switch → log shows `[SAFETY] Water level LOW - pump locked OFF`,
   and `smartfarm/aquaculture/sensor/water_level_low` = `ON`.
6. With low water active, send `ON` to `smartfarm/aquaculture/control/pump/set`
   → pump must stay OFF.
7. Only then proceed to the relay test and `NO_LIVESTOCK_TEST` profile.
