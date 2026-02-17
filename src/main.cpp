//ESP32 Receiver
#include <Arduino.h>
#include <RF24.h> 
#include <ArduinoOTA.h>
#include <wifimqtt.h>
#include <storage.h> //persistent preferences handling
#include <pins.h> //pin declarations and writing
#include <Timer.h> 
#include <motor.h>
#include "lights.h"
#include "stick.h"

#define SAFE_MODE false
#define DEBUG_MODE false

#define RADIO_TIMEOUT_MS 150

#define DEFAULT_ANALOG_VALUE 512
#define DEFAULT_DIGITAL_VALUE 0
#define DEFAULT_STICK_THRESHOLD 256

//Motor constants 
#define PWM_FREQUENCY 20000
#define PWM_RESOLUTION 8
#define CHANNEL_A 0
#define CHANNEL_B 1
#define MOTOR_TIMER_INTERVAL 5

//Radio setup 
RF24 radio(PIN_CE, PIN_CSN); // CE, CSN
constexpr byte address[6] = "00100";

//Data Package 
struct __attribute__((packed)) Data_Package {
  uint16_t lx;
  uint16_t ly;
  uint8_t  lz;

  uint16_t rx;
  uint16_t ry;
  uint8_t  rz;
};
Data_Package data; 

unsigned long lastRadioRxTime = 0; 

uint8_t currentPwm = 0; 
uint8_t targetPwm = 0; 

bool lowGearEnabled = false; 

bool hornActive = false;

bool rxlIsIgnored = false;
bool interiorLightsActive = false;

bool rxrIsIgnored = false; 
bool lz1Active = false; 

bool rylIsIgnored = false;
bool rearLightsActive = false;  

bool ryuIsIgnored = false;
bool exteriorLightsActive = false; 

//Timer registration
StagedTimer offlineBlinkerTimer(200, 10);
StagedTimer motorIsBlockedTimer(500, 2);
Timer offlineTimer(500); 

//Motor registration
Motor motor(
  PIN_PWM_FORWARD, 
  PIN_PWM_BACKWARD, 
  PWM_FREQUENCY,
  PWM_RESOLUTION,
  CHANNEL_A, 
  CHANNEL_B, 
  MOTOR_TIMER_INTERVAL
);

//Lights
Lights lights(true, LightMode::Off, false);



/*void handleUpperStickInput_512_1024(uint16_t stickData, bool &lockVar, bool &outPut);
void handleLowerStickInput_0_512(uint16_t stickData, bool &lockVar, bool &outPut);*/
//void debugInputs();
void blinkLightsInOfflinePattern();
void blinkLightsInMotorBlockedPattern();
//void serialPrint(int number, int places);

void setup() {  
  Serial.begin(115200);
  Serial.println(); 
  Serial.println("Program: RC Receiver ESP32");
  Serial.println("Setup - Start");

  //Initialize Radio Communication
  const bool radioInitStatus = radio.begin();
  Serial.print("Radio Initialization: ");
  Serial.println(radioInitStatus ? "Success" : "Failed");

  Serial.print("Struct size: ");
  Serial.println(sizeof(Data_Package));

  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();

  Serial.print("SAFE MODE ");
  Serial.println(SAFE_MODE ? "enabled, all Pin-Interactions disabled" : "disabled, Pin-Interactions enabled"); 
  
  //Init motor
  motor.init();

  //Set pin Modes and write LOW
  initPins();
  
  //Set up preferences, init light states 
  const Preferences_Data_Struct savedStates = initPreferences();
  exteriorLightsActive = savedStates.exteriorLightsActive; 
  //interiorLightsActive = savedStates.interiorLightsActive;
  rearLightsActive = savedStates.rearLightsActive; 
  lz1Active = savedStates.lz1Active; 

  if (!SAFE_MODE) {
    //writeInteriorLights(interiorLightsActive);
    writeExteriorLights(motor.getCurrentDirection(), exteriorLightsActive, rearLightsActive, lz1Active);
  }

  //Set up OTA 
  connectAP(); 
  delay(500); 
  ArduinoOTA.begin(); 

  Serial.println("Setup - End");
}

//void determineAndApplyLightStates(uint16_t dataRX, uint16_t dataRY);



// Loop frequency measurement
unsigned long lastLoopMeasureTime = 0;
unsigned long loopCounter = 0;
void printLoopFrequency() {
  loopCounter++;
  unsigned long currentTime = millis();
  if (currentTime - lastLoopMeasureTime >= 1000) {
    Serial.print("Loops/s: ");
    Serial.println(loopCounter);

    loopCounter = 0;
    lastLoopMeasureTime = currentTime;
  }
}

RightStickPathDetector rightStick(DEFAULT_STICK_THRESHOLD);

void loop() {
  printLoopFrequency();
  //LX: Direction V> R<
  //LY: Throttle
  //LZ: Horn 

  //RX: Cabin Lights < / FZ1 (1 white light fr and rl) >
  //RY: toggle rear lights / toggle lights
  //RZ: low gear (slower V-max)?

  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package));
    lastRadioRxTime = millis(); 
  }
  bool dataIsFresh = (millis() - lastRadioRxTime) <= RADIO_TIMEOUT_MS;

  if (!dataIsFresh) {
    ArduinoOTA.handle();
    motor.block();
    motor.write(data.ly, lowGearEnabled); //<- write nevertheless, blocking is handled internally
    blinkLightsInOfflinePattern(); //Blink front lights in a circular pattern
    writeHorn(false);
    return;
  }

  if (motor.getBlocked() && data.ly == 0) {
    motor.unblock();
  }

  //Rest vom Code
  //LX
  if (data.lx < 250) {
    motor.setDirectionIfStopped(true);
  } else if (data.lx > 1024 - 250) {
    motor.setDirectionIfStopped(false);
  }

  //LZ
  hornActive = data.lz;

  //RX - Left Side (>512)

  //lights.setCabLightStatus(interiorLightsActive);

  //RX - Right Side (<512)
  /*handleLowerStickInput_0_512(data.rx, rxrIsIgnored, lz1Active);

  //RY - Lower (<512)
  handleLowerStickInput_0_512(data.ry, rylIsIgnored, rearLightsActive);

  //RY - Upper (>512)
  handleUpperStickInput_512_1024(data.ry, ryuIsIgnored, exteriorLightsActive); */

  //determineAndApplyLightStates(data.rx, data.ry);
  //lights.setDirection(motor.getCurrentDirection());
  auto result = rightStick.update(data.rx, data.ry);
  if (result.pathDetected) {
    switch (result.path) {
      case RightStickPathDetector::Path::Up_Left:
        lights.setMode(LightMode::FrontOnly);
        break;

      case RightStickPathDetector::Path::Up_Right:
        lights.setMode(LightMode::FrontAndRear);
        break;

        case RightStickPathDetector::Path::Right_Up:
        //lights.setMode(LightMode::Lz1);
        lights.setDirection(true);
        break;

      case RightStickPathDetector::Path::Right_Down:
        //lights.setMode(LightMode::Off);
        lights.setDirection(false);
        break;

      case RightStickPathDetector::Path::Down_Left:
        lights.setMode(LightMode::Off);
        break;

      case RightStickPathDetector::Path::Down_Right:
        lights.setMode(LightMode::Lz1);
        break;

      case RightStickPathDetector::Path::Left_Up:
        //lights.setMode(LightMode::Lz1);
        lights.setCabLightStatus(true);
        break;

      case RightStickPathDetector::Path::Left_Down:
        //lights.setMode(LightMode::Lz1);
        lights.setCabLightStatus(false);
        break;

      default:
        break;
    }
  }
  /*Serial.print(data.rx);
  Serial.print("   |  ");
  Serial.print(data.ry);
  Serial.print("   |  ");
  Serial.print(result.pathDetected);
  Serial.print("   |  ");
  Serial.print(static_cast<int>(result.path));
  Serial.print("   |  ");
  Serial.println(static_cast<int>(lights.getMode()));*/




  //RZ
  lowGearEnabled = !data.rz;

  /*if(DEBUG_MODE) {
    debugInputs();
  }*/
  //Store preferences
  //storeLightStates(Preferences_Data_Struct({interiorLightsActive, exteriorLightsActive, rearLightsActive, lz1Active}));

  if (!SAFE_MODE) {
    const bool writeSucceeded = motor.write(data.ly, lowGearEnabled);

    if (writeSucceeded) {
      lights.write();
    } else {
      blinkLightsInMotorBlockedPattern();
    }
    //writeExteriorLights(motor.getCurrentDirection(), exteriorLightsActive, rearLightsActive, lz1Active);
    //writeInteriorLights(interiorLightsActive);
    writeHorn(hornActive);
  }
}

/*bool isIgnored = false;
void determineAndApplyLightStates(uint16_t dataRX, uint16_t dataRY) {
  if (dataRX > 1024 - DEFAULT_STICK_THRESHOLD) {
    if (dataRY > 1024 - DEFAULT_STICK_THRESHOLD) {
      lights.setCabLightStatus(true);
    }
    if (dataRY < DEFAULT_STICK_THRESHOLD) {
      lights.setCabLightStatus(false);
    }
  }

  if (dataRY > 1024 - DEFAULT_STICK_THRESHOLD && dataRX < 1024 - DEFAULT_STICK_THRESHOLD) {
    if (!isIgnored) {
      lights.setMode(LightMode::FrontOnly);
      if (dataRX < DEFAULT_STICK_THRESHOLD) {
        lights.setMode(LightMode::FrontAndRear);
        isIgnored = true;
      }
    }
  } else {
    isIgnored = false;
  }

  if (dataRX < DEFAULT_STICK_THRESHOLD && dataRY < 1024 - DEFAULT_STICK_THRESHOLD) {
    lights.setMode(LightMode::Lz1);
  }

  if (dataRY < DEFAULT_STICK_THRESHOLD) {
    lights.setMode(LightMode::Off);
  }
}*/

/*
void handleLowerStickInput_0_512(const uint16_t stickData, bool &lockVar, bool &outPut) {
  if (stickData < DEFAULT_STICK_THRESHOLD) {
    if (!lockVar) {
      outPut = !outPut;
      lockVar = true; 
    }
  } else {
    lockVar = false; 
  }
}

void handleUpperStickInput_512_1024(const uint16_t stickData, bool &lockVar, bool &outPut) {
  if (stickData > 1024 - DEFAULT_STICK_THRESHOLD) {
    if (!lockVar) {
      outPut = !outPut;
      lockVar = true; 
    }
  } else {
    lockVar = false; 
  }
}*/

/*void debugInputs() {
  Serial.print("lx: "); 
  serialPrint(data.lx, 4);
  Serial.print("   |   going forward: "); 
  Serial.print(motor.getCurrentDirection());
  Serial.print("   |   low gear: "); 
  Serial.print(lowGearEnabled);
  Serial.print("   |   ly: "); 
  serialPrint(data.ly, 4);
  Serial.print("   |   pwm: "); 
  uint8_t pwm = map(data.ly, 0, 1024, 0, 255);
  serialPrint(pwm, 3);
  Serial.println();
}*/

void blinkLightsInOfflinePattern() {
  switch (offlineBlinkerTimer.getStage()) {
  case 1:
    lights.writeExteriorLights  (
      1, 0,
      0, 0, 
      0,
      0, 0, 
      0, 0, 
      0,
      0
    );  
    break;
  case 2: 
    lights.writeExteriorLights(
      0, 0,
      1, 0, 
      0,
      0, 0, 
      0, 0, 
      0,
      0
    );  
  break;
  case 3:
    lights.writeExteriorLights(
   0, 0,
      0, 0, 
      1,
      0, 0, 
      0, 0, 
      0, 
      0
    ); 
    break;
  case 4: 
    lights.writeExteriorLights(
      0, 0,
      0, 0, 
      0,
      0, 0, 
      0, 0, 
      0, 
      1
    ); 
    break;
  case 5: 
    lights.writeExteriorLights(
      0, 0,
      0, 0, 
      0,
      0, 0, 
      0, 0, 
      1, 
      0
    ); 
    break;
  case 6:
    lights.writeExteriorLights(
      0, 0,
      0, 0, 
      0,
      0, 0, 
      1, 0, 
      0, 
      0
    ); 
    break;
    case 7: 
    lights.writeExteriorLights(
      0, 0,
      0, 0, 
      0,
      1, 0, 
      0, 0, 
      0,
      0
    ); 
    break;
    case 8: 
    lights.writeExteriorLights(
      0, 0,
      0, 0, 
      0,
      0, 0, 
      0, 0, 
      1, 
      0
    );
    break;
    case 9: 
    lights.writeExteriorLights(
      0, 0,
      0, 0, 
      0,
      0, 0, 
      0, 0, 
      0, 
      1
    ); 
    break;
    case 10: 
    lights.writeExteriorLights(
      0, 0,
      0, 0, 
      1,
      0, 0, 
      0, 0, 
      0, 
      0
    ); 
    break;
  default: ;
  }
}

void blinkLightsInMotorBlockedPattern() {
  switch (motorIsBlockedTimer.getStage()) {
    case 1:
      lights.writeExteriorLights(
        0, 1,
        0, 1,
        0,
        0, 1,
        0, 1,
        0,
        0);
      break;
    case 2:
      lights.writeExteriorLights(
        0, 0,
        0, 0,
        0,
        0, 0,
        0, 0,
        0,
        0);
      break;
    default: ;
  }
}