#include <Arduino.h>
#include <nRF24L01.h> 
#include <RF24.h>

#define SAFE_MODE true

#define DEFAULT_ANALOG_VALUE 512
#define DEFAULT_DIGITAL_VALUE 0


//Pin Declarations
#define PIN_CE  22
#define PIN_CSN 21

//Radio setup 
RF24 radio(PIN_CE, PIN_CSN); // CE, CSN
const byte address[6] = "00100";

//Data Package 
struct Data_Package {
  uint16_t lx;
  uint16_t ly;
  uint8_t  lz;

  uint16_t rx;
  uint16_t ry;
  uint8_t  rz;
};
Data_Package data; 

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

  //Set Pin Modes 

  Serial.print("SAFE MODE ");
  Serial.println(SAFE_MODE ? "enabled, all Pin-Interactions disabled" : "disabled, Pin-Interactions enabled"); 
  
  Serial.println("Setup - End");

}
void serialPrint(int number, int places);
void loop() {
  //Check radio availability, read if available, else make a fallback to default values
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package));
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
  } else {
    //data.lx = DEFAULT_ANALOG_VALUE; 
    //data.ly = DEFAULT_ANALOG_VALUE; 
    //data.lz = DEFAULT_DIGITAL_VALUE; 
    //data.rx = DEFAULT_ANALOG_VALUE; 
    //data.ry = DEFAULT_ANALOG_VALUE; 
    //data.rz = DEFAULT_DIGITAL_VALUE; 
  }



}

void serialPrint(int number, int places) {
  //char buffer[16];
  // Format-String dynamisch bauen, z.B. "%04d", "%06d", ...
  //char format[8];
  //snprintf(format, sizeof(format), "%%0%dd", places);
  //snprintf(buffer, sizeof(buffer), format, number);
  Serial.print(number);
}