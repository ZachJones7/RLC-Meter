#include "Wire.h"
#include "MCP4725.h"
#include "ad9832.h"

#define FSYNC 10
#define CLK   8 
#define DATA  7
#define FXTAL 24000000
#define comp1 2
#define comp2 3

AD9832 ad9832(FSYNC, CLK, DATA);
MCP4725 MCP(0x62);

uint32_t rx = 10000;

float V_out;
float V_in;
float period;
float dt;
float prev_time;
float magnitude;

int DAC_out = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin();
  
  // Setup the DAC
  MCP.begin();
  MCP.setValue(DAC_out);

  // Setup DDS
  ad9832.init(FXTAL);
  ad9832.set_freq(rx);
  digitalWrite(FSYNC, LOW);

  pinMode(comp1, INPUT);
  pinMode(comp2, INPUT);

  Serial.println("Begin");
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
float R = 50;

// Offset applied to Vin and Vout before magnitude measured
float offset_val = 1.96; 

int DUT = 2; //0 = R, 1 = C, 2 = L

void loop() {
  ad9832.set_freq(i);
  //Serial.print(i);
  //Serial.print(" ");
  magnitude_func();
  calc_values();
}

void magnitude_func() {
  DAC_out = 0;
  dt = 0;
  MCP.setValue(DAC_out);
  prev_time = millis();
  while(dt < 0.1){
    dt = (millis() - prev_time)/1000;
    if (digitalRead(comp1) == 0){
      DAC_out++;
      MCP.setValue(DAC_out);
      prev_time = millis();
    }
  }
  V_in = (MCP.getValue()*5.0/4096) - offset_val;

  DAC_out = 0;
  dt = 0;
  MCP.setValue(DAC_out);
  prev_time = millis();
  while(dt<1){
    dt = (millis() - prev_time)/1000;
    if (digitalRead(comp2) == 0){
      DAC_out++;
      MCP.setValue(DAC_out);
      prev_time = millis();
    }
  }
  V_out = (MCP.getValue()*5.0/4096) - offset_val;

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
  
  i += 20000;

  if(i > 100000){
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




