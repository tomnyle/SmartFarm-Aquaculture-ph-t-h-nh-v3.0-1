#ifndef PINS_H
#define PINS_H

// ==================== SENSOR PINS ====================
// ADS1115 single-ended analog input channels
#define ADS1115_ADDRESS 0x48
#define PH_ADC_CHANNEL 0
#define DO_ADC_CHANNEL 1
#define TURBIDITY_ADC_CHANNEL 2
#define CO2_ADC_CHANNEL 3

// 1-Wire Bus
#define ONE_WIRE_BUS 4      // DS18B20 temperature sensor

// DHT Sensor
#define DHTPIN 15           // DHT22 data pin
#define DHTTYPE DHT22       // DHT22 sensor type

// I2C Pins (BH1750 Light Sensor)
#define I2C_SDA 21          // I2C Data
#define I2C_SCL 22          // I2C Clock

// ==================== OUTPUT PINS ====================
// Relay Pins
#define PUMP_PIN 13         // Main pump relay
#define AERATOR_PIN 25      // Aerator relay (GPIO12 is a boot-strapping pin)
#define CIRCULATION_PIN 14  // Circulation pump relay
#define FEEDER_PIN 27       // Automatic feeder relay
#define RELAY5_PIN 32       // Reserved relay output
#define RELAY6_PIN 33       // Reserved relay output
#define RELAY7_PIN 26       // Reserved relay output
#define RELAY8_PIN 23       // Reserved relay output

// Set RELAY_ACTIVE_LEVEL to LOW for active-low relay modules.
#ifndef RELAY_ACTIVE_LEVEL
#define RELAY_ACTIVE_LEVEL HIGH
#endif
#define RELAY_INACTIVE_LEVEL ((RELAY_ACTIVE_LEVEL) == HIGH ? LOW : HIGH)

// Status LED (Optional)
#define LED_PIN 2           // Built-in LED for status

// ==================== SPI PINS (Optional - Reserved) ====================
// #define SPI_MOSI 23
// #define SPI_MISO 19
// #define SPI_CLK 18

#endif // PINS_H
