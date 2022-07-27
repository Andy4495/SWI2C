/* -----------------------------------------------------------------
   SWI2C Library Example Sketch
   https://github.com/Andy4495/SWI2C
   MIT License

   07/27/2022 - Andy4495 - Original
*/
/* -----------------------------------------------------------------

   Configure PCA9548A 8-chan I2C switch
   Also uses SparkFun ZX Sensor as I2C device on the switched side

   -----------------------------------------------------------------
*/
#include "SWI2C.h"

#define SCL_PIN 9
#define SDA_PIN 10

// Constants
const uint8_t PCA_ADDR = 0x70;       // Assume A2, A1, A0 tied to GND (0)
const uint8_t ZX_BUS_SELECT = 0x08;  // Corresponds to PCA channel 3
const uint8_t ZX_ADDR = 0x10;        // ZX Sensor I2C address, connected to PCA channel 3
const uint8_t ZPOS_REG = 0x0A;
const uint8_t XPOS_REG = 0x08;
const unsigned long delayTime = 1000;

uint8_t z_pos, x_pos;
unsigned long lastmilli;

SWI2C myPCA = SWI2C(SDA_PIN, SCL_PIN, PCA_ADDR);
SWI2C myZX = SWI2C(SDA_PIN, SCL_PIN, ZX_ADDR);

void setup() {

  Serial.begin(9600);

  myPCA.begin();
  myZX.begin();   // This is not really necessary, since both devices are on same I2C bus pins

  Serial.println("SWI2C. Starting measurements...");

  lastmilli = millis();

}

void loop() {
  uint8_t data[1];

  data[0] = ZX_BUS_SELECT;

  if ( (millis() - lastmilli) > delayTime) {

    Serial.print("PCA config register: "); 
    Serial.println(myPCA.readBytesFromDevice(data,1));

    Serial.println("Setting PCA config register.");
    myPCA.writeBytesToDevice(data, 1); // Enable channel 3 on PCA
    Serial.print("PCA config register: "); 
    Serial.println(myPCA.readBytesFromDevice(data,1));

    myZX.read1bFromRegister(ZPOS_REG, &z_pos);
    myZX.read1bFromRegister(XPOS_REG, &x_pos);

    Serial.print("Zpos: ");
    Serial.println(z_pos);
    Serial.print("Xpos: ");
    Serial.println(x_pos);
    Serial.print("Millis: ");
    lastmilli = millis();
    Serial.println(lastmilli);

    Serial.print("PCA config register: "); 
    Serial.println(myPCA.readBytesFromDevice(data,1));

    Serial.println("Clearing PCA control register.");
    data[0] = 0; 
    myPCA.writeBytesToDevice(data,1);
  }
}
