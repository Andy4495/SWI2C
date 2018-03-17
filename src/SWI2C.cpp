#include "SWI2C.h"

void sclHi() {
  pinMode(SCL_PIN, INPUT);         // Pull-up resistor pulls SCL high in Hi-Z mode
  // Check to make sure SCL pin has actually gone high before returning
  // Slave may be pulling SCL low to delay transfer (clock stretching)
  while (digitalRead(SCL_PIN) == LOW);
}

void sclLo() {
  pinMode(SCL_PIN, OUTPUT);        // SCL_PIN set LOW in constructor
}

void sdaHi() {
  pinMode(SDA_PIN, INPUT);
}

void sdaLo() {
  pinMode(SDA_PIN, OUTPUT);        // SDA_PIN set LOW in constructor
}

void startBit() {  // Assume SDA already HIGH
  sclHi();
  sdaLo();
  sclLo();
}

void writeAddress(int address, int r_w) {  // Assume SCL, SDA already LOW from startBit()
  if (address & 0x40) sdaHi();     // bit 6
  else sdaLo();
  sclHi();
  sclLo();
  if (address & 0x20) sdaHi();     // bit 5
  else sdaLo();
  sclHi();
  sclLo();
  if (address & 0x10) sdaHi();     // bit 4
  else sdaLo();
  sclHi();
  sclLo();
  if (address & 0x08) sdaHi();     // bit 3
  else sdaLo();
  sclHi();
  sclLo();
  if (address & 0x04) sdaHi();     // bit 2
  else sdaLo();
  sclHi();
  sclLo();
  if (address & 0x02) sdaHi();     // bit 1
  else sdaLo();
  sclHi();
  sclLo();
  if (address & 0x01) sdaHi();     // bit 0
  else sdaLo();
  sclHi();
  sclLo();
  if (r_w == 1) sdaHi();         // R/W bit
  sclHi();
  sclLo();
  sdaHi();    // Release the bus for ACK signal from slave
}

uint8_t checkAckBit() { // Can also be used by master to send NACK after last byte is read from slave
  uint8_t ack;
  sdaHi();    // Release data bus. This will cause a NACK when reading bytes.
  sclHi();
  ack = digitalRead(SDA_PIN);
  sclLo();
  return ack;
}

void writeAck() {  // Used by Master to ACK to slave bewteen multi-byte reads
  sdaLo();    // Release data bus. This will cause a NACK when reading bytes.
  sclHi();
  sclLo();
  sdaHi();
}

void writeRegister(int reg_id) {
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
  sdaHi();  // Release the bus for ACK from slave
}

void stopBit() {  // Assume SCK is already LOW (from ack or data write)
  sdaLo();
  sclHi();
  sdaHi();
}

uint8_t read1Byte() {
  uint8_t value = 0;
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x80;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x40;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x20;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x10;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x08;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x04;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x02;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x01;
  sclLo();

  return value;
}

uint16_t read2Byte() {
  uint16_t value = 0;
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x8000;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x4000;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x2000;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x1000;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x0800;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x0400;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x0200;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x0100;
  sclLo();
  writeAck();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x80;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x40;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x20;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x10;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x08;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x04;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x02;
  sclLo();
  sclHi();
  if (digitalRead(SDA_PIN) == 1) value += 0x01;
  sclLo();
  return value;
}

void writeByte(int data) {
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
  sdaHi();  // Release the bus for ACK from slave
}

int writeToRegister(int deviceID, int regAddress, uint8_t data) {
  startBit();
  writeAddress(deviceID, 0);
  checkAckBit();
  writeRegister(regAddress); // DRCFG register
  checkAckBit();
  writeByte(data);   // Clear register; turn off DR
  checkAckBit();
  stopBit();
  return 1;       // Future support for error checking
}

int readFromRegister(int deviceID, int regAddress, uint8_t* data) {
  startBit();
  writeAddress(deviceID, 0); // 0 == Write bit
  checkAckBit();
  writeRegister(regAddress);
  checkAckBit();
  stopBit();
  startBit();
  writeAddress(deviceID, 1); // 1 == Read bit
  checkAckBit();
  *data = readByte();
  checkAckBit(); // Master needs to send NACK when done reading data
  stopBit();
  return 1;    // Future support for error checking
}

// Need a 16-bit (or variable-size) read and write functions
// Need an ACK-between-read-bytes function (current ACK will signal NACK on reads)
