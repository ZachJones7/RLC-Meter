#include "ad9832.h"

#define FSYNC  10
#define CLK    8
#define DATA   7
#define FXTAL  24000000              // F_MCLK is 25 MHz

AD9832 ad9832(FSYNC, CLK, DATA);     // Arduino pins to FSYNC, CLK, DATA

uint32_t start_f = 2000;

uint32_t end_f = 100000;

//uint32_t rx = 1000000;               // test 5 MHz
//---------------------------------------------------------------------------------------------------------
void setup() {
  ad9832.init(FXTAL);
  Serial.begin(115200);
}
//---------------------------------------------------------------------------------------------------------

uint32_t i = start_f;

void loop() {
  
  Serial.println(i);
  ad9832.set_freq(i); 
  i += 10000;
  delay(100);

  if(i > 1000000){
    i = start_f;
  }

  
}

