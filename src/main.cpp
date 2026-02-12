//ESP32 Receiver
#include <Arduino.h>
#include <nRF24L01.h> 
#include <RF24.h> 
#include <ArduinoOTA.h>
#include <WiFiUdp.h>
#include <wifimqtt.h>
#include <storage.h> //persistent preferences handling
#include <pins.h> //pin declarations and writing
#include <Timer.h> 
#include <motor.h> 

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
const byte address[6] = "00100";

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

void handleUpperStickInput_512_1024(uint16_t data, bool &lockVar, bool &outPut); 
void handleLowerStickInput_0_512(uint16_t data, bool &lockVar, bool &outPut);
void debugInputs();
void blinkLightsInFunnyPattern(); 
void serialPrint(int number, int places);

void setup() {  
  Serial.begin(115200);
  Serial.println(); 
  Serial.println("Program: RC Receiver ESP32");
  Serial.println("Setup - Start");

  //Initialize Radio Communication
  bool radioInitStatus = radio.begin();
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
  Preferences_Data_Struct savedStates = initPreferences();
  exteriorLightsActive = savedStates.exteriorLightsActive; 
  interiorLightsActive = savedStates.interiorLightsActive; 
  rearLightsActive = savedStates.rearLightsActive; 
  lz1Active = savedStates.lz1Active; 

  if (!SAFE_MODE) {
    writeInteriorLights(interiorLightsActive); 
    writeExteriorLights(motor.getCurrentDirection(), exteriorLightsActive, rearLightsActive, lz1Active);
  }

  //Set up OTA 
  connectAP(); 
  delay(500); 
  ArduinoOTA.begin(); 

  Serial.println("Setup - End");
}

void loop() {
  //LX: Richtung V> R< 
  //LY: Fahrregler 
  //LZ: Horn 

  //RX: Kabinenbeleuchtung  < / FZ1 (1 weiße Leuchte auf Pufferhöhe) > 
  //RY: Umschalten Hecklichter / Licht an/aus 
  //RZ: Hauptschalter?  / Rangierschalter (langsamere V-max)?
  ArduinoOTA.handle(); 

  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package));
    lastRadioRxTime = millis(); 
  }
  bool dataIsFresh = (millis() - lastRadioRxTime) <= RADIO_TIMEOUT_MS;

  if (dataIsFresh) {
    
    //LX
    if (data.lx < 250) {
      motor.setDirectionIfStopped(true);   
    } else if (data.lx > 1024 - 250) {
      motor.setDirectionIfStopped(false); 
    }
    
    //LZ
    hornActive = data.lz;  

    //RX - Left Side (>512)
    handleUpperStickInput_512_1024(data.rx, rxlIsIgnored, interiorLightsActive); 

    //RX - Right Side (<512) 
    handleLowerStickInput_0_512(data.rx, rxrIsIgnored, lz1Active); 

    //RY - Lower (<512)
    handleLowerStickInput_0_512(data.ry, rylIsIgnored, rearLightsActive); 
    
    //RY - Upper (>512)
    handleUpperStickInput_512_1024(data.ry, ryuIsIgnored, exteriorLightsActive); 

    //RZ
    lowGearEnabled = !data.rz; 

    if(DEBUG_MODE) {
      debugInputs(); 
    }

    //Store preferences
    storeLightStates(Preferences_Data_Struct({interiorLightsActive, exteriorLightsActive, rearLightsActive, lz1Active})); 

    if (!SAFE_MODE) {
      motor.write(data.ly, lowGearEnabled); 
      writeExteriorLights(motor.getCurrentDirection(), exteriorLightsActive, rearLightsActive, lz1Active); 
      writeInteriorLights(interiorLightsActive); 
      writeHorn(hornActive);  
    }

  } else {
    motor.block(); 
    motor.write(data.ly, lowGearEnabled); //<- write nevertheless, blocking is handled internally 
    //Blink front lights in circular pattern
    blinkLightsInFunnyPattern();     
    writeExteriorLights(0, 0, 0, 0);
    writeHorn(0);
  }
}

void serialPrint(int number, int places) {
  char buffer[16];
  //Format-String dynamisch bauen, z.B. "%04d", "%06d", ...
  char format[8];
  snprintf(format, sizeof(format), "%%0%dd", places);
  snprintf(buffer, sizeof(buffer), format, number);
  Serial.print(buffer);
}

void handleLowerStickInput_0_512(uint16_t data, bool &lockVar, bool &outPut) {
  if (data < DEFAULT_STICK_THRESHOLD) {
    if (!lockVar) {
      outPut = !outPut;
      lockVar = true; 
    }
  } else {
    lockVar = false; 
  }
}

void handleUpperStickInput_512_1024(uint16_t data, bool &lockVar, bool &outPut) {
  if (data > 1024 - DEFAULT_STICK_THRESHOLD) {
    if (!lockVar) {
      outPut = !outPut;
      lockVar = true; 
    }
  } else {
    lockVar = false; 
  }
}

void debugInputs() {
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
}

void blinkLightsInFunnyPattern() {
  Serial.println(offlineBlinkerTimer.getStage());
  switch (offlineBlinkerTimer.getStage()) {
  case 1: 
    writeLights(
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
    writeLights(
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
    writeLights(
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
    writeLights(
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
    writeLights(
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
    writeLights(
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
    writeLights(
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
    writeLights(
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
    writeLights(
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
    writeLights(
      0, 0,
      0, 0, 
      1,
      0, 0, 
      0, 0, 
      0, 
      0
    ); 
    break;
  }
}