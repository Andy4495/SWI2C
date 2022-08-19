/* -----------------------------------------------------------------
   SWI2C Library Example Sketch - PCF8574
   https://github.com/Andy4495/SWI2C
   MIT License

   08/17/2022 - Andy4495 - Original
*/
/* -----------------------------------------------------------------

   This example uses the "read1bFromDevice()" and "readBytesFromDevice()"
   class methods to demonstrate the use case of I2C devices that do 
   not have register addresses.

   -----------------------------------------------------------------
*/
#include "SWI2C.h"

#define SCL_PIN 9
#define SDA_PIN 10

// Configure the address of the device here.
const uint8_t PCF_ADDR = 0x38;       // PCF8574A: assumes A2, A1, A0 tied to GND (0)
//const uint8_t PCF_ADDR = 0x20;       // PCF8574: assumes A2, A1, A0 tied to GND (0)

const unsigned long delayTime = 1000;

unsigned long lastmillis;

SWI2C myPCF = SWI2C(SDA_PIN, SCL_PIN, PCF_ADDR);

void setup() {
  Serial.begin(9600);
  myPCF.begin();
  Serial.println("SWI2C PCF8574 Example.");
  lastmillis = millis();
}

void loop() {
  uint8_t data;
  uint8_t array[1];
  unsigned long currentmillis = millis();
  int retval;

  if (currentmillis - lastmillis > delayTime) {
    lastmillis = currentmillis;

    retval = myPCF.read1bFromDevice(&data);
    if (retval == 0) Serial.println("NACK received. Data is invalid.");
    else {
      Serial.print("Reading PCF8574 input port value: ");
      Serial.println(data);
    }
    retval = myPCF.readBytesFromDevice(array, 1);
    if (retval == 0) Serial.println("NACK received. Data is invalid.");
    else {
      Serial.print("Reading PCF8574 input port (using alternate method): ");
      Serial.println(array[0]);
    }
    Serial.println("");

  }
}