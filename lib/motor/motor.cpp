#include "motor.h"

#define MIN_STICK_INPUT 0
#define MAX_STICK_INPUT 1024
#define MIN_PWM_VALUE 0 
#define MAX_PWM_VALUE 255

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
    if (isBlocked) Serial.println("Motor is blocked. Make sure to make a '0' input first"); 
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

bool Motor::setDirectionIfStopped(bool dir) {
    if (isStopped) {
        this->direction = dir;
        return true; 
    }  
    return false; 
}

void Motor::write(uint16_t stickData, bool lowGearIsEnabled) {   
    if (isBlocked && stickData == 0) unblock(); //<- to ensure that the stick has read 0 before writing motor  
    if (!writeIsAllowed()) stickData = 0; 
    Serial.print("Writing ");
    Serial.println(stickData);  
    isStopped = currentPwm == 0;
    uint16_t targetPwm = map(stickData, MIN_STICK_INPUT, MAX_STICK_INPUT, MIN_PWM_VALUE, MAX_PWM_VALUE);
    targetPwm = lowGearIsEnabled ? targetPwm/2 : targetPwm;  
    //ramp
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

bool Motor::getCurrentDirection() {
    return direction; 
}

void Motor::block() {
    isBlocked = true; 
} 

void Motor::unblock() {
    isBlocked = false; 
} 





