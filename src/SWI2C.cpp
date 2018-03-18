#include "SWI2C.h"

SWI2C::SWI2C(uint8_t sda_pin, uint8_t scl_pin, uint8_t deviceID) {
  _sda_pin = sda_pin;
  _scl_pin = scl_pin;
  _deviceID = deviceID;
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
  pinMode(_scl_pin, INPUT);         // Pull-up resistor pulls SCL high in Hi-Z mode
  // Check to make sure SCL pin has actually gone high before returning
  // Slave may be pulling SCL low to delay transfer (clock stretching)
  while (digitalRead(_scl_pin) == LOW);
}

void SWI2C::sclLo() {
  pinMode(_scl_pin, OUTPUT);        // _scl_pin set LOW in constructor
}

void SWI2C::sdaHi() {
  pinMode(_sda_pin, INPUT);
}

void SWI2C::sdaLo() {
  pinMode(_sda_pin, OUTPUT);        // _sda_pin set LOW in constructor
}

void SWI2C::startBit() {  // Assume SDA already HIGH
  sclHi();
  sdaLo();
  sclLo();
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
  sdaHi();    // Release the data line for ACK signal from slave
}

uint8_t SWI2C::checkAckBit() { // Can also be used by master to send NACK after last byte is read from slave
  uint8_t ack;
  sdaHi();    // Release data line. This will cause a NACK from master when reading bytes.
  sclHi();
  ack = digitalRead(_sda_pin);
  sclLo();
  return ack;
}

void SWI2C::writeAck() {  // Used by Master to ACK to slave bewteen multi-byte reads
  sdaLo();
  sclHi();
  sclLo();
  sdaHi();  // Release the data line
}

void SWI2C::writeRegister(int reg_id) {
  if (reg_id & 0x80) sdaHi();     // bit 7
  else sdaLo();
  sclHi();
  sclLo();
  if (reg_id & 0x40) sdaHi();     // bit 6
  else sdaLo();
  sclHi();
  sclLo();
  if (reg_id & 0x20) sdaHi();     // bit 5
  else sdaLo();
  sclHi();
  sclLo();
  if (reg_id & 0x10) sdaHi();     // bit 4
  else sdaLo();
  sclHi();
  sclLo();
  if (reg_id & 0x08) sdaHi();     // bit 3
  else sdaLo();
  sclHi();
  sclLo();
  if (reg_id & 0x04) sdaHi();     // bit 2
  else sdaLo();
  sclHi();
  sclLo();
  if (reg_id & 0x02) sdaHi();     // bit 1
  else sdaLo();
  sclHi();
  sclLo();
  if (reg_id & 0x01) sdaHi();     // bit 0
  else sdaLo();
  sclHi();
  sclLo();
  sdaHi();  // Release the data line for ACK from slave
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
  sdaHi();  // Release the data line for ACK from slave
}

int SWI2C::writeToRegister(int regAddress, uint8_t data) {
  startBit();
  writeAddress(0);
  checkAckBit();
  writeRegister(regAddress); // DRCFG register
  checkAckBit();
  writeByte(data);   // Clear register; turn off DR
  checkAckBit();
  stopBit();
  return 1;       // Future support for error checking
}

int SWI2C::read1bFromRegister(int regAddress, uint8_t* data) {
  startBit();
  writeAddress(0); // 0 == Write bit
  checkAckBit();
  writeRegister(regAddress);
  checkAckBit();
//  stopBit();
  startBit();
  writeAddress(1); // 1 == Read bit
  checkAckBit();
  *data = read1Byte();
  checkAckBit(); // Master needs to send NACK when done reading data
  stopBit();
  return 1;    // Future support for error checking
}

int SWI2C::read2bFromRegister(int regAddress, uint16_t* data) {
  startBit();
  writeAddress(0); // 0 == Write bit
  checkAckBit();
  writeRegister(regAddress);
  checkAckBit();
//  stopBit();
  startBit();
  writeAddress(1); // 1 == Read bit
  checkAckBit();
  *data = read2Byte();
  checkAckBit(); // Master needs to send NACK when done reading data
  stopBit();
  return 1;    // Future support for error checking
}

// Need 16-bit (or variable-size) read and write functions
