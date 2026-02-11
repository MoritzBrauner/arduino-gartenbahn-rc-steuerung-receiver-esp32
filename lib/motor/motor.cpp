#include "motor.h"

Motor::Motor(
    uint8_t forwardPwmPin, uint8_t backwardPwmPin, 
    uint32_t pwmFrequency, uint8_t pwmResolution,
    uint8_t channelA, uint8_t channelB,
    unsigned long timerInterval
) : forwardPwmPin(forwardPwmPin),
  backwardPwmPin(backwardPwmPin),
  pwmFrequency(pwmFrequency),
  pwmResolution(pwmResolution),
  channelA(channelA),
  channelB(channelB),
  timer(timerInterval),               

  currentPwm(0),
  direction(true),
  isStopped(true),
  isBlocked(true),
  isSetUp(false)
{

}

bool Motor::writeIsAllowed() {
    if (isBlocked) Serial.println("Motor is blocked."); 
    if (!isSetUp) Serial.println("Motor is not set up yet."); 
    bool allowed = (!isBlocked && isSetUp); 
    return allowed; 
}

void Motor::init() {
    Serial.println("Initializing Motor: "); 
    uint32_t ch1Freq = ledcSetup(channelA, pwmFrequency, pwmResolution);
    uint32_t ch2Freq = ledcSetup(channelB, pwmFrequency, pwmResolution); 
    ledcAttachPin(forwardPwmPin, channelA);
    ledcAttachPin(backwardPwmPin, channelB);  
    ledcWrite(channelA, 0);
    ledcWrite(channelB, 0);
    Serial.println("Set up LEDC channels with frequencies:"); 
    Serial.print("   Channel 1: ");
    Serial.println(ch1Freq); 
    Serial.print("   Channel 2: ");
    Serial.println(ch2Freq);
    isSetUp = true; 
    Serial.println("Motor initialized successfully"); 
}

bool Motor::setDirection(bool dir) {
    if (isStopped) {
        this->direction = dir;
        return true; 
    }  
    return false; 
}

void Motor::write(uint16_t stickData, bool lowGearIsEnabled) {    
    isStopped = currentPwm == 0;
    uint16_t targetPwm = map(stickData, 0, 1024, 0, 255);
    targetPwm = lowGearIsEnabled ? targetPwm/2 : targetPwm;  
    if (timer.fires()) {
        if (targetPwm > currentPwm) currentPwm++; 
        if (targetPwm < currentPwm) currentPwm--; 
    }
    if (direction) {
        writeForwardPwm(currentPwm);
    } else {
        writeBackwardPwm(currentPwm); 
    }
}

void Motor::writeForwardPwm(uint8_t pwm) {
    //Never write 2 opposing PWM pins HIGH at the same time
    ledcWrite(channelB, 0); 
    ledcWrite(channelA, pwm); 
} 

void Motor::writeBackwardPwm(uint8_t pwm) {
    //Never write 2 opposing PWM pins HIGH at the same time
    ledcWrite(channelA, 0); 
    ledcWrite(channelB, pwm); 
}





