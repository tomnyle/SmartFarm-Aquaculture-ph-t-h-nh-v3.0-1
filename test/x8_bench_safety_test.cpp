#include <cassert>
#include <cstdint>
#include "x8_bench_safety.h"

int main() {
  X8BenchSafety safety;
  assert(!safety.start(0, 0));
  safety.arm(10);
  assert(!safety.start(8, 11));
  assert(safety.start(7, 11));
  assert(safety.active() == 7 && !safety.armed());
  safety.arm(12);
  assert(!safety.start(0, 13));
  safety.tick(5010);
  assert(safety.active() == 7);
  safety.tick(5011);
  assert(safety.active() == -1);
  assert(!safety.start(0, 5012));

  safety.arm(6000);
  assert(!safety.start(0, 36000));
  safety.arm(UINT32_MAX - 100);
  assert(safety.start(0, UINT32_MAX - 50));
  safety.tick(4949);
  assert(safety.active() == -1);
  safety.arm(100);
  assert(safety.start(1, 101));
  safety.disconnect();
  assert(safety.active() == -1 && !safety.armed());
  safety.arm(200);
  safety.disconnect();
  assert(!safety.start(1, 201));
  safety.arm(300);
  assert(safety.start(2, 301));
  safety.stop(1);
  assert(safety.active() == 2);
  safety.stop(2);
  assert(safety.active() == -1);
}
