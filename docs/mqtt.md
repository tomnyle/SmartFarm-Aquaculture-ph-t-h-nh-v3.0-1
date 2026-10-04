# MQTT Protocol

The controller connects to the broker configured in `include/app_config.h`.
It publishes under `smartfarm/aquaculture/`; state and discovery messages are
retained. Home Assistant discovery uses the configured
`homeassistant` prefix.

## Sensor state

| Topic | Payload |
| --- | --- |
| `sensor/water_temp` | Numeric °C |
| `sensor/ph` | Numeric pH estimate |
| `sensor/do` | Numeric mg/L estimate |
| `sensor/co2` | Numeric ppm estimate |
| `sensor/turbidity` | Numeric raw ADC reading |
| `sensor/air_temp` | Numeric °C |
| `sensor/humidity` | Numeric percent |
| `sensor/light` | Numeric lux |

The firmware publishes these as retained numeric strings every 10 seconds.
Analog values are simple firmware conversions, not calibrated sensor
measurements; validate and calibrate the complete sensor chain before relying
on them.

## Commands

Publish to the following topics:

| Command topic | Accepted values |
| --- | --- |
| `control/pump/set` | `ON`, `OFF` |
| `control/aerator/set` | `ON`, `OFF` |
| `control/circulation/set` | `ON`, `OFF` |
| `control/feeder/set` | `ON`, `OFF` |
| `config/mode/set` | `AUTO`, `MANUAL`, `SCHEDULE`, `SAFE` |
| `config/species/set` | A species option published by Home Assistant discovery |
| `config/operation_profile/set` | `SENSOR_TEST`, `NO_LIVESTOCK_TEST`, `PRODUCTION` |
| `config/livestock_present/set` | `ON`, `OFF`, `TRUE`, `FALSE`, `1`, `0` |
| `config/relay_test/set` | `NOT_STARTED`, `IN_PROGRESS`, `PASSED`, `FAILED` |

An output command changes the mode to `MANUAL`. `SENSOR_TEST_MODE=true` locks
outputs and suppresses ON requests. Do not use MQTT to mark a relay test as
`PASSED` unless it has actually been performed and recorded; the firmware
accepts this state as a command.

## State and eligibility topics

| Topic | Meaning |
| --- | --- |
| `output/pump`, `output/aerator`, `output/circulation`, `output/feeder` | Retained `ON` / `OFF` output states |
| `config/mode/state`, `config/species/state` | Retained selected configuration |
| `config/operation_profile/selected` | Requested profile |
| `config/operation_profile/actual` | Active profile after eligibility checks |
| `config/livestock_present/state` | Retained `ON` / `OFF` confirmation |
| `config/relay_test/requested`, `config/relay_test/state` | Requested and current relay test status |
| `eligibility/can_no_load_test`, `eligibility/can_production` | Retained `ON` / `OFF` eligibility |
| `eligibility/outputs_locked` | Retained `ON` / `OFF` output lock |
| `eligibility/required_sensors_valid`, `eligibility/sensor_fault_active` | Sensor readiness indicators |
| `eligibility/any_active_alarm`, `eligibility/critical_condition_active` | Alarm indicators |
| `eligibility/block_reason_codes`, `eligibility/block_reason_summary` | Current profile blockers |
| `eligibility/production_block_reason`, `eligibility/active_reminder` | Production blocker and reminder text |
| `eligibility/blockers/<reason>` | Retained `ON` / `OFF` indicator for each blocker |

The controller status topic is `smartfarm/aquaculture/status` and publishes
`online` after connecting. MQTT availability is not a substitute for local
alarms or safe electrical design.
