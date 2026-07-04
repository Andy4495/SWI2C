/* -----------------------------------------------------------------
   SWI2C Library Example Sketch - MPU6050
   https://github.com/Andy4495/SWI2C

   MIT License

   07/03/2026 - Original
   ----------------------------------------------------------------- */
/* -----------------------------------------------------------------
   This example shows how to communicate with a typical register-based
   I2C sensor -- in this case, an MPU6050 accelerometer/gyroscope --
   using the SWI2C library.

   It demonstrates:
     - Waking up a sensor by writing to a configuration register
     - Reading several consecutive data registers in one transaction
     - Combining a high byte and low byte into a signed 16-bit value

   These same techniques apply to most other register-based I2C
   sensors, not just the MPU6050, so this sketch can also be used as
   a general starting point for a new SWI2C sensor project.
   -----------------------------------------------------------------
*/

#include "SWI2C.h"

#define SDA_PIN 2
#define SCL_PIN 3

// MPU6050 7-bit I2C address (assumes the AD0 pin is tied to GND)
const uint8_t MPU6050_ADDR = 0x68;

// MPU6050 register addresses used in this example
const uint8_t PWR_MGMT_1   = 0x6B; // Power management register
const uint8_t ACCEL_XOUT_H = 0x3B; // First of 14 consecutive sensor data registers

// The MPU6050 stores its accelerometer, temperature, and gyroscope
// readings in 14 consecutive registers starting at ACCEL_XOUT_H.
const uint8_t SENSOR_DATA_LENGTH = 14;

// Byte offset of each measurement within the 14-byte block.
const uint8_t ACCEL_XOUT_OFFSET = 0;
const uint8_t ACCEL_YOUT_OFFSET = 2;
const uint8_t ACCEL_ZOUT_OFFSET = 4;
const uint8_t TEMP_OUT_OFFSET   = 6;
const uint8_t GYRO_XOUT_OFFSET  = 8;
const uint8_t GYRO_YOUT_OFFSET  = 10;
const uint8_t GYRO_ZOUT_OFFSET  = 12;

const unsigned long delayTime = 1000;
unsigned long lastMillis = 0;

// Buffer to hold the 14 raw bytes read from the sensor each cycle
uint8_t sensorData[SENSOR_DATA_LENGTH];

// Create the I2C device object for the MPU6050
SWI2C myMPU = SWI2C(SDA_PIN, SCL_PIN, MPU6050_ADDR);

// Combines a high byte and a low byte into a signed 16-bit value.
int16_t combineBytes(uint8_t highByte, uint8_t lowByte) {
  return (int16_t)(((uint16_t)highByte << 8) | lowByte);
}

void setup() {
  Serial.begin(115200);

  // Set up the SDA/SCL pins for this device
  myMPU.begin();

  // The MPU6050 powers up in sleep mode, so it must be woken up before
  // it will take any measurements.
  int retval = myMPU.writeToRegister(PWR_MGMT_1, 0x00);

  if (retval == 0) {
    Serial.println("Error: MPU6050 did not respond. Check wiring and I2C address.");
  } else {
    Serial.println("SWI2C MPU6050 Example. MPU6050 initialized.");
  }

  lastMillis = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastMillis >= delayTime) {
    lastMillis = currentMillis;

    // Read all 14 sensor bytes in a single I2C transaction.
    int retval = myMPU.readFromRegister(ACCEL_XOUT_H, sensorData, SENSOR_DATA_LENGTH);

    if (retval == 0) {
      Serial.println("Error: Failed to read sensor data (NACK received).");
      return;
    }

    int16_t accelX  = combineBytes(sensorData[ACCEL_XOUT_OFFSET],     sensorData[ACCEL_XOUT_OFFSET + 1]);
    int16_t accelY  = combineBytes(sensorData[ACCEL_YOUT_OFFSET],     sensorData[ACCEL_YOUT_OFFSET + 1]);
    int16_t accelZ  = combineBytes(sensorData[ACCEL_ZOUT_OFFSET],     sensorData[ACCEL_ZOUT_OFFSET + 1]);
    int16_t rawTemp = combineBytes(sensorData[TEMP_OUT_OFFSET],       sensorData[TEMP_OUT_OFFSET + 1]);
    int16_t gyroX   = combineBytes(sensorData[GYRO_XOUT_OFFSET],      sensorData[GYRO_XOUT_OFFSET + 1]);
    int16_t gyroY   = combineBytes(sensorData[GYRO_YOUT_OFFSET],      sensorData[GYRO_YOUT_OFFSET + 1]);
    int16_t gyroZ   = combineBytes(sensorData[GYRO_ZOUT_OFFSET],      sensorData[GYRO_ZOUT_OFFSET + 1]);

    float temperatureC = rawTemp / 340.0 + 36.53;

    Serial.println("---------------------------------");
    Serial.println("Accelerometer");
    Serial.print("  X: "); Serial.println(accelX);
    Serial.print("  Y: "); Serial.println(accelY);
    Serial.print("  Z: "); Serial.println(accelZ);

    Serial.println("Gyroscope");
    Serial.print("  X: "); Serial.println(gyroX);
    Serial.print("  Y: "); Serial.println(gyroY);
    Serial.print("  Z: "); Serial.println(gyroZ);

    Serial.print("Temperature: ");
    Serial.print(temperatureC);
    Serial.println(" C");
    Serial.println("---------------------------------");
    Serial.println();
  }
}