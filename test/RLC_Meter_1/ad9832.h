#include "Arduino.h"

#ifndef ad9832_h
#define ad9832_h
//---------------------------------------------------------------------------------------------------------
class AD9832 {
  public:
    AD9832(uint8_t, uint8_t, uint8_t);               // Arduino pins to FSYNC, CLK, DATA
    void init(uint32_t xt);                          // XTAL crystal frequency [Hz]
    void set_freq(uint32_t frequency);               // Fout [Hz]
    void sleep(uint8_t state);                       // state [0: power on, 1: power off]

  private:
    uint32_t F_MCLK;
    uint8_t FSYNC;
    uint8_t CLK;
    uint8_t DATA;

    void write_reg(uint16_t data);
};
//---------------------------------------------------------------------------------------------------------
#endif
