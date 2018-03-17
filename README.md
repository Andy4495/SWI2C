SWI2C Library
====================

This Energia library provides simple, easy-to-use functions to return the
calibrated internal temperature and Vcc level on supported MSP430 processor types.

Needed this library particularly for the MSP430G2 and FR4133 variants in cases where I needed both SPI and I2C peripherals. The G2 and FR4133 have a single serial controller module (USCI???) which shares the I2C and SPI signals on the same pins.

However, this library was written without any platform-specific code, and should therefore work on any hardware supported by the Arduino or Energia IDE.

Is not a drop-in replacement for Wire (which has its own shortcomings). Therefore need to write your own code for modules that need to use this library. However, the interface is kept simple while still providing low-level signaling functionality to allow tailoring to your specific application.

Clock speed of approximately .... MHz when using the ... MHz.
- Add a scope trace showing SDA/SCK signalling. 

Shortcomings of other libraries:
- Hardcoded delays
   - Should not be necessary at the "slow" signaling speeds inherent in a software solution
- Lack of or incorrect support of clock stretching
- Improper pin mode settings for "HIGH" value
  - Since I2C is an open-collector design that requires pull-up resistors, a "high" value on the data and clock lines should be implemented by putting the pin in "INPUT" mode, which effectively disconnects the signal from the I2C bus (a High Impedance state) allows the pull-ups to indicate the high level.

Usage
-----

Include the library header file:

    #include "SWI2C.h"

Instantiate an object for each I2C device using SWI2C. The device address and I2C pins are defined in the object constructor, so you need a separate object for each device.

    SWI2C myDevice;

Write Data:

    myDevice.write(parameters,....);
        - Description...

Read Data:

    TempF = myMspTemp.getTempCalibratedF();


Lower Level Functions
---------------------
Although most I2C communication can be done with the above `read()` and `write()` methods, there may be times where more direct control of the protocol is required. The following methods are also available:

   m1();
   m2();
   ... etc...

Implementation Details
----------------------

The library ............

Busy Waits and Delays
---------------------

Explain where they are, and how they can be improved.
sclHi() function: waits until line actually goes high before returning (clock stretching)


References
---------------------

+ Texas Instruments I2C [Application Report] (http://www.ti.com/lit/an/slva704/slva704.pdf)
+ Clock Stretching https://www.i2c-bus.org/clock-stretching/
+ Reference related to using INPUT mode for "HIGH" value and high-impedance state
