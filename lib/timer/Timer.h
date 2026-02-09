#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>

class Timer {
    private:
        unsigned long interval; 
        unsigned long lastTick;  
    public: 
        Timer(unsigned long ms); 
        bool fires(); 
};

#endif