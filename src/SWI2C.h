/* -----------------------------------------------------------------
   SWI2C Library
   https://gitlab.com/Andy4495/SWI2C
   MIT License

   03/25/2018 - A.T. - Original
*/

#ifndef SWI2C_H
#define SWI2C_H

#include "Arduino.h"

class SWI2C {
public:
  SWI2C(uint8_t sda_pin, uint8_t scl_pin, uint8_t deviceID);
  void begin();
  int writeToRegister(int regAddress, uint8_t data);
  int read1bFromRegister(int regAddress, uint8_t* data);
  int read2bFromRegister(int regAddress, uint16_t* data);
  void sclHi();
  void sclLo();
  void sdaHi();
  void sdaLo();
  void startBit();
  void writeAddress(int r_w);
  uint8_t checkAckBit();
  void writeAck();
  void writeRegister(int regAddress);
  void stopBit();
  uint8_t read1Byte();
  uint16_t read2Byte();
  void writeByte(int data);
private:
  uint8_t _deviceID;
  uint8_t _scl_pin;
  uint8_t _sda_pin;
};

#endif
