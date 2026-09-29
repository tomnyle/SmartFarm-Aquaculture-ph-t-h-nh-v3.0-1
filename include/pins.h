#ifndef PINS_H
#define PINS_H

// =====================================================================
// SmartFarm Aquaculture Controller - Hardware Pin Map Rev.A
// ---------------------------------------------------------------------
// This file is the ONLY source of truth for GPIO / ADC channel mapping.
// The PCB schematic (see docs/wiring.md) must use exactly this map.
// Do not define pins anywhere else in the codebase.
// =====================================================================

// ==================== ADS1115 (ANALOG WATER QUALITY) ====================
#define ADS1115_ADDRESS 0x48  // ADDR pin tied to GND
#define PH_CHANNEL 0          // ADS1115 A0 - pH sensor
#define DO_CHANNEL 1          // ADS1115 A1 - Dissolved Oxygen sensor
#define TURB_CHANNEL 2        // ADS1115 A2 - Turbidity sensor
#define CO2_CHANNEL 3         // ADS1115 A3 - CO2 sensor

// ==================== I2C BUS (ADS1115 + BH1750) ====================
#define I2C_SDA 21            // I2C Data
#define I2C_SCL 22            // I2C Clock

// ==================== SENSOR PINS ====================
#define ONE_WIRE_BUS 4        // DS18B20 water temperature (4.7k pull-up to 3.3V)
#define DHTPIN 15             // DHT22 data pin (strapping pin, input only use)
#define DHTTYPE DHT22         // DHT22 sensor type
#define WATER_LEVEL_PIN 33    // Float switch / water level safety interlock (INPUT_PULLUP)

// ==================== OUTPUT PINS (RELAY DRIVERS) ====================
#define PUMP_PIN 13           // Relay 1 - Main water pump (float-switch interlocked)
#define AERATOR_PIN 25        // Relay 2 - Aerator
#define CIRCULATION_PIN 14    // Relay 3 - Circulation pump
#define FEEDER_PIN 27         // Relay 4 - Automatic feeder
#define VALVE_PIN 26          // Relay 5 - Valve
#define LIGHT_PIN 32          // Relay 6 - Light
#define SPARE1_PIN 12         // Relay 7 - Spare 1 (strapping pin: must be LOW at boot)
#define SPARE2_PIN 16         // Relay 8 - Spare 2

// ==================== STATUS ====================
#define LED_PIN 2             // Status LED (strapping pin, keep LED load light)

#endif // PINS_H
