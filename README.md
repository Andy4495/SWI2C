# SWI2C Library

This library implements a software (bit-bang) I2C controller interface. It was written without any platform-specific code, and should therefore work on any platform supported by the Arduino or Energia IDEs.

The MSP430G2 and MSP430FR4133 LaunchPads do not support using both hardware SPI and hardware I2C at the same time. This became a roadblock in one of my projects as I needed to use both types of interfaces for the devices I was using. I tried several different software I2C libraries, but each one that I tried had a shortcoming that was not acceptable for my application:

- Hardcoded delays
  - Hardcoded delays should not be necessary at the "slow" signaling speeds inherent in a software-based I2C solution using `digitalWrite()` and `pinMode()` methods.
- Lack of or incorrect support of clock stretching
- Incorrectly using `pinMode(HIGH)` for indicating a "HIGH" value on SCL or SDA
  - Since I2C is an open-collector design that requires pull-up resistors, a "high" value on the data and clock lines should be implemented by putting the pin in `INPUT` mode. `INPUT` mode does not drive the bus high or low,  and allows the external pull-ups to assert the high level.

This library implements the I2C protocol without any of the above issues.

This library is not a drop-in replacement for Wire. You will therefore need to write your own code for modules that need to use this library. However, the interface is kept simple while still providing low-level signaling functionality to allow tailoring to your specific application.

Since this is a software-based implementation, the clock speed is significantly reduced compared to a hardware-based I2C implementation. Expect a clock speed of about 25 KHz when using an 8 MHz microcontroller.

## Usage

_Be sure to review the example sketch included with the library. Also see [below](#additional-code-examples) for links to other libraries and sketches using SWI2C._

First, **include** the library header file:

```cpp
#include "SWI2C.h"
```

Next, **instantiate** an object for each I2C device using SWI2C. The device address and I2C pins are defined in the object constructor, so you need a separate object for each device. Note that this is different from the way that the Wire library operates.

`sda_pin` is the pin number for the SDA signal, `scl_pin` is the pin number for the SCL signal, and `deviceID` is the 7-bit device address of the I2C device represented by this object instance. `deviceID` does *not* include the read/write bit.

```cpp
SWI2C myDevice(uint8_t sda_pin, uint8_t scl_pin, uint8_t deviceID);
```

Then, **initialize** the hardware before using the I2C device:

```cpp
myDevice.begin();
```

### High Level Library Methods

The following library methods are used to read and write data to the device. All currently return the value `1`.

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

- Write `count` bytes from `data` to register address `regAddress`:

    ```cpp
    int myDevice.writeBytesToRegister(int regAddress, uint8_t* data, uint8_t count)
    ```

- Read 8-bit value from register address `regAddress` into the location pointed to by `data`:

    ```cpp
    int myDevice.read1bFromRegister(int regAddress, uint8_t* data);
    ```

- Read 16-bit value from register address `regAddress` into the location pointed to by `data`. This function assumes that the first byte received is the least significant byte:

    ```cpp
    int myDevice.read2bFromRegister(int regAddress, uint16_t* data);
    ```

- Same as `read2bFromRegister()`, except the first byte received is the most significant byte:

    ```cpp
    int myDevice.read2bFromRegisterMSBFirst(int regAddress, uint16_t* data);
    ```

- Read `count` number of bytes from from register address `regAddress` into `data_buffer`. **`data_buffer` must defined to have at least `count` elements.**
Bytes received are placed in `data_buffer` LSB first (i.e., the first byte received is put in `data_buffer[0]`, second byte is `data_buffer[1]`, etc.):

    ```cpp
    int myDevice.readBytesFromRegister(int regAddress, uint8_t* data_buffer, uint8_t count);
    ```

### Lower Level Methods

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

- Write the 7-bit device address along with either a read (1) or write (0) bit, for a total of 8 bits:

    ```cpp
    void writeAddress(int r_w);
    ```

- Return the value of the ACK bit received (either 1 or 0). This function is also used to send a NACK from the controller device when reading the last byte from the peripheral device:

    ```cpp
    uint8_t checkAckBit();
    ```

- Write an ACK bit -- used by the controller device between bytes of a multi-byte read operation. The final byte is indicated by a NACK from the controller device (see `checkAckBit()` above):

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

There are no hardcoded delays in the code. In order to support clock-stretching, there is a busy-wait loop in the `sclHi()` method which waits until the SCL line actually goes high before exiting the function. There is a default timeout of 500 ms before the wait times out and the function returns. This delay can be changed on a per-device basis (`setStretchTimeout()`). It can also be set to zero if no timeout is desired (which could potentially cause the library to "lock up" if the I2C device does not properly release the SCL line).

## Future Updates

Future potential library updates:

- Additional error checking

## References

- NXP [I2C Bus Specification and User Manual][1]
- Texas Instruments [I2C Application Report][2]
- [Clock Stretching][3]

## Additional Code Examples

Besides the sketch included in the `examples` folder, several more examples of code using SWI2C are available in my other published libraries and sketches:

- Weather Sensors Software I2C [Library][10]
- EEPROM Software I2C [Library][11]
- BQ27441 Software I2C [Library][12]
- Tempearature Sensor With Display [Sketch][13]
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
[100]: https://choosealicense.com/licenses/mit/
[101]: ./LICENSE
[200]: https://github.com/Andy4495/SWI2C
