# SmartFarm Aquaculture Controller V0.3

ESP32-based aquaculture controller for family pond management with MQTT & Home Assistant integration.

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
  - Turbidity (via ADS1115)
  - CO2 (via ADS1115)
  - Water level float switch (pump safety interlock)

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
- Water Level Float Switch
- 8 relay channels (active-HIGH drivers, see `docs/wiring.md`)
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

## Hardware Pin Map (Rev.A)

`include/pins.h` is the single source of truth for pin mapping; the PCB must use the
same map. Full schematic guidance, net list and BOM: [`docs/wiring.md`](docs/wiring.md).

| Function | Pin | Function | Pin |
|----------|-----|----------|-----|
| ADS1115 (I2C `0x48`) A0 | pH | Pump relay | GPIO13 |
| ADS1115 A1 | DO | Aerator relay | GPIO25 |
| ADS1115 A2 | Turbidity | Circulation relay | GPIO14 |
| ADS1115 A3 | CO2 | Feeder relay | GPIO27 |
| I2C SDA / SCL | GPIO21 / GPIO22 | Valve relay | GPIO26 |
| DS18B20 (1-Wire) | GPIO4 | Light relay | GPIO32 |
| DHT22 | GPIO15 | Spare 1 relay | GPIO12 (must be LOW at boot) |
| Float switch | GPIO33 | Spare 2 relay | GPIO16 |
| Status LED | GPIO2 | | |

### Water Level Safety Interlock

The float switch on GPIO33 (`INPUT_PULLUP`, `LOW` = water low by default) forces the pump
OFF immediately. While low water is active every pump ON request — MQTT/Home Assistant,
AUTO rules or relay test — is rejected, and the pump stays locked at boot until the switch
reports normal level for `WATER_LEVEL_CLEAR_DELAY`. State is published on
`smartfarm/aquaculture/sensor/water_level_low`.

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
- `smartfarm/aquaculture/output/{pump,aerator,circulation,feeder,valve,light,spare1,spare2}` - Output states
- `smartfarm/aquaculture/control/{pump,aerator,circulation,feeder,valve,light,spare1,spare2}/set` - Output commands
- `smartfarm/aquaculture/sensor/water_level_low` - Float switch low-water state
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
- all 8 output switches (pump, aerator, circulation, feeder, valve, light, spare 1, spare 2)
- water level low binary sensor

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
- `wiring.md` - Hardware pin map Rev.A, schematic blocks, net list & BOM for PCB design
- `mqtt.md` - MQTT protocol details

## License

MIT License - See LICENSE file

## Author

Tom Nyle (tomnyle) - 2026
