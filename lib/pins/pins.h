#ifndef PINS_H
#define PINS_H

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

void initPins();

void writeLights(bool fla, bool flb, bool fra, bool frb, bool ft, bool rla, bool rlb, bool rra, bool rrb, bool rt, bool interior); 
void writeExteriorLights(bool direction, bool lightsActive, bool rearLightsActive, bool lz1Active); 
void writeInteriorLights(bool active); 
void writeHorn(bool active);

#endif