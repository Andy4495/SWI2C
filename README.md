# SWI2C - Software I2C Library

[![Arduino Compile Sketches](https://github.com/Andy4495/SWI2C/actions/workflows/arduino-compile-sketches.yml/badge.svg)](https://github.com/Andy4495/SWI2C/actions/workflows/arduino-compile-sketches.yml)
[![Check Markdown Links](https://github.com/Andy4495/SWI2C/actions/workflows/CheckMarkdownLinks.yml/badge.svg)](https://github.com/Andy4495/SWI2C/actions/workflows/CheckMarkdownLinks.yml)

This library implements a software I2C controller interface. It was written without any platform-specific code, and should therefore work on any platform supported by the Arduino or Energia IDEs.

Since this is a software-based, platform-agnostic implementation, the clock speed is significantly reduced compared to a hardware-based I2C implementation. Expect a clock speed of about 25 KHz when using an 8 MHz microcontroller.

## Comparison to Arduino Wire Library

This library is not a drop-in replacement for [Wire][15]. You will therefore need to write your own code for sketches that need to use this library.

However, this library provides a much simpler interface than the Wire library, and modifying existing code to use this new library should be relatively straightforward in most cases.

I consider the Arduino Wire library needlessly complicated, particularly for beginners, for two key reasons:

1. The vast majority of I2C devices that I have worked with generally have a simple "read from register" or "write to register" interface.
    - This implies that simple `readFromRegister()` and `writeToRegister()` methods should be a fundamental part of the library API.
    - The user should not have to string together multiple commands just to perform a simple write to a device register.
    - The API should provide the hooks necessary to support more complicated communications, but not at the expense of ease-of-use for the most common use cases.
2. The Wire library abstracts the I2C hardware interface as the object being acted on.
    - From a design standpoint, I would suggest that the actual I2C *device* is the object that is being acted on.  
    - The interface should therefore be modeled with the *device* as the object instantiated in code, not the hardware interface.  

This software I2C library was designed so that the I2C *device* is the object that is instantiated and being acted on by the various methods. The API provides several simple `readFrom()` and `writeTo()` methods, while also including low-level signaling functionality to create more complicated communication flows when necessary.

## Additional Software I2C Improvements

In addition to simplifying the user interface compared to Wire, this library also addresses the following shortcomings of some of the other software I2C libraries:

- Hardcoded delays
  - Hardcoded delays should not be necessary at the "slow" signaling speeds inherent in a software-based I2C solution using `digitalWrite()` and `pinMode()` methods.
- Lack of or incorrect support of clock stretching
- Incorrectly using `pinMode(OUTPUT)` and `digitalWrite(HIGH)` for indicating a "HIGH" value on SCL or SDA
  - Since I2C is an open-collector design that requires pull-up resistors, a "high" value on the data and clock lines should be implemented by putting the pin in `INPUT` mode. `INPUT` mode does not drive the bus high or low,  and allows the external pull-ups to assert the high level.

This library implements the I2C protocol without the above issues.

## Usage

*Be sure to review the example sketches included with the library. Also see [below](#additional-code-examples) for links to other libraries and sketches using SWI2C.*

1. **Include** the library header file:

    ```cpp
    #include "SWI2C.h"
    ```

2. **Instantiate** an object for each I2C device using SWI2C. The device address and I2C pins are defined in the object constructor, so you need a separate object for each device. Note that this is different from the way that the Wire library operates.

    `sda_pin` is the pin number for the SDA signal, `scl_pin` is the pin number for the SCL signal, and `deviceID` is the 7-bit device address of the I2C device represented by this object instance. `deviceID` does *not* include the read/write bit.

    ```cpp
    SWI2C myDevice(uint8_t sda_pin, uint8_t scl_pin, uint8_t deviceID);
    ```

3. **Initialize** the hardware before using the I2C device:

    ```cpp
    myDevice.begin();
    ```

4. Use the high level or low level library methods described below.

### High Level Library Methods

The following high level library methods are used to read and write data to the device. See [below](#return-codes) for details on the return codes for these methods.

- Write an 8-bit `data` value to device register `regAddress`:

    ```cpp
    int myDevice.writeToRegister(int regAddress, uint8_t data);
    ```

- Write a 16-bit `data` value to device register `regAddress`. The first byte written is the least signifcant byte of `data`:

    ```cpp
    int myDevice.write2bToRegister(int regAddress, uint16_t data);
    ```

- Same as `write2bToRegister()`, except that the first byte written is the most signifcant byte of `data`:

    ```cpp
    int myDevice.write2bToRegisterMSBFirst(int regAddress, uint16_t data);
    ```

- Write `count` bytes from the location pointed to by `data_buffer` to register address `regAddress`:

    ```cpp
    int myDevice.writeBytesToRegister(int regAddress, uint8_t* data_buffer, uint8_t count)
    ```

- Write `count` bytes from the location pointed to by `data_buffer` to the device. This is used for devices which do not have registers (e.g. PCA9548):

    ```cpp
    int myDevice.writeBytesToDevice(uint8_t* data_buffer, uint8_t count)
    ```

- Read 8-bit value from register address `regAddress` into the location pointed to by `data_buffer`:

    ```cpp
    int myDevice.read1bFromRegister(int regAddress, uint8_t* data_buffer);
    ```

- Read 16-bit value from register address `regAddress` into the location pointed to by `data`. This function assumes that the first byte received is the least significant byte:

    ```cpp
    int myDevice.read2bFromRegister(int regAddress, uint16_t* data);
    ```

- Same as `read2bFromRegister()`, except the first byte received is the most significant byte:

    ```cpp
    int myDevice.read2bFromRegisterMSBFirst(int regAddress, uint16_t* data);
    ```

- Read `count` number of bytes from register address `regAddress` into the location pointed to by `data_buffer`. **`data_buffer` must be defined to have at least `count` elements.**
Bytes received are placed in `data_buffer` LSB first (i.e., the first byte received is put in `data_buffer[0]`, second byte is `data_buffer[1]`, etc.):

    ```cpp
    int myDevice.readBytesFromRegister(int regAddress, uint8_t* data_buffer, uint8_t count);
    ```

- Read `count` number of bytes from the device into the location pointed to by `data_buffer`. This is used for devices which do not have registers (e.g. PCA9548). **`data_buffer` must be defined to have at least `count` elements.**
Bytes received are placed in `data_buffer` LSB first (i.e., the first byte received is put in `data_buffer[0]`, second byte is `data_buffer[1]`, etc.):

    ```cpp
    int myDevice.readBytesFromDevice(uint8_t* data_buffer, uint8_t count);
    ```

#### Return Codes

All of the high level methods above return the integer `1` if the message was sent successfully, and return `0` if a NACK was detected during the I2C communication.

If a NACK was detected:  

- The transmission is immediately stopped, with no more data sent or received.
- An I2C STOP condition is signalled on the bus.
- The I2C bus is released.
- The contents of the `data` variable for the various `readRegister()` methods should be assumed to be invalid, as a partial or incorrect data transfer has occurred.

#### Repeated Start

Each of the high level methods listed above also takes an optional final parameter `bool sendStopBit` which defaults to `true` (and therefore does not need to be specified when calling any of these methods). When `sendStopBit` is `true`, the I2C message will end with a STOP bit and release control of the I2C bus. If set to `false`, a STOP bit will not be sent at the end of the message, and the I2C bus will be kept active by the controller. This is also referred to as a "repeated start" or "restart" condition, and is probably of limited usefulness when using this library.

### Low Level Methods

Although general I2C communication can be done with the above `readFrom` and `writeTo` methods, there may be times where more direct control of the protocol is required. The following public methods are also available in the SWI2C class.

- The following methods are self-explanatory -- they set the SCL or SDA lines HIGH or LOW:

    ```cpp
    void sclHi();
    void sclLo();
    void sdaHi();
    void sdaLo();
    ```

- Signal a START bit on the I2C bus:

    ```cpp
    void startBit();
    ```

- Write the 7-bit device address along with either a read (1) or write (0) bit (`r_w`), for a total of 8 bits:

    ```cpp
    void writeAddress(int r_w);
    ```

- Return the value of the ACK bit received (either 1 or 0) from the device. This function is also used to send a NACK from the controller when reading the last byte from the peripheral device:

    ```cpp
    uint8_t checkAckBit();
    ```

- Write an ACK bit -- used by the controller between bytes of a multi-byte read operation. The final byte is indicated by a NACK from the controller (see `checkAckBit()` above):

    ```cpp
    void writeAck();
    ```

- Write the 8-bit `regAddress` value to the SDA bus:

    ```cpp
    void writeRegister(int regAddress);
    ```

- Signal a STOP bit on the I2C bus:

    ```cpp
    void stopBit();
    ```

- Return an 8-bit value read from the I2C bus:

    ```cpp
    uint8_t read1Byte();
    ```

- Return a 16-bit value read from the I2C bus. The first byte received is assumed to be the least significant byte:

    ```cpp
    uint16_t read2Byte();
    ```

- Write an 8-bit `data` value to the I2C bus:

    ```cpp
    void writeByte(int data);
    ```

- Set the clock stretching timeout to `t` milliseconds. The default timeout is 500 ms when the SWI2C object is created. If the timeout value is set to zero, then the library will wait indefinitely for the clock to be released by the device:

    ```cpp
    void setStretchTimeout(unsigned long t);
    ```

- Read the current clock stretching timeout value:

    ```cpp
    unsigned long getStretchTimeout();
    ```

- Check if a previous clock operation resulted in a timeout. Returns non-zero if the timeout was reached waiting for the SCL line to go high. The error will remain set internally until this function is called. Reading the value (by calling this function) clears the error:

    ```cpp
    unsigned long checkStretchTimeout();
    ```

## Implementation Details

The library does not use any platform-specific code. All I/O functions use standard Arduino `pinMode()`, `digitalRead()`, and `digitalWrite()` functions. Reading and writing to the I2C device is accomplished by stringing together the basic signaling primitives specified by the protocol.

There are no hardcoded delays in the code. However, the high-level `readFrom()` and `writeTo()` methods are blocking -- they do not return until the message is completed or the clock-stretching timeout has been exceeded.

The clock-stretching timeout is implemented with a busy-wait loop in the `sclHi()` method which waits until the SCL line actually goes high before exiting the function. There is a default timeout of 500 ms before the wait times out and the function returns. This delay can be changed on a per-device basis (`setStretchTimeout()`). It can also be set to zero if no timeout is desired (which could potentially cause the library to "lock up" if the I2C device does not properly release the SCL line).

## References

- NXP [I2C Bus Specification and User Manual][1]
- Texas Instruments [I2C Application Report][2]
- [Clock Stretching][3]
- Arduino Hardware I2C [Wire library][15]
- PCA9548A 8 channel I2C switch [datasheet][16]
  - Example of a device which does not have registers. I2C data is read/written immediately after sending the device address.

## Additional Code Examples

Besides the sketches included in the `examples` folder, several more examples of code using SWI2C are available in my other published libraries and sketches:

- Weather Sensors Software I2C [Library][10]
- EEPROM Software I2C [Library][11]
- BQ27441 Software I2C [Library][12]
- Temperature Sensor With Display [Sketch][13]
- Sensor Repeater [Sketch][14]

## License

The software and other files in this repository are released under what is commonly called the [MIT License][100]. See the file [`LICENSE`][101] in this repository.

[1]: https://www.nxp.com/docs/en/user-guide/UM10204.pdf
[2]: http://www.ti.com/lit/an/slva704/slva704.pdf
[3]: https://www.i2c-bus.org/clock-stretching/
[10]: https://github.com/Andy4495/Weather_Sensors_SWI2C
[11]: https://github.com/Andy4495/EEPROM_24C08_SWI2C
[12]: https://github.com/Andy4495/BQ27441_SWI2C
[13]: https://github.com/Andy4495/MSP430TempSensorWithDisplay
[14]: https://github.com/Andy4495/Sensor-Repeater
[15]: https://www.arduino.cc/reference/en/language/functions/communication/wire/
[16]: https://www.ti.com/lit/ds/symlink/pca9548a.pdf
[100]: https://choosealicense.com/licenses/mit/
[101]: ./LICENSE
[200]: https://github.com/Andy4495/SWI2C
