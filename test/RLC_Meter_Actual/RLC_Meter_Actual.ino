#include <Wire.h>
#include "MCP3X21.h"
#include "MCP47X6.h"
#include "ad9832.h"
//RLC Meter 

#define FSYNC 10
#define CLK   8 
#define DATA  7
#define FXTAL 24000000
#define comp1 2
#define comp2 3

MCP47X6 DAC=MCP47X6(MCP47X6_DEFAULT_ADDRESS);
AD9832 ad9832(FSYNC, CLK, DATA);

uint32_t rx = 1000;

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  TWBR = 12;
  Serial.begin(115200);

  DAC.begin();
  DAC.setVReference(5.12);
  DAC.setGain(MCP47X6_GAIN_1X);
  DAC.saveSettings();

  ad9832.init(FXTAL);
  ad9832.set_freq(rx);
  Serial.print("Im here");

}

float V_out;
float V_in;
float period;
float dt;
float prev_time;
float magnitude;
uint16_t DAC_out = 0;

void magnitude_func(){
  DAC_out = 0;
  dt = 0;
  DAC.setOutputLevel(DAC_out);
  prev_time = millis();
  while(dt < 0.1){
    dt = (millis() - prev_time)/1000;
    //Serial.println(dt);
    if (digitalRead(comp1) == 1){
      DAC_out++;
      DAC.setOutputLevel(DAC_out);
      prev_time = millis();
    }
  }
  V_in = (DAC_out*5.0/4096);

  DAC_out = 0;
  dt = 0;
  DAC.setOutputLevel(DAC_out);
  prev_time = millis();
  while(dt < 0.1){
    dt = (millis() - prev_time)/1000;
    if (digitalRead(comp2) == 1){
      DAC_out++;
      DAC.setOutputLevel(DAC_out);
      prev_time = millis();
    }
  }
  V_out = (DAC_out*5.0/4096);

  magnitude = V_out/V_in;
  if (magnitude > 1.0){
    magnitude = 1.0;
  }
  Serial.print(V_out);
  Serial.print(" ");
  Serial.print(V_in);
  Serial.print(" ");
  Serial.println(magnitude);
}

uint32_t start_f = rx;
uint32_t i = start_f;

float avg_capacitance;
float sum_avg_capacitance = 0;

float avg_inductance;
float sum_avg_inductance = 0;

float avg_resistance;
float sum_avg_resistance = 0;

//std::vector<double> frequencies_measured;
//std::vector<double> magnitudes_measured;

// Resistor Value in Series with DUT
float R = 1000;

int DUT = 1; //0 = R, 1 = C, 2 = L


void calc_values(){
  
  if (DUT == 0){
    sum_avg_resistance += (magnitude*R)/(1-magnitude);
    Serial.println((magnitude*R)/(1-magnitude));
  }
  if (DUT == 1){
    sum_avg_capacitance += sqrt((1/magnitude)*(1/magnitude)-1)/(2*PI*float(i)*R);
  }
  if (DUT == 2){
    sum_avg_inductance += sqrt(((magnitude*magnitude)*(R*R))/(1-(magnitude*magnitude)))/(2*PI*float(i));
  }
  
  i *= 10;

  if(i > 1000000){
    if (DUT == 0){
      avg_resistance = sum_avg_resistance/5;
      Serial.print(" ");
      Serial.println(avg_resistance,12);
      sum_avg_resistance = 0;
    }
    if (DUT == 1){
      avg_capacitance = sum_avg_capacitance/5;
      Serial.print(" ");
      Serial.println(avg_capacitance,12);
      sum_avg_capacitance = 0;
    }
    if (DUT == 2){
      avg_inductance = sum_avg_inductance/5;
      Serial.print(" ");
      Serial.println(avg_inductance,12);
      sum_avg_inductance = 0;
    }
    
    i = start_f;
    
  }
}

//void curve_fit(){
  
//}



void loop() {
  // put your main code here, to run repeatedly:
  ad9832.set_freq(i);
  magnitude_func();
  calc_values();
} 
