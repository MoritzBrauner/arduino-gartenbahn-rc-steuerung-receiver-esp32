#include "Timer.h"

Timer::Timer(unsigned long ms) {
    interval = ms; 
    lastTick = millis(); 
}

bool Timer::fires() {
    unsigned long now = millis(); 
    if (now - lastTick >= interval) {
        lastTick = now;
        return true;  
    }
    return false; 
}