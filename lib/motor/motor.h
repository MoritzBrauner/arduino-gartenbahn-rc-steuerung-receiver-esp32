#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include <Timer.h>



class Motor {
    private:
        uint8_t forwardPwmPin; 
        uint8_t backwardPwmPin;
 
        uint32_t pwmFrequency; 
        uint8_t pwmResolution; 

        uint8_t channelA;
        uint8_t channelB; 

        Timer timer; 

        uint8_t currentPwm; 
        bool direction; 

        bool isStopped; 

        bool isBlocked = true; 
        bool isSetUp = false; 

        bool writeIsAllowed(); 
        void writeForwardPwm(uint8_t pwm); 
        void writeBackwardPwm(uint8_t pwm); 
    public: 
        Motor(
            uint8_t forwardPwmPin, uint8_t backwardPwmPin, 
            unsigned int pwmFrequency, uint8_t pwmResolution,
            uint8_t channelA, uint8_t channelB, 
            unsigned long timerInterval
        ); 
        void init(); 
        void write(uint16_t stickData, bool lowGearIsEnabled); 
        bool setDirection(bool forward);

};  


#endif