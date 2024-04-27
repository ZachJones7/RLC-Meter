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
  Serial.println("Im here");

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

float capacitance;
float capacitance_sum = 0;

float inductance;
float inductance_sum = 0;

float resistance;
float resistance_sum = 0;

// Resistor Value in Series with DUT
float R = 1000;


void calc_value_resistance(){
  float frequencies[4]; // Array to store frequencies
  float magnitudes[4]; // Array to store magnitudes
  float calc_magnitudes[4];
  float error_total = 1;
  float prev_error_total = 2;
  float R2 = 1.0;
  float target_error = 0.0000001;
  float errorSum = 0;
  float capacitance_found;
  float prev_R;

  // Measure magnitudes at different frequencies
  for(int i = 0; i < 4; i++) {
    ad9832.set_freq(rx);
    magnitude_func();
    frequencies[i] = rx;
    magnitudes[i] = magnitude;
    rx *= 10;
    //delay(100); // Adjust delay as needed
  }

  int iteration = 0; // Track the number of iterations
  int oscillation_count = 0;
  const int max_iterations = 3000; // Maximum number of iterations


  while(target_error < error_total){
    for(int i = 0; i<4; i++){
      calc_magnitudes[i] = R2/(R + R2);
    }
  
    for(int i=0; i<4; i++){
      errorSum += abs(magnitudes[i] - calc_magnitudes[i]);
    }
    error_total = errorSum/4;

    double delta_R = R*error_total; // Initial delta_C value


    if (error_total > prev_error_total && R2 < prev_R){
      R2 += delta_R;
    } else if (error_total < prev_error_total && R2 < prev_R) {
      R2 -= delta_R;
    } else if (error_total > prev_error_total && R2 > prev_R){
      R2 -= delta_R;
    } else if (error_total < prev_error_total && R2 > prev_R){
      R2 += delta_R;
    }
    if (error_total == prev_error_total){
      R2 -= delta_R;
    }

    // Check if capacitance is oscillating around the correct value
    Serial.print(abs(prev_error_total - error_total),7);
    Serial.print(" ");
    if (abs(prev_error_total - error_total) < 0.001 && iteration > 500) {
      
      oscillation_count++;
      resistance_sum += R2;
      Serial.println(oscillation_count);

      if(oscillation_count > 10){
        resistance = resistance_sum/oscillation_count;
        resistance_sum = 0;
        oscillation_count = 0;
        break; 
      }

    }

    if(error_total < prev_error_total){
      resistance = R2;
    }

    if(R < 0.0000000000001){
      break;
    }
   
    prev_error_total = error_total;
    errorSum = 0;

    Serial.print(iteration);
    Serial.print(" ");
    Serial.print(R2, 12);
    Serial.print(" ");
    Serial.println(error_total, 12);

    if(iteration > max_iterations){
      break;
    }
    iteration ++;
  }
  
  
  for(int i = 0; i < 4; i++) {
    frequencies[i] = 0;
    magnitudes[i] = 0;
  }
  rx = start_f;
}

void calc_value_capacitance(){
  float frequencies[4]; // Array to store frequencies
  float magnitudes[4]; // Array to store magnitudes
  float calc_magnitudes[4];
  float error_total = 1;
  float prev_error_total = 2;
  float C = 1.0;
  float target_error = 0.0000001;
  float errorSum = 0;
  float capacitance_found;
  float prev_C;

  // Measure magnitudes at different frequencies
  for(int i = 0; i < 4; i++) {
    ad9832.set_freq(rx);
    magnitude_func();
    frequencies[i] = rx;
    magnitudes[i] = magnitude;
    rx *= 10;
    //delay(100); // Adjust delay as needed
  }

  int iteration = 0; // Track the number of iterations
  int oscillation_count = 0;
  const int max_iterations = 3000; // Maximum number of iterations


  while(target_error < error_total){
    for(int i = 0; i<4; i++){
      calc_magnitudes[i] = 1/(sqrt(1 + ((2*frequencies[i]*PI*R*C)*(2*frequencies[i]*PI*R*C))));
    }
  
    for(int i=0; i<4; i++){
      errorSum += abs(magnitudes[i] - calc_magnitudes[i]);
    }
    error_total = errorSum/4;

    double delta_C = C*error_total; // Initial delta_C value


    if (error_total > prev_error_total && C < prev_C){
      C += delta_C;
    } else if (error_total < prev_error_total && C < prev_C) {
      C -= delta_C;
    } else if (error_total > prev_error_total && C > prev_C){
      C -= delta_C;
    } else if (error_total < prev_error_total && C > prev_C){
      C += delta_C;
    }
    if (error_total == prev_error_total){
      C -= delta_C;
    }

    // Check if capacitance is oscillating around the correct value
    Serial.print(abs(prev_error_total - error_total),7);
    Serial.print(" ");
    if (abs(prev_error_total - error_total) < 0.001 && iteration > 500) {
      
      oscillation_count++;
      capacitance_sum += C;
      Serial.println(oscillation_count);

      if(oscillation_count > 10){
        capacitance = capacitance_sum/oscillation_count;
        capacitance_sum = 0;
        oscillation_count = 0;
        break; 
      }

    }

    if(error_total < prev_error_total){
      capacitance = C;
    }

    if(C < 0.0000000000001){
      break;
    }
   
    prev_error_total = error_total;
    errorSum = 0;

    Serial.print(iteration);
    Serial.print(" ");
    Serial.print(C, 12);
    Serial.print(" ");
    Serial.println(error_total, 12);

    if(iteration > max_iterations){
      break;
    }
    iteration ++;
  }
  
  
  for(int i = 0; i < 4; i++) {
    frequencies[i] = 0;
    magnitudes[i] = 0;
  }
  rx = start_f;
}

void calc_value_inductance(){
  float frequencies[4]; // Array to store frequencies
  float magnitudes[4]; // Array to store magnitudes
  float calc_magnitudes[4];
  float error_total = 1;
  float prev_error_total = 2;
  float L = 1.0;
  float target_error = 0.0000001;
  float errorSum = 0;
  float capacitance_found;
  float prev_L;

  // Measure magnitudes at different frequencies
  for(int i = 0; i < 4; i++) {
    ad9832.set_freq(rx);
    magnitude_func();
    frequencies[i] = rx;
    magnitudes[i] = magnitude;
    rx *= 10;
    //delay(100); // Adjust delay as needed
  }

  int iteration = 0; // Track the number of iterations
  int oscillation_count = 0;
  const int max_iterations = 3000; // Maximum number of iterations


  while(target_error < error_total){
    for(int i = 0; i<4; i++){
      calc_magnitudes[i] = (2*PI*frequencies[i]*L)/sqrt((R*R) + ((2*PI*frequencies[i]*L)*(2*PI*frequencies[i]*L)));
    }
  
    for(int i=0; i<4; i++){
      errorSum += abs(magnitudes[i] - calc_magnitudes[i]);
    }
    error_total = errorSum/4;

    double delta_L = L*error_total; // Initial delta_C value


    if (error_total > prev_error_total && L < prev_L){
      L += delta_L;
    } else if (error_total < prev_error_total && L < prev_L) {
      L -= delta_L;
    } else if (error_total > prev_error_total && L > prev_L){
      L -= delta_L;
    } else if (error_total < prev_error_total && L > prev_L){
      L += delta_L;
    }
    if (error_total == prev_error_total){
      L -= delta_L;
    }

    // Check if capacitance is oscillating around the correct value
    Serial.print(abs(prev_error_total - error_total),7);
    Serial.print(" ");
    if (abs(prev_error_total - error_total) < 0.001 && iteration > 500) {
      
      oscillation_count++;
      inductance_sum += L;
      Serial.println(oscillation_count);

      if(oscillation_count > 10){
        inductance = inductance_sum/oscillation_count;
        inductance_sum = 0;
        oscillation_count = 0;
        break; 
      }

    }

    if(error_total < prev_error_total){
      inductance = L;
    }

    if(L < 0.0000000000001){
      break;
    }
   
    prev_error_total = error_total;
    errorSum = 0;

    Serial.print(iteration);
    Serial.print(" ");
    Serial.print(L, 12);
    Serial.print(" ");
    Serial.println(error_total, 12);

    if(iteration > max_iterations){
      break;
    }
    iteration ++;
  }
  
  
  for(int i = 0; i < 4; i++) {
    frequencies[i] = 0;
    magnitudes[i] = 0;
  }
  rx = start_f;
}

//uint32_t start_f = rx;



int DUT = 2; //0 = R, 1 = C, 2 = L

void loop() {
  // put your main code here, to run repeatedly:
  ad9832.set_freq(i);

  if(DUT == 0){
    calc_value_resistance();
     Serial.print("Resistance: ");
    Serial.println(resistance);
  }else if(DUT == 1){
    calc_value_capacitance();
    Serial.print("Capacitance: ");
    Serial.println(capacitance, 12);
  }else if(DUT == 2){
     calc_value_inductance();
     Serial.print("Inductance: ");
    Serial.println(inductance, 12);
  }
  

} 
