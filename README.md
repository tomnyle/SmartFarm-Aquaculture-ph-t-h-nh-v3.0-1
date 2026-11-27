# SmartFarm Aquaculture Controller V0.3

ESP32-based aquaculture controller for family pond management with MQTT & Home Assistant integration.

## ESP32 Relay X8: no-load bench firmware

The `esp32relay8_bench` environment is a **separate relay-only test**, not the
aquaculture controller. It does not read sensors, run SmartFarm automation, or
implement Modbus. Do not use it with connected loads or livestock. The existing
`esp32dev` firmware is not compatible with this X8 board.

The board marked `ESP32_Relay_X8_Modbus (303E32DC812)` is believed to use a
74HC595 with active-low relay outputs: GPIO13 OE, GPIO25 latch, GPIO26 clock,
GPIO33 data. Verify the hardware with no loads connected; this mapping and the
boot/reset output behavior have not been tested on your board. No software can
guarantee the relay state during power-up, reset, or a firmware fault.

1. Disconnect every relay load. Configure Wi-Fi and MQTT in `include/app_config.h`.
2. Flash with `platformio run -e esp32relay8_bench -t upload` and open
   `platformio device monitor -b 115200`.
3. After MQTT connects, press and release the physical BOOT button to arm one
   command for 30 seconds. Toggle one of the eight X8 bench switches in Home
   Assistant. One relay at a time turns on for up to five seconds; press BOOT
   again for each test. OFF ends a pulse early. Network disconnect disarms and
   turns the relay off when detected. These are software limits, not safety
   guarantees.

For direct MQTT use, command topics are printed in the serial log and change
on each connection, preventing old retained commands from replaying. Send `ON`
or `OFF` to the printed topic. States are published to
`smartfarm/x8_bench/relay/1/state` through `relay/8/state`; availability is
`smartfarm/x8_bench/status`. Keep command messages non-retained. The eight
switches are separate from the original SmartFarm entities.

Sensor types, calibration, safe wiring, and access to their physical pins remain
unresolved. Production firmware must be adapted and tested separately before
connecting pumps, aerators, or an occupied pond.

## Features

- **Autonomous Control**: Works independently even without Home Assistant
- **Multi-Species Support**: Profiles for Koi, Catfish, Shrimp, Tilapia, and more
- **4 Operating Modes**: AUTO, MANUAL, SCHEDULE, SAFE
- **Safe Runtime Operation Profiles**: `SENSOR_TEST`, `NO_LIVESTOCK_TEST`, and `PRODUCTION`
- **Commissioning Interlocks**: Separate `can_no_load_test`, `can_production`, retained block reasons, and livestock confirmation
- **Core Sensors**:
  - Water Temperature (DS18B20)
  - pH Level (via ADS1115)
  - Dissolved Oxygen (via ADS1115)
  - Turbidity

- **Relay Control** (8-channel):
  - Aerator (Máy sục khí)
  - Water Pump (Bơm cấp nước)
  - Circulation Pump (Bơm tuần hoàn)
  - Feeder (Máy cho ăn)
  - Valve (Van)
  - Light (Đèn)
  - 2x Spare

## Hardware Requirements

- ESP32 DevKitC V4 / ESP-WROOM-32
- DS18B20 Temperature Sensor
- pH Electrode + ADS1115 ADC Module
- Dissolved Oxygen Probe + ADS1115
- Water Level Sensor
- 8-Channel Relay Module
- 5V Power Supply

## Getting Started

### 1. Clone Repository
```bash
git clone https://github.com/tomnyle/SmartFarm-Aquaculture.git
cd SmartFarm-Aquaculture
```

### 2. Configure
Edit `include/app_config.h`:
- WiFi SSID & Password
- MQTT Broker Address
- Device Name & Location
- `SENSOR_TEST_MODE` (keep `true` until commissioning is complete)

### 3. Build & Upload
```bash
platformio run -e esp32dev -t upload
```

### 4. Monitor Serial Output
```bash
platformio device monitor -b 115200
```

## System Architecture

```
         Home Assistant
              │
             MQTT
              │
      Aquaculture ESP32
              │
    ┌─────────┼─────────┐
    │         │         │
Sensors  Rule Engine  Outputs
    │         │         │
    └─────────┼─────────┘
         Local Controller
```

## Operating Modes

### AUTO Mode
ESP32 automatically controls relays based on sensor readings and active profile thresholds.

### MANUAL Mode
Control relays directly from Home Assistant.

### SCHEDULE Mode
Execute predefined schedules (e.g., feeding times).

### SAFE Mode
Activated when critical errors detected:
- Sensor failures
- Water level too low
- Temperature critical
- DO critical

## Operation Profile State Machine

The firmware now keeps a second safety layer for commissioning and production selection:

- `SENSOR_TEST`
  - Sensor-only validation.
  - Outputs stay locked OFF.
  - ON commands are suppressed.
  - AUTO rules are skipped.
- `NO_LIVESTOCK_TEST`
  - Wet / no-load test with no animals.
  - Allowed only when:
    - required sensors are valid (`water_temp`, `pH`, `DO`, `turbidity`)
    - no required sensor fault is active
    - `SENSOR_TEST_MODE=false`
    - no critical condition is active
    - no external output lock is active (currently `SAFE` mode)
    - relay test status is `PASSED`
- `PRODUCTION`
  - Real livestock production.
  - Requires everything from `NO_LIVESTOCK_TEST`, plus:
    - retained `livestock_present=ON`
    - no active alarm is present

Runtime note:

- `SENSOR_TEST_MODE` is its own blocker and always forces the active profile back to `SENSOR_TEST`.
- The active `SENSOR_TEST` profile still locks outputs at runtime, but that profile lock is not reused as an eligibility blocker when evaluating whether the next requested profile can be accepted.

Requested profile changes from Home Assistant are never trusted blindly:

- Unsafe `NO_LIVESTOCK_TEST` requests are downgraded to `SENSOR_TEST`.
- Unsafe `PRODUCTION` requests are downgraded to the safest valid profile:
  - `PRODUCTION` when fully eligible
  - otherwise `NO_LIVESTOCK_TEST` when commissioning-safe
  - otherwise `SENSOR_TEST`

Stable blocker codes published by the firmware:

- `required_sensor_invalid`
- `sensor_fault_active`
- `relay_test_not_passed`
- `outputs_locked`
- `sensor_test_mode_enabled`
- `active_alarm`
- `livestock_not_confirmed`
- `critical_condition_active`

## Profiles

Each species has predefined parameter ranges:

```json
{
  "name": "shrimp",
  "temperature": { "min": 28, "max": 32 },
  "ph": { "min": 7.5, "max": 8.5 },
  "do": { "min": 5.0 }
}
```

## MQTT Topics

Core retained topics:

- `smartfarm/aquaculture/sensor/*` - Sensor readings
- `smartfarm/aquaculture/output/*` - Output states
- `smartfarm/aquaculture/config/mode/set|state`
- `smartfarm/aquaculture/config/species/set|state`
- `smartfarm/aquaculture/config/operation_profile/set|selected|actual`
- `smartfarm/aquaculture/config/livestock_present/set|state`
- `smartfarm/aquaculture/config/relay_test/set|requested|state`
- `smartfarm/aquaculture/eligibility/can_no_load_test`
- `smartfarm/aquaculture/eligibility/can_production`
- `smartfarm/aquaculture/eligibility/outputs_locked`
- `smartfarm/aquaculture/eligibility/required_sensors_valid`
- `smartfarm/aquaculture/eligibility/sensor_fault_active`
- `smartfarm/aquaculture/eligibility/any_active_alarm`
- `smartfarm/aquaculture/eligibility/critical_condition_active`
- `smartfarm/aquaculture/eligibility/block_reason_codes`
- `smartfarm/aquaculture/eligibility/block_reason_summary`
- `smartfarm/aquaculture/eligibility/production_block_reason`
- `smartfarm/aquaculture/eligibility/active_reminder`
- `smartfarm/aquaculture/eligibility/blockers/*`

Home Assistant MQTT discovery also creates entities for:

- operation profile selector
- active profile sensor
- livestock present switch
- relay test request selector and status sensor
- can no-load test
- can production
- outputs locked
- production block reason / active reminder
- per-blocker reminder indicators

Binary-sensor style eligibility and blocker topics publish retained `ON` / `OFF` payloads. Select entities publish fixed option strings such as `SENSOR_TEST`, `NO_LIVESTOCK_TEST`, `PRODUCTION`, `NOT_STARTED`, `IN_PROGRESS`, `PASSED`, and `FAILED`.

## Home Assistant Automation Example

Example persistent notification when a requested profile is rejected or the active block reason changes:

```yaml
automation:
  - alias: Aquaculture profile blocked
    trigger:
      - platform: state
        entity_id:
          - sensor.aquaculture_operation_profile_block_summary
          - sensor.aquaculture_active_profile
    condition:
      - condition: template
        value_template: >
          {{ states('sensor.aquaculture_operation_profile_block_summary') not in
             ['All profile conditions satisfied', 'unknown', 'unavailable'] }}
    action:
      - service: persistent_notification.create
        data:
          title: Aquaculture profile reminder
          message: >
            Requested={{ states('select.aquaculture_operation_profile') }}
            Actual={{ states('sensor.aquaculture_active_profile') }}
            Summary={{ states('sensor.aquaculture_operation_profile_block_summary') }}
```

## Documentation

See `/docs` folder for:
- `architecture.md` - System design
- `sensors.md` - Sensor specifications & calibration
- `wiring.md` - Hardware wiring diagram
- `mqtt.md` - MQTT protocol details

## License

MIT License - See LICENSE file

## Author

Tom Nyle (tomnyle) - 2026
