// AD9832 generator test
// 3.09.2017, Arduino v1.8.5
//---------------------------------------------------------------------------------------------------------
#include "ad9832.h"

#define FSYNC  10
#define CLK    8
#define DATA   7
#define FXTAL  24000000              // F_MCLK is 25 MHz

AD9832 ad9832(FSYNC, CLK, DATA);     // Arduino pins to FSYNC, CLK, DATA

uint32_t rx = 100;               // test 5 MHz
//---------------------------------------------------------------------------------------------------------
void setup() {
  ad9832.init(FXTAL);
  ad9832.set_freq(rx);
  digitalWrite(FSYNC, LOW);
}
//---------------------------------------------------------------------------------------------------------
void loop() {
}
