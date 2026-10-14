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
- ADC: ADS1115 (16-bit, I2C, address 0x48, channel A0)
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

ADS1115 → ESP32 DevKit
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
- ADC: ADS1115 (16-bit, I2C, address 0x48, channel A1)
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

## Water Level Sensor

### Specifications
- Type: Ultrasonic or capacitive water level sensor
- Signal: 0-5V analog (future firmware input; not connected in current mapping)
- Accuracy: ±2-5% of range
- Response Time: <100ms
- Cost: $10-20

### Wiring
```
Water Level Sensor
  ├─ VCC → 5V
  ├─ GND → GND
  └─ OUT → additional ADS1115 A0 after voltage conditioning
```

### Calibration

1. **Measure empty level (0%)**
   ```cpp
   sensor.setMinMaxLevel(0, 100);  // in cm
   ```

2. **Measure full level (100%)**
   ```cpp
   sensor.setCalibrationPoints(adc_empty, adc_full, 0, 100);
   ```

## Sensor Selection Strategy

### Phase 1 (V0.1) - Essential Only
✓ Temperature (DS18B20)
✓ pH (via ADS1115)
✓ Dissolved Oxygen (via ADS1115)
✓ Water Level (planned; not currently read by firmware)

### Phase 2 - Enhanced Monitoring
- ORP (Oxidation potential)
- Conductivity/Salinity

### Phase 3 - Advanced Analysis
- Ammonia/Nitrite/Nitrate
- Turbidity (ADS1115 A2; firmware input)
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

Note: Prices vary significantly by region and supplier quality. The estimate lists two
ADS1115 modules, but the current four analog firmware inputs share one module.

## Analog Input Wiring and Voltage Limits

The firmware reads four analog signals through one ADS1115 at I²C address `0x48`:

| Signal | ADS1115 input | Firmware scaling |
|--------|---------------|------------------|
| pH amplifier output | A0 | 0–3.3 V mapped to pH 0–14 |
| DO amplifier output | A1 | 0–3.3 V mapped to 0–20 mg/L |
| Turbidity output | A2 | 0–3.3 V mapped to 0–4095 |
| CO₂ output | A3 | 0–3.3 V mapped to 0–10 |

These are linear placeholder scales, not a substitute for probe calibration. Keep every
ADS1115 input between GND and its 3.3 V supply. Sensor modules with 5 V analog outputs
must use a suitable divider or signal-conditioning stage before connecting to the ADC.
Power the ADS1115 and its I²C pull-ups from 3.3 V; do not allow 5 V pull-ups on SDA/SCL.
Water level is not currently read by the firmware; adding it requires another ADS1115
with a distinct I²C address (for example, `0x49`) and input voltage conditioning.
Turbidity already uses A2 on the existing ADC. If the ADS1115 is absent, the firmware
marks the required analog readings invalid.
