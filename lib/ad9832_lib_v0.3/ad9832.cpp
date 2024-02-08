// AD9832 library for Arduino, v0.3
// 9.10.2018, Arduino IDE v1.8.7, LZ2WSG, KN34PC
//---------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include "ad9832.h"
//---------------------------------------------------------------------------------------------------------
AD9832::AD9832(uint8_t _fsync, uint8_t _clk, uint8_t _data) {
  FSYNC = _fsync;
  CLK = _clk;
  DATA = _data;
}
//---------------------------------------------------------------------------------------------------------
void AD9832::init(uint32_t xt) {
  F_MCLK = xt;

  pinMode(FSYNC, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DATA, OUTPUT);

  digitalWrite(FSYNC, HIGH);
  digitalWrite(CLK, HIGH);

  write_reg(0xF800);  // 1, 1, SLEEP = 1, RESET = 1, CLR = 1
  write_reg(0xB000);  // 1, 0, SYNC = 1, SELSRC = 1
  write_reg(0xC000);  // 1, 1, SLEEP = 0, RESET = 0, CLR = 0
}
//---------------------------------------------------------------------------------------------------------
void AD9832::set_freq(uint32_t frequency) {
  uint32_t f = (uint32_t)(((uint64_t)frequency << 32) / F_MCLK);

  uint16_t data_3 = 0x3300 | (0x00FF & (uint16_t)(f >> 24));  // FREQ0 H-MSBs
  uint16_t data_2 = 0x2200 | (0x00FF & (uint16_t)(f >> 16));  // FREQ0 L-MSBs
  uint16_t data_1 = 0x3100 | (0x00FF & (uint16_t)(f >> 8));   // FREQ0 H-LSBs
  uint16_t data_0 = 0x2000 | (0x00FF & (uint16_t)f);          // FREQ0 L-LSBs

  write_reg(data_3);
  write_reg(data_2);
  write_reg(data_1);
  write_reg(data_0);

  write_reg(0x1900);  // PHASE0 MSBs
  write_reg(0x0800);  // PHASE0 LSBs
}
//---------------------------------------------------------------------------------------------------------
void AD9832::write_reg(uint16_t data) {
  digitalWrite(CLK, HIGH);                     // falling edges
  digitalWrite(FSYNC, LOW);
  shiftOut(DATA, CLK, MSBFIRST, data >> 8);    // high byte
  shiftOut(DATA, CLK, MSBFIRST, data & 0xFF);  // low byte
  digitalWrite(FSYNC, HIGH);
}
//---------------------------------------------------------------------------------------------------------
void AD9832::sleep(uint8_t state) {
  if (state == 0)
    write_reg(0xC000);                         // power up
  else if (state == 1)
    write_reg(0xE000);                         // power down
}
