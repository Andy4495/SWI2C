/* -----------------------------------------------------------------
   SWI2C Address Scanner
   https://github.com/Andy4495/SWI2C
   MIT License

   08/17/2022 - Andy4495 - Original
*/
/* -----------------------------------------------------------------

   I2C Address Scanner. 
   This example uses several low-level class methods to implement 
   the address scanner.
   - NXP Spec: https://www.nxp.com/docs/en/user-guide/UM10204.pdf
   - Reserved addresses (7-bit): 
     - 0000XXX: 0x00 - 0x07
       - 0x00: general call address (read)/START byte (write)
       - 0x01: CBUS address
       - 0x02: Reserved for different bus format
       - 0x03: Reserved for future purposes
       - 0x04: HS-mode controller code
       - 0x05: HS-mode controller code
       - 0x06: HS-mode controller code
       - 0x07: HS-mode controller code
     - 1111XXX: 0x78 - 0x7F
       - 0x78 - 0x7B: 10-bit target addressing
       - 0x7C - 0x7F: Device ID
   - Valid device addresses: 0x08 - 0x77

   -----------------------------------------------------------------
*/
#include "SWI2C.h"

#define SCL_PIN 9   /// A5 is hardware SCL on Uno
#define SDA_PIN 10  /// A4 is hardware SDA on Uno

#define I2C_START_ADDRESS 0x08
#define I2C_END_ADDRESS   0x77

const unsigned long delayTime = 100;

unsigned long lastmillis;
int valid_address_found = 0; 
uint8_t addr = I2C_START_ADDRESS;
uint8_t valid_addresses[I2C_END_ADDRESS - I2C_START_ADDRESS + 1] = {0};

// Initally setting I2C device address at 0; address will be updated during scan loop below.
SWI2C myScanner = SWI2C(SDA_PIN, SCL_PIN, 0);

void setup() {
  Serial.begin(9600);
  myScanner.begin();
  myScanner.setStretchTimeout(100);
  lastmillis = millis();
  Serial.println("");
  Serial.println("");
  Serial.println("SWI2C Address Scanner.");
  Serial.print("Addresses (7-bit) 0x");
  Serial.print(I2C_START_ADDRESS, HEX);
  Serial.print(" to 0x");
  Serial.print(I2C_END_ADDRESS, HEX);
  Serial.println(" will be scanned.");
  Serial.println("Other addresses are reserved and will not be scanned.");
  Serial.print("Scanning");
}

void loop() {
  unsigned long currentmillis = millis();

  if (currentmillis - lastmillis > delayTime) {
    lastmillis = currentmillis;

    // Use low-level methods. We are only sending the address and no data bytes
    myScanner.setDeviceID(addr);
    myScanner.startBit();
    myScanner.writeAddress(0);
    if (myScanner.checkAckBit() == 0 ) {  // If ACK detected, then valid address found
      if (myScanner.checkStretchTimeout()) { // Unless a clock stretch timeout was detected
        valid_addresses[addr - I2C_START_ADDRESS] = 2;
      } else {     
        valid_addresses[addr - I2C_START_ADDRESS] = 1;
      }
      valid_address_found++;
    }
    myScanner.stopBit(); 

    Serial.print(".");

    if (++addr > I2C_END_ADDRESS) {
        Serial.println("");
        if (valid_address_found) {
            if (valid_address_found == 1) Serial.println("I2C device found.");
            else Serial.println("I2C devices found.");
            Serial.println("----- ----------- ----------");
            Serial.println("7-bit 8-bit Write 8-bit Read");
            Serial.println("----- ----------- ----------");
            for (int i = I2C_START_ADDRESS; i <= I2C_END_ADDRESS; i++ ) {
                if (valid_addresses[i - I2C_START_ADDRESS]) {
                    if (i <= 0x0F) Serial.print(" 0x0");
                    else Serial.print(" 0x");
                    Serial.print(i, HEX);
                    Serial.print("     0x");
                    Serial.print((i<<1), HEX);
                    Serial.print("       0x");
                    Serial.print((i<<1) + 1, HEX);
                    if (valid_addresses[i - I2C_START_ADDRESS] == 2) {
                      Serial.print("    <-- Clock stretch timeout detected. Address may be invalid. ");
                      Serial.print("Check hardware and pin numbers.");
                    }
                    Serial.println("");
                }
            }
        } else {
            Serial.println("No I2C devices found.");
        }
        Serial.println("");
        Serial.println("Reset to start new scan.");
        while (1); // Loop forever
    }
  }
}