# TFMini-Plus-I2C
### PLEASE NOTE:
**v1.7.3** - Different Arduinos use different Wire libraries.  The for `requestFrom` the most common signature is `( int, int, int)`.  For the two calls in this library, the final `stopbit` value is changed from boolean `true` to literal `1` to prevent certain IDE error messages.

A big "Thank you!" to Hans Boot (https://github.com/hb020) for finding, researching and gently correcting this error.

**v1.7.2** - All delays are eliminated from the `recoverI2Cbus()` function so that data acquisition can resume as quickly as possible after an I2C error.  If SDA and SCL pin numbers are specified, the function acts silently.  The default version displays status, i.e. "Recover default I2C bus.".

**v1.7.0** - This version extends use of the `recoverI2Cbus()` function by assuming default I2C port pin numbers. If pins are not specified, the function will look for and include a second I2C port, if any. This function has been added to the example sketch.

**v1.6.0** - This version reverses and corrects the `ENABLE_OUTPUT` and `DISABLE_OUTPUT` commands.<br />

**v1.5.0** - An I2C device that quits unexpectedly can leave the bus in a hung state, waiting for a data transfer to complete.  With this version, users can call `recoverI2CBus()` instead of `Wire.begin()` in the `setup()` portion of the example sketch to relieve this condition.  See the library files for more details.<br />

Also in this version, redundant code to close the I2C bus following a `requestFrom()` created problems for some users.  It was not needed and has been eliminated.<br />

Three commands names have changed in this version:
<br />&nbsp;&nbsp;&#9679;&nbsp;`OBTAIN_FIRMWARE_VERSION`  is now `GET_FIRMWARE_VERSION`
<br />&nbsp;&nbsp;&#9679;&nbsp;`RESTORE_FACTORY_SETTINGS` is now `HARD_RESET`
<br />&nbsp;&nbsp;&#9679;&nbsp;`SYSTEM_RESET`             is now `SOFT_RESET`<br />

**v1.4.0** - With this version, data variables changed from unsigned to signed 16bit integers in order to support error codes returned in the `dist` (distance) and `flux` (signal strength) data. The only working error code at the moment is a `-1` returned in `flux` data when the signal is saturated.

In the example code, `printStatus()` or `printErrorStatus()` has been replaced with `printFrame()` in response to a failed `getData()` or `printReply()` if responding to `sendCommand()`.
<hr />

### Arduino library for the Benewake TFMini-Plus using I2C communication interface

The **TFMini-S** is largely compatible with the **TFMini-Plus** and both are able to use this library.  Although the **TFLuna** is compatible with the **TFMini-Plus** in the UART (serial) mode, in **I2C** mode it is very different and therefor this library is _not compatible_ with the **TFLuna** in **I2C** mode.  This library is also *not compatible* with the **TFMini**, a product with its own unique command and data structure.

Since hardware version 1.3.5 and firmware version 1.9.0, the TFMini-Plus can be configured to use the **I2C** (two-wire) protocol for its communication interface.  The command to configure the device for **I2C** communication must be sent using the **UART** interface.  Therefore, configuration should be made prior to the device's service installation either by using the TFMini-Plus library or by using a serial GUI test application available from the manufacturer.  Thereafter, this libarary can be used for all further communication with the device.  _When switching between communication modes, please remember to switch the data cables, too.  That sounds obvious, but has tripped me up more than once._

This library calls the Arduino **Wire** library.  Although largely standardized, the Wire library has been customized to accommodate a wide variety of Arduino boards that are available.
<hr />

### Arduino Library Commands

`getData( dist, flux, temp, addr)`&nbsp; passes back three, signed, 16-bit measurement data values and sends an optional, unsigned, 8-bit address.  If the default device address is used, the optional `addr` value may be omitted.  Otherwise, a correct `addr` value always must be sent.  If the function completes without error, it returns 'True' and sets the public, one-byte 'status' code to zero.  Otherwise, it returns 'False' and sets the 'status' code to a library defined error code.

Measurement data values are passed-back in three, 16-bit, signed integer variables:
<br />&nbsp;&nbsp;&#9679;&nbsp; `dist` Distance to target in centimeters. Range: 0 - 1200
<br />&nbsp;&nbsp;&#9679;&nbsp; `flux` Strength or quality of return signal or error. Range: -1, 0 - 32767
<br />&nbsp;&nbsp;&#9679;&nbsp; `temp` Temperature of device chip in code. Range: -25°C to 125°C

`getData( dist)`&nbsp; and `getData( dist, addr)`&nbsp; functions are included for further simplicity and convenience.  These two functions pass back only distance data and use either the default or an assigned I2C address.

`sendCommand( cmnd, param, addr)`&nbsp; function sends an unsigned, 32-bit command, an unsigned, 32-bit parameter and an optional, unsigned, 8-bit I2C address to the device.  A proper command (`cmnd`) must be chosen from the library's list of defined commands.  A parameter (`param`) can be entered directly (0x10, 250, etc.), or chosen from the Library's list of defined parameters.  For many commands, i.e. `HARD_RESET`, the correct `param` is a `0` (zero).  If the default device address is used, the optional `addr` value may be omitted.  If the function completes without error, it returns 'true' and sets a public, one-byte 'status' code to zero.  Otherwise, it returns 'false' and sets the 'status' to a Library defined error code.

`cmnd`&nbsp;&nbsp; The defined commands are:<br />
`GET_FIRMWARE_VERSION`, `TRIGGER_DETECTION`, `SOFT_RESET`, `HARD_RESET`, `SAVE_SETTINGS`, `SET_FRAME_RATE`, `SET_BAUD_RATE`, `STANDARD_FORMAT_CM`, `STANDARD_FORMAT_MM`, `ENABLE_OUTPUT`, `DISABLE_OUTPUT`, `SET_I2C_ADDRESS`, `SET_SERIAL_MODE`, `SET_I2C_MODE`, `I2C_FORMAT_CM`, `I2C_FORMAT_MM`

`param`&nbsp;&nbsp; The defined paramters are:<br />
`BAUD_9600`, `BAUD_14400`, `BAUD_19200`, `BAUD_56000`, `BAUD_115200`, `BAUD_460800`, `BAUD_921600`<br />
and<br />
`FRAME_0`, `FRAME_1`, `FRAME_2`, `FRAME_5`, `FRAME_10`, `FRAME_20`, `FRAME_25`, `FRAME_50`, `FRAME_100`, `FRAME_125`, `FRAME_200`, `FRAME_250`, `FRAME_500`, `FRAME_1000`
<hr>

### Using the I2C version of the device
In **I2C** mode, the TFMini-Plus functions like an I2C slave device.  The default address is `0x10` (16 decimal), but is user-programmable by sending the `SET_I2C_ADDRESS` command and a parameter in the range of `1` to `127`.  A new address will take effect immediately and permanently without sending a `SAVE_SETTINGS` command.

Although I2C address of the device can be changed while in UART communication mode, the value of that address cannot be tested in UART mode. For that, the device must be in I2C mode. Then a user can scan the I2C bus for the presence of the device's address. The `TFMPI2C_changeI2C.ino` sketch in the example folder includes a `scanAddr()` function.

**If the I2C device address is any other than the default value of `0x10`, that new, non-default address must be included with every subsequent command, including `getData()`, as the optional `addr` byte.**

The `HARD_RESET` command (Restore Factory Settings) will reset the device to the default address of `0x10`. The `SOFT_RESET` command (System Reset) appears to have no effect on the I2C address.  The `HARD_RESET` command will **not** restore the device to the default, **UART** communication interface mode.  The **only** way to return the device to serial mode is to send the `SET_SERIAL_MODE` command.

Since Version 1.5.0 of the Library, a `recoverI2Cbus( SDA, SCL)` function is available to clear an I2C bus "hung" condition after an I2C error is reported.  If pin numbers are not specified, `recoverI2Cbus()` will use the default pin numbers specified in the board's `varints.h` file.

Benewake is not forthcoming about the internals of the device, however they did share this:
>Some commands that modify internal parameters are processed within 1ms.  Some commands (that) require the MCU to communicate with other chips may take several ms.  And some commands, such as saving configuration and restoring the factory (default settings) need to erase the FLASH of the MCU, which may take several hundred ms.

Also, according to Benewake:
>The measuring frequency (frame-rate) of the module should be 2.5 times larger than the I2C reading frequency.<br />
>The I2C reading frequency should not exceed 100Hz.<br />

Benewake says the data frame-rate is limited to 1KHz, which would suggest a 400Hz data sampling limit in **I2C** mode.  But Benewake also says data sampling should not exceed 100Hz.  They don't say why; but you might keep those supposed limitations in mind while you are using the **I2C** interface.

Frame-rate changes should be followed by a `SAVE_SETTINGS` command or may be lost when power is removed.  There is no way to determine what the data frame-rate is actually set to.
<hr />

### Using the I/O modes of the device
The so-called I/O modes are not supported in this library.  Please do not attempt to use any I/O commands that you may find to be defined in this library's header file.

The I/O output mode is enabled and disabled by this 9 byte command:<br />
5A 09 3B MODE DL DH ZL ZH SU

Command byte number:<br />
0 &nbsp;&nbsp; `0x5A`:  Header byte, starts every command frame<br />
1 &nbsp;&nbsp; `0x09`:  Command length, number of bytes in command frame<br />
2 &nbsp;&nbsp; `0x3B`:  Command number<br />
<br />
3 &nbsp;&nbsp; MODE:<br />
&nbsp;&nbsp;&nbsp;&nbsp; `0x00`: I/O Mode OFF, standard data output mode<br />
&nbsp;&nbsp;&nbsp;&nbsp; `0x01`: I/O Mode ON, output: near = high and far = low<br />
&nbsp;&nbsp;&nbsp;&nbsp; `0x02`: I/O Mode ON, output: near = low and far = high<br />
<br />
4 &nbsp;&nbsp; DL: Near distance lo order byte of 16 bit integer<br />
5 &nbsp;&nbsp; DH: Near distance hi order byte<br />
<br />
6 &nbsp;&nbsp; ZL: Zone width lo byte<br />
7 &nbsp;&nbsp; ZL: Zone width hi byte<br />
<br />
8 &nbsp;&nbsp;SU: Checkbyte (the lo order byte of the sum of all the other bytes in the frame)<br />
<br />
If an object's distance is greater than the Near distance (D) plus the Zone width (Z) then the object is "far."<br />
If the distance is less than the Near distance (D) then the object is "near".<br />
The Zone is a neutral area. Any object distances measured in this range do not change the output.<br />
The output can be set to be either high when the object is near and low when it's far (Mode 1); or low when it's near and high when it's far (Mode 2).<br />
The high level is 3.3V, the low level is 0V.
<hr>

Also included in the repository are:
<br />&nbsp;&nbsp;&#9679;&nbsp; An Arduino sketch "TFMPI2C_example.ino" in the Example folder.
<br />&nbsp;&nbsp;&#9679;&nbsp; An Arduino sketch "TFMPI2C_changeI2C.ino" in the Example folder.
<br />&nbsp;&nbsp;&#9679;&nbsp; Recent copies of manufacturer's Datasheet and Product Manual in Documents.
<br />&nbsp;&nbsp;&#9679;&nbsp; A folder containing the Datasheet and Product Manual for the TFMini-S
<br />&nbsp;&nbsp;&#9679;&nbsp; General information regarding Time of Flight distance sensing and the Texas Instruments OPT3101 module in Documents in the TI OPT3101 sub-folder.

All of the code for this library is richly commented to assist with understanding and in problem solving.
