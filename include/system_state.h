#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <Arduino.h>
#include <ArduinoJson.h>

// ==================== SENSOR READINGS ====================

struct SensorReadings {
    float water_temperature;      // °C
    float water_ph;               // pH units
    float dissolved_oxygen;       // mg/L or %
    float water_level;            // cm
    
    uint32_t temperature_timestamp;
    uint32_t ph_timestamp;
    uint32_t do_timestamp;
    uint32_t level_timestamp;
    
    bool temperature_valid;
    bool ph_valid;
    bool do_valid;
    bool level_valid;
};

// ==================== OUTPUT STATES ====================
// One entry per relay output in pins.h (Rev.A: 8 outputs).
// Each output is published on smartfarm/aquaculture/output/<name> and
// controlled via smartfarm/aquaculture/control/<name>/set.

struct OutputStates {
    bool pump;              // Bơm cấp nước (PUMP_PIN) - float-switch interlocked
    bool aerator;           // Máy sục khí (AERATOR_PIN)
    bool circulation;       // Bơm tuần hoàn (CIRCULATION_PIN)
    bool feeder;            // Máy cho ăn (FEEDER_PIN)
    bool valve;             // Van (VALVE_PIN)
    bool light;             // Đèn (LIGHT_PIN)
    bool spare1;            // Dự phòng 1 (SPARE1_PIN)
    bool spare2;            // Dự phòng 2 (SPARE2_PIN)
    
    uint32_t pump_on_time;
    uint32_t aerator_on_time;
    uint32_t circulation_on_time;
    uint32_t feeder_on_time;
    uint32_t valve_on_time;
    uint32_t light_on_time;
    uint32_t spare1_on_time;
    uint32_t spare2_on_time;
};

// ==================== WATER LEVEL SAFETY ====================
// Float switch on WATER_LEVEL_PIN. While water_level_low is true the pump
// is forced OFF and every ON request (manual, AUTO, relay test) is rejected.
// Published on smartfarm/aquaculture/sensor/water_level_low (ON = low water).

struct WaterLevelSafetyState {
    bool water_level_low;          // Debounced low-water state (true = pump locked)
    bool raw_low;                  // Last raw float switch reading
    uint32_t last_change;          // millis() of last raw change
    uint32_t pump_blocked_count;   // Number of pump ON requests rejected
};

// ==================== SYSTEM STATUS ====================

enum SystemMode {
    MODE_AUTO,      // Tự động theo cảm biến
    MODE_MANUAL,    // Điều khiển từ HA
    MODE_SCHEDULE,  // Lịch biểu
    MODE_SAFE       // Chế độ an toàn
};

enum SystemStatus {
    STATUS_INITIALIZING,
    STATUS_RUNNING,
    STATUS_ERROR,
    STATUS_SAFE,
    STATUS_DISCONNECTED
};

enum ErrorCode {
    ERROR_NONE = 0,
    ERROR_TEMP_SENSOR = 1,
    ERROR_PH_SENSOR = 2,
    ERROR_DO_SENSOR = 3,
    ERROR_LEVEL_SENSOR = 4,
    ERROR_MQTT_DISCONNECTED = 5,
    ERROR_WATER_LEVEL_LOW = 6,
    ERROR_TEMP_CRITICAL = 7,
    ERROR_DO_CRITICAL = 8,
    ERROR_WATCHDOG = 9
};

struct SystemState {
    SystemMode mode;
    SystemStatus status;
    uint32_t status_timestamp;
    
    uint32_t uptime;           // seconds
    uint32_t last_mqtt_message; // timestamp
    uint32_t last_rule_run;     // timestamp
    
    float cpu_load;            // 0.0 - 100.0
    uint32_t free_memory;
    
    bool mqtt_connected;
    bool ha_discovered;        // Home Assistant discovered
    bool config_valid;
    
    uint16_t error_code;       // ErrorCode
    char error_message[128];
};

// ==================== COMPLETE SYSTEM STATE ====================

struct AquacultureSystemState {
    // Device info
    char device_id[64];
    char firmware_version[32];
    uint32_t startup_time;
    
    // Data
    SensorReadings sensors;
    OutputStates outputs;
    WaterLevelSafetyState water_level;
    SystemState system;
    
    // Active profile
    char active_profile[32];
    
    // Last 10 events (for debugging)
    struct {
        uint32_t timestamp;
        char message[128];
    } events[10];
    uint8_t event_index;
};

// ==================== STATE MANAGEMENT ====================

class StateManager {
public:
    static void initializeState(AquacultureSystemState& state);
    static void updateSensorReading(AquacultureSystemState& state, const char* sensor_type, float value);
    static void updateOutputState(AquacultureSystemState& state, const char* output_name, bool output_state);
    static void setSystemMode(AquacultureSystemState& state, SystemMode mode);
    static void setSystemStatus(AquacultureSystemState& state, SystemStatus status, ErrorCode error = ERROR_NONE);
    static void addEvent(AquacultureSystemState& state, const char* message);
    static JsonDocument stateToJson(const AquacultureSystemState& state);
    static void printState(const AquacultureSystemState& state);
};

#endif // SYSTEM_STATE_H
