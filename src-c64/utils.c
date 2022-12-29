#include <c64.h>
#include <conio.h>
#include <stdint.h>
#include "utils.h"

void start_timing(void) {
  CIA2.tod_hour = 0;
  CIA2.tod_min = 0;
  CIA2.tod_sec = 0;
  CIA2.tod_10 = 0;
}

void print_timing(void) {
  uint8_t hour = CIA2.tod_hour;
  uint8_t min  = CIA2.tod_min;
  uint8_t sec  = CIA2.tod_sec;
  uint8_t tsec = CIA2.tod_10;

  cprintf("%02x:%02x:%02x.%02x", hour, min, sec, tsec);
}
