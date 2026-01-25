//ESP32 Receiver
#include <Arduino.h>
#include <nRF24L01.h> 
#include <RF24.h> 
#include <Wire.h>

#define SAFE_MODE true
#define DEBUG_MODE true

#define RADIO_TIMEOUT_MS 150

#define DEFAULT_ANALOG_VALUE 512
#define DEFAULT_DIGITAL_VALUE 0
#define DEFAULT_STICK_TOLERANCE 256

#define PWM_FREQUENCY 5000
#define PWM_RESOLUTION 8
#define CHANNEL_A 0
#define CHANNEL_B 1

//Pin Declarations
#define PIN_CE  22
#define PIN_CSN 21

#define PIN_PWM_FORWARD 32 
#define PIN_PWM_BACKWARD 33

#define PIN_HORN 15

#define PIN_LIGHT_INTERIOR 5

#define PIN_LIGHT_FL_A 25
#define PIN_LIGHT_FL_B 26
#define PIN_LIGHT_FR_A 27
#define PIN_LIGHT_FR_B 14 
#define PIN_LIGHT_FT   12

#define PIN_LIGHT_RL_A 17
#define PIN_LIGHT_RL_B 16
#define PIN_LIGHT_RR_A 4
#define PIN_LIGHT_RR_B 0
#define PIN_LIGHT_RT   2

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

bool isStopped = true; 
bool goingForward = true; 
bool lxInputIsIgnored = false; 

bool lowGearEnabled = false; 

bool hornActive = false;

bool rxlIsIgnored = false;
bool interiorLightsActive = false;  

bool rxrIsIgnored = false; 
bool lz1Active = false; 

bool rylIsIgnored = false;
bool rearLightsActive = false;  

bool ryuIsIgnored = false;
bool lightsActive = false; 

void serialPrint(int number, int places);
void writeMotor(uint8_t pwm, bool direction, bool lowGearEnabled); 
void writeHorn(bool active);
void writeExteriorLights(bool direction, bool lightsActive, bool rearLightsActive, bool lz1Active); 
void writeInteriorLights(bool active); 
void handleUpperStickInput_512_1024(uint16_t &data, bool &lockVar, bool &outPut); 
void handleLowerStickInput_0_512(uint16_t &data, bool &lockVar, bool &outPut);

void setup() {  
  Serial.begin(115200);
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

  //Setup LEDC API
  uint32_t ch1Freq = ledcSetup(CHANNEL_A, PWM_FREQUENCY, PWM_RESOLUTION);
  uint32_t ch2Freq = ledcSetup(CHANNEL_B, PWM_FREQUENCY, PWM_RESOLUTION); 
  ledcAttachPin(PIN_PWM_FORWARD, CHANNEL_A);
  ledcAttachPin(PIN_PWM_BACKWARD, CHANNEL_B);  
  ledcWrite(CHANNEL_A, 0);
  ledcWrite(CHANNEL_B, 0);
  Serial.println("Set up LEDC channels with frequencies:"); 
  Serial.print("   Channel 1: ");
  Serial.println(ch1Freq); 
  Serial.print("   Channel 2: ");
  Serial.println(ch1Freq); 

  Serial.print("SAFE MODE ");
  Serial.println(SAFE_MODE ? "enabled, all Pin-Interactions disabled" : "disabled, Pin-Interactions enabled"); 

  //Set Pin Modes
  
  Serial.println("Setup - End");

}

void loop() {
  for (int i = 0; i < 255; i ++) {
    writeMotor(i, true, false); 
    Serial.print("white up ");
    serialPrint(i, 3); 
    Serial.println();  
    delay(10); 
  }

  for (int i = 255; i > 0; i--) {
    writeMotor(i, true, false); 
    Serial.print("white down ");
    serialPrint(i, 3); 
    Serial.println();
    delay(10);
  }

    for (int i = 0; i < 255; i ++) {
    writeMotor(i, false, false); 
    Serial.print("red up ");
    serialPrint(i, 3); 
    Serial.println();
    delay(10); 
  }

  for (int i = 255; i > 0; i--) {
    writeMotor(i, false, false); 
    Serial.print("red down ");
    serialPrint(i, 3); 
    Serial.println(); 
    delay(10);
  }

  //LX: Richtung V> R< 
  //LY: Fahrregler 
  //LZ: Horn 

  //RX: Kabinenbeleuchtung  < / FZ1 (1 weiße Leuchte auf Pufferhöhe) > 
  //RY: Umschalten Hecklichter / Licht an/aus 
  //RZ: Hauptschalter?  / Rangierschalter (langsamere V-max)? 

  /*if (radio.available()) {
    radio.read(&data, sizeof(Data_Package));
    lastRadioRxTime = millis(); 
  }
  bool dataIsFresh = (millis() - lastRadioRxTime) <= RADIO_TIMEOUT_MS;

  if (dataIsFresh) {
    int padding = 4; 
    Serial.print("LX: "); 
    serialPrint(data.lx, padding); 
    Serial.print("   |   LY: "); 
    serialPrint(data.ly, padding); 
    Serial.print("   |   LZ: "); 
    serialPrint(data.lz, padding); 
    Serial.print("   |   RX: "); 
    serialPrint(data.rx, padding); 
    Serial.print("   |   RY: "); 
    serialPrint(data.ry, padding); 
    Serial.print("   |   RZ: "); 
    serialPrint(data.rz, padding);
    Serial.println();

    //LY
    uint8_t pwm = map(data.ly, 0, 1024, 0, 255);
    
    //LX
    isStopped = pwm == 0;
    if (isStopped) {
      if (data.lx < 250) {
        goingForward = true; 
        lxInputIsIgnored = true; 
      } else if (data.lx > 1024 - 250) {
        goingForward = false; 
        lxInputIsIgnored = true;
      } else {
        lxInputIsIgnored = false; 
      } 
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
    handleUpperStickInput_512_1024(data.ry, ryuIsIgnored, lightsActive); 

    //RZ
    lowGearEnabled = data.rz; 

    if (!SAFE_MODE) {
      writeMotor(pwm, goingForward, lowGearEnabled); 
      writeExteriorLights(goingForward, lightsActive, rearLightsActive, lz1Active); 
      writeInteriorLights(interiorLightsActive); 
      writeHorn(hornActive);
    }


  } else {
    long millisNow = millis(); 
    Serial.print("Data too old. Last data received at: ");
    Serial.print(lastRadioRxTime);
    Serial.print(" Current timestamp: "); 
    Serial.print(millisNow);
    Serial.print(" ("); 
    Serial.print(millisNow - lastRadioRxTime); 
    Serial.println(" ms ago)");
  }*/
}

void writeForwardPWM(uint8_t pwm) {    
  //Never write 2 opposing PWM pins HIGH at the same time
  ledcWrite(CHANNEL_B, 0); 
  ledcWrite(CHANNEL_A, pwm); 
}

void writeBackwardPWM(uint8_t pwm) {
  //Never write 2 opposing PWM pins HIGH at the same time
  ledcWrite(CHANNEL_A, 0); 
  ledcWrite(CHANNEL_B, pwm); 
}

void writeMotor(uint8_t pwm, bool direction, bool lowGearEnabled) {
  pwm = lowGearEnabled ? pwm / 2 : pwm; 
  if (direction) {
    writeForwardPWM(pwm); 
  } else {
    writeBackwardPWM(pwm); 
  }
} 

void writeHorn(bool active) {
  digitalWrite(PIN_HORN, active ? HIGH : LOW); 
}

void writeInteriorLights(bool active) {
  digitalWrite(PIN_LIGHT_INTERIOR, active ? HIGH : LOW); 
}

void writeExteriorLights(bool direction, bool lightsActive, bool rearLightsActive, bool lz1Active) {
  if (lightsActive) {
    if (lz1Active) {
      //write LZ1
      
    } else {
      if (direction) {
        //write front lights white

        if (rearLightsActive) {
          //write rear lights red
        
        } else {
          //write rear lights off
        
        }
      } else {
        //write rear lights white
        
        if (rearLightsActive) {
          //Write front lights red
        
        } else {
          //write front lights off
        
        }
      }
    }
  } else {
    //write all lights off
  
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

/*-------------------------------------------------------------------------------------------------------------
HELPER FUNCTIONS - HELPER FUNCTIONS - HELPER FUNCTIONS - HELPER FUNCTIONS - HELPER FUNCTIONS - HELPER FUNCTIONS
-------------------------------------------------------------------------------------------------------------*/

void handleLowerStickInput_0_512(uint16_t &data, bool &lockVar, bool &outPut) {
  if (data < DEFAULT_STICK_TOLERANCE) {
    if (!lockVar) {
      outPut = !outPut;
      lockVar = true; 
    }
  } else {
    lockVar = false; 
  }
}

void handleUpperStickInput_512_1024(uint16_t &data, bool &lockVar, bool &outPut) {
  if (data > 1024 - DEFAULT_STICK_TOLERANCE) {
    if (!lockVar) {
      outPut = !outPut;
      lockVar = true; 
    }
  } else {
    lockVar = false; 
  }
}