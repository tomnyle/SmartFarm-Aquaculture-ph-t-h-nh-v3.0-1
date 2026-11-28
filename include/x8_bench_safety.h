#ifndef X8_BENCH_SAFETY_H
#define X8_BENCH_SAFETY_H

#include <stdint.h>

class X8BenchSafety {
 public:
  static const uint8_t relay_count = 8;
  static const uint32_t arm_timeout_ms = 30000;
  static const uint32_t pulse_ms = 5000;

  void arm(uint32_t now) {
    if (active_ < 0) {
      armed_ = true;
      armed_at_ = now;
    }
  }

  bool start(uint8_t relay, uint32_t now) {
    tick(now);
    if (!armed_ || active_ >= 0 || relay >= relay_count) {
      return false;
    }
    armed_ = false;
    active_ = relay;
    started_at_ = now;
    return true;
  }

  void stop(uint8_t relay) {
    if (active_ == relay) active_ = -1;
  }

  void disconnect() {
    armed_ = false;
    active_ = -1;
  }

  void tick(uint32_t now) {
    if (armed_ && now - armed_at_ >= arm_timeout_ms) armed_ = false;
    if (active_ >= 0 && now - started_at_ >= pulse_ms) active_ = -1;
  }

  bool armed() const { return armed_; }
  int active() const { return active_; }

 private:
  bool armed_ = false;
  int active_ = -1;
  uint32_t armed_at_ = 0;
  uint32_t started_at_ = 0;
};

#endif
