/* -----------------------------------------------------------------
   SWI2C Library
   https://github.com/Andy4495/SWI2C
   MIT License

   03/25/2018 - A.T. - Original
   07/04/2018 - A.T. - Add timeout for clock stretching
   10/17/2018 - A.T. - Add 2-byte write method
   09/16/2021 - A.T. - Add support for Repeated Start (Issue #5)
   07/27/2022 - Andy4495 - Support single-register devices (Issue #3)
*/

#ifndef SWI2C_H
#define SWI2C_H

#include "Arduino.h"

class SWI2C {
public:
  SWI2C(uint8_t sda_pin, uint8_t scl_pin, uint8_t deviceID);
  void begin();
  int write1bToRegister(int regAddress, uint8_t data, bool sendStopBit = true);
  int write2bToRegister(int regAddress, uint16_t data, bool sendStopBit = true);
  int write2bToRegisterMSBFirst(int regAddress, uint16_t data, bool sendStopBit = true) ;
  int writeToRegister(int regAddress, uint8_t data, bool sendStopBit = true);
  int writeBytesToRegister(int regAddress, uint8_t* data, uint8_t count, bool sendStopBit = true);
  int write1bToDevice(uint8_t data, bool sendStopBit = true);
  int writeBytesToDevice(uint8_t* data, uint8_t count, bool sendStopBit = true);
  int read1bFromRegister(int regAddress, uint8_t* data, bool sendStopBit = true);
  int read2bFromRegister(int regAddress, uint16_t* data, bool sendStopBit = true);
  int read2bFromRegisterMSBFirst(int regAddress, uint16_t* data, bool sendStopBit = true);
  int readBytesFromRegister(int regAddress, uint8_t* data, uint8_t count, bool sendStopBit = true);
  int read1bFromDevice(uint8_t* data, bool sendStopBit = true);
  int readBytesFromDevice(uint8_t* data, uint8_t count, bool sendStopBit = true);
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
  unsigned long getStretchTimeout();
  void setStretchTimeout(unsigned long t);
  int checkStretchTimeout();
  uint8_t getDeviceID();
  void setDeviceID(uint8_t deviceid);
private:
  enum {DEFAULT_STRETCH_TIMEOUT = 500UL};   // ms timeout waiting for device to release SCL line
  uint8_t _deviceID;
  uint8_t _scl_pin;
  uint8_t _sda_pin;
  unsigned long _stretch_timeout_delay;
  int _stretch_timeout_error;
};

#endif
