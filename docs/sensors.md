# Sensor Specifications & Calibration

## Temperature Sensor (DS18B20)

### Specifications
- Sensor: Dallas DS18B20
- Protocol: 1-Wire (GPIO4)
- Resolution: 12-bit
- Accuracy: ±0.5°C (-10 to 85°C)
- Response Time: ~750ms
- Cost: ~$1-2

### Wiring
```
DS18B20
  ├─ VCC (Red) → 3.3V or 5V
  ├─ GND (Black) → GND
  └─ DATA (Yellow) → GPIO4 (with 4.7k pull-up to VCC)
```

### Calibration
DS18B20 has built-in calibration. Optional offset correction:

```cpp
sensor.setCalibrationOffset(offset_degrees_C);
```

## pH Sensor

### Specifications
- Electrode: Glass electrode (analog output)
- Amplifier Module: PH meter analog module
- ADC: ADS1115 (16-bit, I2C)
- Sensitivity: ~59mV per pH unit (at 25°C)
- Accuracy: ±0.2 pH (after calibration)
- Response Time: 30-60 seconds
- Cost: $15-30

### Wiring
```
PH Electrode → Amplifier → ADS1115 A0
  ├─ VCC → 3.3V
  ├─ GND → GND
  └─ OUT → ADS1115 A0

ADS1115 → ESP32
  ├─ VCC → 3.3V
  ├─ GND → GND
  ├─ SDA → GPIO21
  └─ SCL → GPIO22
```

### Calibration (Two-Point)

Must do two-point calibration before use:

1. **Calibration Point 1 (pH 7.0 or 6.86)**
   - Place electrode in buffer solution pH 7.0
   - Record voltage reading
   - Call: `sensor.setCalibrationPoint1(voltage, 7.0)`

2. **Calibration Point 2 (pH 4.0 or 10.0)**
   - Place electrode in second buffer solution
   - Record voltage reading
   - Call: `sensor.setCalibrationPoint2(voltage, 4.0)` or `(voltage, 10.0)`

### Maintenance
- Storage: Keep electrode in storage solution
- Cleaning: Rinse with distilled water before use
- Replacement: Electrode typically lasts 1-2 years

## Dissolved Oxygen Sensor

### Specifications
- Type: Optical DO probe or analog DO sensor
- ADC: ADS1115 (16-bit, I2C)
- Range: 0-20 mg/L (or 0-100%)
- Accuracy: ±0.3 mg/L (after calibration)
- Response Time: 10-30 seconds
- Cost: $40-100 (most expensive sensor)

### Wiring
```
DO Probe → Amplifier → ADS1115 A1
  ├─ VCC → 3.3V or 5V
  ├─ GND → GND
  └─ OUT → ADS1115 A1
```

### Calibration (Two-Point Recommended)

1. **Zero Calibration (0% DO)**
   - Use nitrogen gas or boiled water
   - Record voltage
   - Call: `sensor.setCalibrationPoints(voltage, 0.0, ...)`

2. **Span Calibration (100% DO)**
   - Expose probe to air-saturated water
   - Record voltage at 100% saturation
   - Note: 100% varies by temperature and altitude
   - At sea level, 25°C: ~8.6 mg/L

### Temperature Compensation
```cpp
sensor.setWaterTemperature(25.5);  // For accurate DO calculation
```

## Turbidity & CO2 Sensors

Both are analog sensors read through the shared ADS1115 (address `0x48`):

```
Turbidity OUT → ADS1115 A2   (TURB_CHANNEL)
CO2 OUT       → ADS1115 A3   (CO2_CHANNEL)
```

Any sensor output above 3.3 V must be divided down before reaching the
ADS1115 input (see `docs/wiring.md`). The firmware currently applies a linear
placeholder scaling over 0–3.3 V; calibrate before relying on the values.

## Water Level Float Switch (Safety Interlock)

### Specifications
- Type: Dry-contact float switch
- Input: `WATER_LEVEL_PIN` = GPIO33, `INPUT_PULLUP`
- Active level: `WATER_LEVEL_LOW_STATE` (default `LOW` = switch closed to GND = water low)

### Wiring
```
GPIO33 ──[1k series]──┬── Float switch ── GND
                      └── 100nF ── GND  (RC debounce / ESD)
```

### Behaviour
- Low water forces the pump (GPIO13) OFF immediately.
- Every pump ON request (MQTT/Home Assistant, AUTO rules, relay test) is rejected while low water is active.
- On boot the pump starts locked; the lock is released only after the switch reports normal level for `WATER_LEVEL_CLEAR_DELAY` ms.
- State is published on `smartfarm/aquaculture/sensor/water_level_low` (`ON` = low water).

## Sensor Selection Strategy

### Phase 1 (V0.1) - Essential Only
✓ Temperature (DS18B20)
✓ pH (via ADS1115)
✓ Dissolved Oxygen (via ADS1115)
✓ Water Level float switch (GPIO33 interlock)

### Phase 2 - Enhanced Monitoring
- ORP (Oxidation potential)
- Conductivity/Salinity

### Phase 3 - Advanced Analysis
- Ammonia/Nitrite/Nitrate
- Turbidity
- Pressure/Depth

## Recommended Suppliers

- **Sensors**: AliExpress, DFRobot, Adafruit
- **Modules**: Taobao, Amazon
- **Electrodes**: YSI, Hach (expensive but accurate)

## Cost Estimate (Phase 1)

| Component | Cost | Notes |
|-----------|------|-------|
| DS18B20 | $2 | Temperature |
| PH Probe + Module | $25 | Full kit |
| DO Probe + Module | $80 | Most expensive |
| Water Level | $15 | Ultrasonic |
| ADS1115 (2x) | $5 | ADC modules |
| **Total** | **~$127** | USD |

Note: Prices vary significantly by region and supplier quality.
