/* -----------------------------------------------------------------
   SWI2C Library
   https://github.com/Andy4495/SWI2C
   MIT License

   03/25/2018 - A.T. - Original
   07/04/2018 - A.T. - Add timeout for clock stretching
   10/17/2018 - A.T. - Add 2-byte write method and methods to swap MSB/LSB
                       for 2-byte reads and writes.
   08/25/2021 - A.T. - Add error checking for NACKs received from device (Issue #4)
   09/16/2021 - A.T. - Add support for Repeated Start (Issue #5)
   06/15/2022 - A.T. - Properly handle NACK from device (Issue #6)
   07/27/2022 - Andy4495 - Support single-register devices (Issue #3)
*/

#include "SWI2C.h"

SWI2C::SWI2C(uint8_t sda_pin, uint8_t scl_pin, uint8_t deviceID) {
  _sda_pin = sda_pin;
  _scl_pin = scl_pin;
  _deviceID = deviceID;
  _stretch_timeout_delay = DEFAULT_STRETCH_TIMEOUT;
  _stretch_timeout_error = 0;
}

void SWI2C::begin() {
  // Set I2C pins low in setup. With pinMode(OUTPUT), line goes low
  //   With pinMode(INPUT), then pull-up resistor pulls line high
  digitalWrite(_scl_pin, LOW);
  digitalWrite(_sda_pin, LOW);
  pinMode(_scl_pin, INPUT);
  pinMode(_sda_pin, INPUT);
}

void SWI2C::sclHi() {
  unsigned long startTimer;

 // I2C pull-up resistor pulls SCL high in INPUT (Hi-Z) mode
  pinMode(_scl_pin, INPUT);

  // Check to make sure SCL pin has actually gone high before returning
  // Device may be pulling SCL low to delay transfer (clock stretching)

  if ( _stretch_timeout_delay == 0) { // If timeout delay == 0, then wait indefinitely for SCL to go high
    while (digitalRead(_scl_pin) == LOW) ;  // Empty statement: keep looping until not LOW
  }
  else {
    // If SCL is not pulled high within a timeout period, then return anyway
    // to avoid locking up the processor.
    startTimer = millis();
    while (millis() - startTimer < _stretch_timeout_delay) {
      if (digitalRead(_scl_pin) == HIGH) return; // SCL high before timeout, return without error
    }
    // SCL did not go high within the timeout, so set error and return anyway.
    _stretch_timeout_error = 1;
  }
}

void SWI2C::sclLo() {
  pinMode(_scl_pin, OUTPUT);  // _scl_pin set LOW in constructor
}

void SWI2C::sdaHi() {
  pinMode(_sda_pin, INPUT);  // I2C pull-up resistor pulls signal high
}

void SWI2C::sdaLo() {
  pinMode(_sda_pin, OUTPUT); // _sda_pin set LOW in constructor
}

void SWI2C::startBit() {  // Assume SDA already HIGH
  sclHi();
  sdaLo();
  sclLo();
}

unsigned long SWI2C::getStretchTimeout() {
  return _stretch_timeout_delay;
}

void SWI2C::setStretchTimeout(unsigned long t) {
  _stretch_timeout_delay = t;
}

int SWI2C::checkStretchTimeout(){
  int retval;
  retval = _stretch_timeout_error;
  // Clear the value upon reading it.
  _stretch_timeout_error = 0;
  return retval;
}

uint8_t SWI2C::getDeviceID() {
  return _deviceID;
}

void SWI2C::setDeviceID(uint8_t deviceid) {
  // deviceid is the 7-bit I2C address
  _deviceID = deviceid;
}

void SWI2C::writeAddress(int r_w) {  // Assume SCL, SDA already LOW from startBit()
  if (_deviceID & 0x40) sdaHi();     // bit 6
  else sdaLo();
  sclHi();
  sclLo();
  if (_deviceID & 0x20) sdaHi();     // bit 5
  else sdaLo();
  sclHi();
  sclLo();
  if (_deviceID & 0x10) sdaHi();     // bit 4
  else sdaLo();
  sclHi();
  sclLo();
  if (_deviceID & 0x08) sdaHi();     // bit 3
  else sdaLo();
  sclHi();
  sclLo();
  if (_deviceID & 0x04) sdaHi();     // bit 2
  else sdaLo();
  sclHi();
  sclLo();
  if (_deviceID & 0x02) sdaHi();     // bit 1
  else sdaLo();
  sclHi();
  sclLo();
  if (_deviceID & 0x01) sdaHi();     // bit 0
  else sdaLo();
  sclHi();
  sclLo();
  if (r_w == 1) sdaHi();         // R/W bit
  else sdaLo();
  sclHi();
  sclLo();
  sdaHi();    // Release the data line for ACK signal from device
}

uint8_t SWI2C::checkAckBit() { // Can also be used by controller to send NACK after last byte is read from device
  uint8_t ack;
  sdaHi();    // Release data line. This will cause a NACK from controller when reading bytes.
  sclHi();
  ack = digitalRead(_sda_pin);
  sclLo();
  return ack;
}

void SWI2C::writeAck() {  // Used by controller to ACK to device bewteen multi-byte reads
  sdaLo();
  sclHi();
  sclLo();
  sdaHi();  // Release the data line
}

void SWI2C::writeRegister(int reg_id) {
  writeByte(reg_id);
}

void SWI2C::stopBit() {  // Assume SCK is already LOW (from ack or data write)
  sdaLo();
  sclHi();
  sdaHi();
}

uint8_t SWI2C::read1Byte() {
  uint8_t value = 0;
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x80;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x40;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x20;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x10;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x08;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x04;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x02;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x01;
  sclLo();

  return value;
}

uint16_t SWI2C::read2Byte() {
  // Assumes LEAST significant BYTE is transferred first
  uint16_t value = 0;
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x80;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x40;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x20;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x10;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x08;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x04;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x02;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x01;
  sclLo();
  writeAck();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x8000;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x4000;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x2000;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x1000;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x0800;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x0400;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x0200;
  sclLo();
  sclHi();
  if (digitalRead(_sda_pin) == 1) value += 0x0100;
  sclLo();
  return value;
}

void SWI2C::writeByte(int data) {
  if (data & 0x80) sdaHi();     // bit 7
  else sdaLo();
  sclHi();
  sclLo();
  if (data & 0x40) sdaHi();     // bit 6
  else sdaLo();
  sclHi();
  sclLo();
  if (data & 0x20) sdaHi();     // bit 5
  else sdaLo();
  sclHi();
  sclLo();
  if (data & 0x10) sdaHi();     // bit 4
  else sdaLo();
  sclHi();
  sclLo();
  if (data & 0x08) sdaHi();     // bit 3
  else sdaLo();
  sclHi();
  sclLo();
  if (data & 0x04) sdaHi();     // bit 2
  else sdaLo();
  sclHi();
  sclLo();
  if (data & 0x02) sdaHi();     // bit 1
  else sdaLo();
  sclHi();
  sclLo();
  if (data & 0x01) sdaHi();     // bit 0
  else sdaLo();
  sclHi();
  sclLo();
  sdaHi();  // Release the data line for ACK from device
}

// New method for name consistency, keep the old method for backward compatibility
int SWI2C::write1bToRegister(int regAddress, uint8_t data, bool sendStopBit) {
  return writeToRegister(regAddress, data, sendStopBit);
}

int SWI2C::write2bToRegister(int regAddress, uint16_t data, bool sendStopBit) {
  // LEAST significant BYTE is transferred first
  // If device is expecting MSB first, use write2bToRegisterMSBFirst()

  startBit();
  writeAddress(0);
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  writeRegister(regAddress);
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  writeByte(data & 0xFF); // LSB
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  writeByte(data >> 8);   // MSB
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  if (sendStopBit) stopBit();
  return 1;  // Return 1 if no NACKs
}

int SWI2C::write2bToRegisterMSBFirst(int regAddress, uint16_t data, bool sendStopBit) {
  // Swaps MSB and LSB
  return write2bToRegister(regAddress, ((data & 0xFF00) >> 8) | ((data & 0xFF) << 8), sendStopBit);
}

int SWI2C::writeToRegister(int regAddress, uint8_t data, bool sendStopBit) {

  startBit();
  writeAddress(0);
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  writeRegister(regAddress);
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  writeByte(data);
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  if (sendStopBit) stopBit();
  return 1;  // Return 1 if no NACKs
}

int SWI2C::read1bFromRegister(int regAddress, uint8_t* data, bool sendStopBit) {

  startBit();
  writeAddress(0); // 0 == Write bit
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  writeRegister(regAddress);
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  startBit();
  writeAddress(1); // 1 == Read bit
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  *data = read1Byte();
  checkAckBit(); // Controller needs to send NACK when done reading data
  if (sendStopBit) stopBit();
  return 1;  // Return 1 if no NACKs
}

int SWI2C::read2bFromRegister(int regAddress, uint16_t* data, bool sendStopBit) {
  // Returns first byte received in LSB. If MSB is first, then use read2bFromRegisterMSBFirst()

  startBit();
  writeAddress(0); // 0 == Write bit
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  writeRegister(regAddress);
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  startBit();
  writeAddress(1); // 1 == Read bit
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  *data = read2Byte(); // Assumes LSB received first
  checkAckBit(); // Controller needs to send NACK when done reading data
  if (sendStopBit) stopBit();
  return 1;  // Return 1 if no NACKs
}

int SWI2C::read2bFromRegisterMSBFirst(int regAddress, uint16_t* data, bool sendStopBit) {
  int retval;
  retval = read2bFromRegister(regAddress, data, sendStopBit);
  *data = ((*data & 0xFF00) >> 8) | ((*data & 0xFF) << 8);
  return retval; 
}

int SWI2C::readBytesFromRegister(int regAddress, uint8_t* data, uint8_t count, bool sendStopBit) {
  // Reads <count> bytes after sending device address and register address.
  // Bytes are returned in <data>, which is assumed to be at least <count> bytes in size.

  startBit();
  writeAddress(0); // 0 == Write bit
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  writeRegister(regAddress);
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  startBit();
  writeAddress(1); // 1 == Read bit
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  // Loop data bytes
  for (int i = 0; i < count; i++) {
    data[i] = read1Byte();
    if (i < (count-1)) {
      writeAck();
    }
    else { // Last byte needs a NACK
      checkAckBit(); // Controller needs to send NACK when done reading data
    }
  }
  if (sendStopBit) stopBit();
  return 1;  // Return 1 if no NACKs
}

int SWI2C::read1bFromDevice(uint8_t* data, bool sendStopBit){
  // Use this with devices that do not use register addresses.

  startBit();
  writeAddress(1); // 1 == Read bit
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  // Loop data bytes
  *data = read1Byte();
  if (sendStopBit) stopBit();
  return 1;  // Return 1 if no NACKs  
}

int SWI2C::readBytesFromDevice(uint8_t* data, uint8_t count, bool sendStopBit) {
  // Use this with devices that do not use register addresses.
  // Reads <count> bytes after sending device address.
  // Bytes are returned in <data>, which is assumed to be at least <count> bytes in size.

  startBit();
  writeAddress(1); // 1 == Read bit
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  // Loop data bytes
  for (int i = 0; i < count; i++) {
    data[i] = read1Byte();
    if (i < (count-1)) {
      writeAck();
    }
    else { // Last byte needs a NACK
      checkAckBit(); // Controller needs to send NACK when done reading data
    }
  }
  if (sendStopBit) stopBit();
  return 1;  // Return 1 if no NACKs
}


int SWI2C::writeBytesToRegister(int regAddress, uint8_t* data, uint8_t count, bool sendStopBit) {
  // Writes <count> bytes after sending device address and register address.
  // Least significant byte is written first, ie. data[0] sent first

  startBit();
  writeAddress(0);
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  writeRegister(regAddress);
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  // Loop data bytes
  for (int i = 0; i < count; i++) {
    writeByte(data[i] & 0xFF); // LSB
    if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  }
  if (sendStopBit) stopBit();
  return 1;  // Return 1 if no NACKs
}

int SWI2C::write1bToDevice(uint8_t data, bool sendStopBit) {
  // Use with devices that do not use register addresses. 

  startBit();
  writeAddress(0);
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  writeByte(data);
  if (sendStopBit) stopBit();
  return 1;  // Return 1 if no NACKs  
}


int SWI2C::writeBytesToDevice(uint8_t* data, uint8_t count, bool sendStopBit) {
  // Use with devices that do not use register addresses. 
  // Writes <count> bytes after sending device address.
  // Least significant byte is written first, ie. data[0] sent first

  startBit();
  writeAddress(0);
  if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  // Loop data bytes
  for (int i = 0; i < count; i++) {
    writeByte(data[i]); // LSB
    if (checkAckBit()) {stopBit(); return 0;} // Immediately end transmission and return 0 if NACK detected
  }
  if (sendStopBit) stopBit();
  return 1;  // Return 1 if no NACKs
}
