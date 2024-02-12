#include "ad9832.h"

#define FSYNC  10
#define CLK    8
#define DATA   7
#define FXTAL  24000000              // F_MCLK is 25 MHz

AD9832 ad9832(FSYNC, CLK, DATA);     // Arduino pins to FSYNC, CLK, DATA

uint32_t start_f = 100;

uint32_t end_f = 1000000;

//uint32_t rx = 1000000;               // test 5 MHz
//---------------------------------------------------------------------------------------------------------
void setup() {
  ad9832.init(FXTAL);
  Serial.begin(115200);
}
//---------------------------------------------------------------------------------------------------------

void loop() {
  for(int i=start_f; i <= end_f;){
    Serial.println(i);
    ad9832.set_freq(i); 
    i = i*10;
    delay(3000);
  }
  
}

