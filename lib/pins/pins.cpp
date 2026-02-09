#include "pins.h"

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

void writePin(uint8_t pin, bool status) {
  digitalWrite(pin, status ? HIGH : LOW); 
}

void writeFrontLights(bool fla, bool flb, bool fra, bool frb, bool ft) {
  writePin(PIN_LIGHT_FL_A, fla);
  writePin(PIN_LIGHT_FL_B, flb);
  writePin(PIN_LIGHT_FR_A, fra);
  writePin(PIN_LIGHT_FR_B, frb); 
  writePin(PIN_LIGHT_FT,   ft);
}

void writeRearLights(bool rla, bool rlb, bool rra, bool rrb, bool rt) {  
  writePin(PIN_LIGHT_RL_A, rla);
  writePin(PIN_LIGHT_RL_B, rlb);
  writePin(PIN_LIGHT_RR_A, rra);
  writePin(PIN_LIGHT_RR_B, rrb);
  writePin(PIN_LIGHT_RT,   rt);
}

void writeFrontLightsWhite() {
  writeFrontLights(true, false, true, false, true); 
}

void writeFrontLightsRed() {
  writeFrontLights(false, true, false, true, false); 
}

void writeFrontLightsOff() {
  writeFrontLights(false, false, false, false, false); 
} 

void writeRearLightsWhite() {
  writeRearLights(true, false, true, false, true); 
} 
void writeRearLightsRed() {
  writeRearLights(false, true, false, true, false); 
} 

void writeRearLightsOff() {
  writeRearLights(false, false, false, false, false); 
} 

void writeLz1() {
  writeFrontLights(false, false, true, false, false); 
  writeRearLights(false, false, true, false, false);
}

void writeExteriorLights(bool direction, bool lightsActive, bool rearLightsActive, bool lz1Active) {
  if (lightsActive) {
    if (lz1Active) {
      //write LZ1
      writeLz1();
    } else {
      if (direction) {
        //write front lights white
        writeFrontLightsWhite();
        if (rearLightsActive) {
          //write rear lights red
          writeRearLightsRed();
        } else {
          //write rear lights off
          writeRearLightsOff();
        }
      } else {
        //write rear lights white
        writeRearLightsWhite();
        if (rearLightsActive) {
          //Write front lights red
          writeFrontLightsRed();
        } else {
          //write front lights off
          writeFrontLightsOff();
        }
      }
    }
  } else {
    //write all lights off
    writeFrontLightsOff();
    writeRearLightsOff(); 
  }
}

void writeInteriorLights(bool active) {
  writePin(PIN_LIGHT_INTERIOR, active); 
}

void writeHorn(bool active) {
  writePin(PIN_HORN, active); 
}

