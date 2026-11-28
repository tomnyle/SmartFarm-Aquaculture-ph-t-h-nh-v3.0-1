#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "app_config.h"
#include "x8_bench_safety.h"

namespace {
constexpr uint8_t OE = 13;
constexpr uint8_t LATCH = 25;
constexpr uint8_t CLOCK = 26;
constexpr uint8_t DATA = 33;
constexpr uint8_t BOOT = 0;
constexpr char BASE[] = "smartfarm/x8_bench";
constexpr char CLIENT_ID[] = "smartfarm_x8_bench";
constexpr uint32_t BUTTON_DEBOUNCE_MS = 50;

WiFiClient wifi_client;
PubSubClient mqtt(wifi_client);
X8BenchSafety safety;
String command_session;
int applied_relay = -1;
bool applied_armed = false;
bool button_raw = HIGH;
bool button_stable = HIGH;
bool button_released = false;
uint32_t button_changed_at = 0;
uint32_t last_wifi_attempt = 0;
uint32_t last_mqtt_attempt = 0;

String relay_topic(uint8_t relay, const char* suffix) {
  return String(BASE) + "/relay/" + String(relay + 1) + "/" + suffix;
}

String relay_command_topic(uint8_t relay) {
  return String(BASE) + "/command/" + command_session + "/" + String(relay + 1);
}

bool publish(const String& topic, const char* payload) {
  if (mqtt.publish(topic.c_str(), payload, true)) return true;
  Serial.println("[BENCH] Failed to publish: " + topic);
  return false;
}

void write_outputs(int relay) {
  uint8_t bits = relay < 0 ? 0xFF : static_cast<uint8_t>(~(1U << relay));
  digitalWrite(LATCH, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, bits);
  digitalWrite(LATCH, HIGH);
}

void sync_state() {
  if (applied_relay != safety.active()) {
    int previous = applied_relay;
    applied_relay = safety.active();
    write_outputs(applied_relay);
    if (mqtt.connected()) {
      if (previous >= 0) publish(relay_topic(previous, "state"), "OFF");
      if (applied_relay >= 0) publish(relay_topic(applied_relay, "state"), "ON");
    }
  }
  if (applied_armed != safety.armed()) {
    applied_armed = safety.armed();
    if (mqtt.connected()) publish(String(BASE) + "/armed", applied_armed ? "ON" : "OFF");
  }
}

void on_message(char* topic, byte* payload, unsigned int length) {
  for (uint8_t i = 0; i < X8BenchSafety::relay_count; ++i) {
    if (relay_command_topic(i) != topic) continue;
    if (length == 2 && memcmp(payload, "ON", 2) == 0) {
      if (!safety.start(i, millis())) Serial.println("[BENCH] ON rejected: press BOOT to arm");
    } else if (length == 3 && memcmp(payload, "OFF", 3) == 0) {
      safety.stop(i);
    } else {
      Serial.println("[BENCH] Invalid relay command");
    }
    sync_state();
    // Correct optimistic or stale UI state even when a command was rejected.
    publish(relay_topic(i, "state"), safety.active() == i ? "ON" : "OFF");
    return;
  }
}

bool publish_discovery() {
  for (uint8_t i = 0; i < X8BenchSafety::relay_count; ++i) {
    String id = "smartfarm_x8_bench_relay_" + String(i + 1);
    String topic = String(HA_DISCOVERY_PREFIX) + "/switch/" + id + "/config";
    String payload = "{\"name\":\"X8 bench relay " + String(i + 1) +
        "\",\"unique_id\":\"" + id +
        "\",\"command_topic\":\"" + relay_command_topic(i) +
        "\",\"state_topic\":\"" + relay_topic(i, "state") +
        "\",\"availability_topic\":\"" + String(BASE) + "/status\"}";
    if (!publish(topic, payload.c_str()) || !publish(relay_topic(i, "state"), "OFF")) return false;
  }
  return publish(String(BASE) + "/armed", "OFF");
}

void update_button(uint32_t now) {
  bool reading = digitalRead(BOOT);
  if (reading != button_raw) {
    button_raw = reading;
    button_changed_at = now;
  }
  if (reading == button_stable || now - button_changed_at < BUTTON_DEBOUNCE_MS) return;
  button_stable = reading;
  if (button_stable == HIGH) {
    button_released = true;
  } else if (button_released && mqtt.connected()) {
    safety.arm(now);
    Serial.println("[BENCH] Armed for one relay pulse");
  }
}
}  // namespace

void setup() {
  // Hold outputs disabled until the inactive byte is in the storage register.
  digitalWrite(OE, HIGH);
  pinMode(OE, OUTPUT);
  digitalWrite(LATCH, LOW);
  digitalWrite(CLOCK, LOW);
  digitalWrite(DATA, HIGH);
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);
  write_outputs(-1);
  digitalWrite(OE, LOW);
  pinMode(BOOT, INPUT_PULLUP);
  button_raw = button_stable = digitalRead(BOOT);
  button_released = button_stable == HIGH;

  Serial.begin(115200);
  Serial.println("[BENCH] NO LOAD ONLY. Press BOOT to allow one five-second relay pulse.");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  last_wifi_attempt = millis();
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(on_message);
  if (!mqtt.setBufferSize(512)) {
    Serial.println("[BENCH] MQTT buffer allocation failed; relays remain off");
    while (true) delay(1000);
  }
  mqtt.setSocketTimeout(1);
}

void loop() {
  uint32_t now = millis();
  if (WiFi.status() != WL_CONNECTED || !mqtt.connected()) safety.disconnect();
  safety.tick(now);
  update_button(now);
  sync_state();

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
        command_session = String(esp_random(), HEX) + String(esp_random(), HEX);
        bool ready = publish_discovery();
        for (uint8_t i = 0; i < X8BenchSafety::relay_count; ++i) {
          String topic = relay_command_topic(i);
          if (!mqtt.subscribe(topic.c_str())) {
            Serial.println("[BENCH] Failed to subscribe: " + topic);
            ready = false;
          }
          Serial.println("[BENCH] Command topic: " + topic);
        }
        if (!ready || !publish(status_topic, "online")) {
          publish(status_topic, "offline");
          mqtt.disconnect();
        }
      } else {
        Serial.printf("[BENCH] MQTT connection failed: %d\n", mqtt.state());
      }
    }
  } else {
    mqtt.loop();
  }
  delay(10);
}
