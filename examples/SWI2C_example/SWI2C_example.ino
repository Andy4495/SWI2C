/* -----------------------------------------------------------------
   SWI2C Library Example Sketch
   https://gitlab.com/Andy4495/SWI2C
   MIT License

   03/25/2018 - A.T. - Original
*/
/* -----------------------------------------------------------------

   Periodically reads X and Z coordinates from the ZX sensor

   -----------------------------------------------------------------
*/
#include "SWI2C.h"

#define SCL_PIN 9 
#define SDA_PIN 10 

// Constants
const uint8_t ZX_ADDR = 0x10;  // ZX Sensor I2C address
const uint8_t ZPOS_REG = 0x0A;
const uint8_t XPOS_REG = 0x08;
const unsigned long delayTime = 1000;

uint8_t z_pos, x_pos;
unsigned long lastmilli;

SWI2C myZX = SWI2C(SDA_PIN, SCL_PIN, ZX_ADDR);

void setup() {

  Serial.begin(9600);

  myZX.begin();

  Serial.println("SWI2C. Starting measurements...");

  lastmilli = millis();

}

void loop() {

  if ( (millis() - lastmilli) > delayTime) {

    myZX.read1bFromRegister(ZPOS_REG, &z_pos);
    myZX.read1bFromRegister(XPOS_REG, &x_pos);

    Serial.print("Zpos: ");
    Serial.println(z_pos);
    Serial.print("Xpos: ");
    Serial.println(x_pos);
    Serial.print("Millis: ");
    lastmilli = millis();
    Serial.println(lastmilli);
  }

}
