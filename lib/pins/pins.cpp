#include "pins.h"

#include <Arduino.h>

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
