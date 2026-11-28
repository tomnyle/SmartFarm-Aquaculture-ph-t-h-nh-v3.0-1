#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "app_config.h"

namespace {
constexpr uint8_t RELAY_OE = 13;
constexpr uint8_t SDA_PIN = 21;
constexpr uint8_t SCL_PIN = 22;
constexpr uint8_t TEMPERATURE_PIN = 23;
constexpr uint8_t ADC_ADDRESS = 0x48;
constexpr uint32_t SAMPLE_INTERVAL_MS = 5000;
constexpr uint32_t CONVERSION_TIME_MS = 800;
constexpr char BASE[] = "smartfarm/x8_sensor_bench";
constexpr char CLIENT_ID[] = "smartfarm_x8_sensor_bench";

WiFiClient wifi_client;
PubSubClient mqtt(wifi_client);
OneWire one_wire(TEMPERATURE_PIN);
DallasTemperature temperature(&one_wire);
uint32_t last_wifi_attempt = 0;
uint32_t last_mqtt_attempt = 0;
uint32_t conversion_started = 0;
uint32_t last_sample = 0;
bool conversion_pending = false;

bool read_adc_millivolts(uint8_t channel, float& millivolts) {
  // Single-shot, single-ended, +/-4.096 V range, 128 samples/s.
  uint16_t config = 0xC383 | (static_cast<uint16_t>(channel) << 12);
  Wire.beginTransmission(ADC_ADDRESS);
  Wire.write(0x01);
  Wire.write(static_cast<uint8_t>(config >> 8));
  Wire.write(static_cast<uint8_t>(config));
  if (Wire.endTransmission() != 0) return false;
  delay(12);
  Wire.beginTransmission(ADC_ADDRESS);
  Wire.write(0x01);
  if (Wire.endTransmission(false) != 0 ||
      Wire.requestFrom(ADC_ADDRESS, static_cast<uint8_t>(2)) != 2) return false;
  uint16_t status = static_cast<uint16_t>(Wire.read()) << 8;
  status |= static_cast<uint8_t>(Wire.read());
  if ((status & 0x8000) == 0) return false;
  Wire.beginTransmission(ADC_ADDRESS);
  Wire.write(0x00);
  if (Wire.endTransmission(false) != 0 ||
      Wire.requestFrom(ADC_ADDRESS, static_cast<uint8_t>(2)) != 2) return false;
  uint16_t raw = static_cast<uint16_t>(Wire.read()) << 8;
  raw |= static_cast<uint8_t>(Wire.read());
  int16_t signed_raw = static_cast<int16_t>(raw);
  if (signed_raw < 0) return false;
  millivolts = signed_raw * 0.125f;
  return millivolts <= 3300.0f;
}

void report(const char* name, bool valid, float value, uint8_t decimals) {
  String topic = String(BASE) + "/" + name;
  String payload = valid ? String(value, decimals) : String("unavailable");
  Serial.printf("[SENSOR] %s: %s\n", name, payload.c_str());
  if (mqtt.connected() && !mqtt.publish(topic.c_str(), payload.c_str(), false)) {
    Serial.printf("[SENSOR] MQTT publish failed: %s\n", topic.c_str());
  }
}

void sample_sensors() {
  float voltage = 0;
  bool valid = read_adc_millivolts(0, voltage);
  report("do_adc_mv", valid, voltage, 1);
  valid = read_adc_millivolts(1, voltage);
  report("ph_adc_mv", valid, voltage, 1);
  float water_temp = temperature.getTempCByIndex(0);
  valid = water_temp != DEVICE_DISCONNECTED_C && water_temp >= 0.0f &&
          water_temp <= 60.0f;
  report("water_temp_c", valid, water_temp, 2);
}
}  // namespace

void setup() {
  digitalWrite(RELAY_OE, HIGH);
  pinMode(RELAY_OE, OUTPUT);
  Serial.begin(115200);
  Serial.println("[SENSOR] NO LOAD ONLY. Relays disabled; readings are not calibrated.");
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setTimeOut(50);
  temperature.begin();
  temperature.setWaitForConversion(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  last_wifi_attempt = millis();
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setSocketTimeout(1);
}

void loop() {
  uint32_t now = millis();
  if (WiFi.status() != WL_CONNECTED) {
    if (now - last_wifi_attempt >= WIFI_CONNECT_TIMEOUT) {
      last_wifi_attempt = now;
      WiFi.disconnect();
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
  } else if (!mqtt.connected()) {
    if (now - last_mqtt_attempt >= MQTT_RECONNECT_INTERVAL) {
      last_mqtt_attempt = now;
      String status_topic = String(BASE) + "/status";
      if (mqtt.connect(CLIENT_ID, MQTT_USER, MQTT_PASSWORD, status_topic.c_str(), 1, true, "offline")) {
        if (!mqtt.publish(status_topic.c_str(), "online", true)) {
          Serial.println("[SENSOR] Failed to publish MQTT availability");
          mqtt.disconnect();
        }
      } else {
        Serial.printf("[SENSOR] MQTT connection failed: %d\n", mqtt.state());
      }
    }
  } else {
    mqtt.loop();
  }

  now = millis();
  if (!conversion_pending && now - last_sample >= SAMPLE_INTERVAL_MS) {
    temperature.requestTemperatures();
    conversion_started = now;
    conversion_pending = true;
  } else if (conversion_pending && now - conversion_started >= CONVERSION_TIME_MS) {
    sample_sensors();
    last_sample = now;
    conversion_pending = false;
  }
  delay(10);
}
