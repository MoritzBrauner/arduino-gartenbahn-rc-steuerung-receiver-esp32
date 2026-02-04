#include <Arduino.h>

#define PIN_CE  22
#define PIN_CSN 21

#define PIN_PWM_FORWARD 32 
#define PIN_PWM_BACKWARD 33

#define PIN_HORN 2

#define PIN_LIGHT_INTERIOR 15

#define PIN_LIGHT_FL_A 12
#define PIN_LIGHT_FL_B 14
#define PIN_LIGHT_FT   27
#define PIN_LIGHT_FR_A 26
#define PIN_LIGHT_FR_B 25

#define PIN_LIGHT_RL_A 0
#define PIN_LIGHT_RL_B 4
#define PIN_LIGHT_RT   16
#define PIN_LIGHT_RR_A 17
#define PIN_LIGHT_RR_B 5

void initPins() {
  //Set Pin Modes
  Serial.println("Setting pin modes...");
  pinMode(PIN_HORN, OUTPUT);
  pinMode(PIN_LIGHT_INTERIOR, OUTPUT); 
  pinMode(PIN_LIGHT_FL_A, OUTPUT); 
  pinMode(PIN_LIGHT_FL_B, OUTPUT); 
  pinMode(PIN_LIGHT_FR_A, OUTPUT); 
  pinMode(PIN_LIGHT_FR_B, OUTPUT); 
  pinMode(PIN_LIGHT_FT, OUTPUT); 
  pinMode(PIN_LIGHT_RL_A, OUTPUT); 
  pinMode(PIN_LIGHT_RL_B, OUTPUT); 
  pinMode(PIN_LIGHT_RR_A, OUTPUT); 
  pinMode(PIN_LIGHT_RR_B, OUTPUT); 
  pinMode(PIN_LIGHT_RT, OUTPUT); 
  
  //Write all Pins LOW 
  Serial.println("Writing pins low...");
  digitalWrite(PIN_HORN, LOW);
  digitalWrite(PIN_LIGHT_INTERIOR, LOW); 
  digitalWrite(PIN_LIGHT_FL_A, LOW); 
  digitalWrite(PIN_LIGHT_FL_B, LOW); 
  digitalWrite(PIN_LIGHT_FR_A, LOW); 
  digitalWrite(PIN_LIGHT_FR_B, LOW); 
  digitalWrite(PIN_LIGHT_FT, LOW); 
  digitalWrite(PIN_LIGHT_RL_A, LOW); 
  digitalWrite(PIN_LIGHT_RL_B, LOW); 
  digitalWrite(PIN_LIGHT_RR_A, LOW); 
  digitalWrite(PIN_LIGHT_RR_B, LOW); 
  digitalWrite(PIN_LIGHT_RT, LOW); 
}

