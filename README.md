# TFMini-Plus-I2C
### PLEASE NOTE:

An device that quits unexpectedly can leave the I2C bus in a hung state, waiting for a data transfer to finish.  With version 1.5.0 of this library, users can call `recoverI2CBus()` instead of `Wire.begin()` in the `setup()` portion of their sketch to releave this condition.  See the library files for more details.

Also in this version, redundant code to close the I2C interface following a `requestFrom()` created problems for some users.  It was not needed and has been eliminated.

Three commands names have changed in this version:
<br />&nbsp;&nbsp;&#9679;&nbsp;`OBTAIN_FIRMWARE_VERSION`  is now `GET_FIRMWARE_VERSION`
<br />&nbsp;&nbsp;&#9679;&nbsp;`RESTORE_FACTORY_SETTINGS` is now `HARD_RESET`
<br />&nbsp;&nbsp;&#9679;&nbsp;`SYSTEM_RESET`             is now `SOFT_RESET`

With version v.1.4.0, data variables changed from unsigned to signed 16bit integers in order to support error codes returned in the `dist` (distance) and `flux` (signal strength) data. The only wokring error code at the moment is `-1` returned as `flux` data when the return signal is saturated.

In the example code, `printStatus()` or `printErrorStatus()` has been replaced with `printFrame()` in response to a failed `getData()` or `printReply()` if responding to `sendCommand()`.
<hr />

### Arduino library for the Benewake TFMini-Plus using I2C communication interface

The **TFMini-S** is said to be compatible with the **TFMini-Plus** and therefore able to use this library.  However, this library is *not compatible* with the **TFMini**, which is a different product with its own command and data structure.

Since hardware version 1.3.5 and firmware version 1.9.0, the TFMini-Plus can be configured to use the **I2C** (two-wire) protocol for its communication interface.  The command to configure the device for **I2C** communication must be sent using the **UART** interface.  Therefore, configuration should be made prior to the device's service installation either by using the TFMini-Plus library or by using a serial GUI test application available from the manufacturer.  Thereafter, this libarary can be used for all further communication with the device.

This library calls the Arduino standard I2C/Two-Wire Library.
<hr />

### Arduino Library Commands

The `getData( dist, flux, temp, addr)` function passes back three, signed, 16-bit measurement data values and sends an optional, unsigned, 8-bit address.  If the default device address is used, the optional `addr` value may be omitted.  Otherwise, a correct `addr` value always must be sent.  If the function completes without error, it returns 'True' and sets the public, one-byte 'status' code to zero.  Otherwise, it returns 'False' and sets the 'status' code to a library defined error code.

Measurement data values are passed-back in three, 16-bit, signed integer variables:
<br />&nbsp;&nbsp;&#9679;&nbsp; `dist` Distance to target in centimeters. Range: 0 - 1200
<br />&nbsp;&nbsp;&#9679;&nbsp; `flux` Strength or quality of return signal or error. Range: -1, 0 - 32767
<br />&nbsp;&nbsp;&#9679;&nbsp; `temp` Temperature of device chip in code. Range: -25°C to 125°C

For further convenience and simplicity, the `getData( dist)` and `getData( dist, addr)` functions are included. These functions pass back distance data only and use either the default or an assigned I2C address.

The `sendCommand( cmnd, param, addr)` function sends an unsigned, 32-bit command, an unsigned, 32-bit parameter and an optional, unsigned, 8-bit I2C address to the device.  A proper command (`cmnd`) must be chosen from the library's list of defined commands.  A parameter (`param`) can be entered directly (0x10, 250, etc.), or chosen from the Library's list of defined parameters.  If the default device address is used, the optional `addr` value may be omitted.  If the function completes without error, it returns 'True' and sets a public, one-byte 'status' code to zero.  Otherwise, it returns 'False' and sets the 'status' to a Library defined error code.

<hr>

In **I2C** mode, the TFMini-Plus functions as an I2C slave device.  The default address is `0x10` (16 decimal), but is user-programable by sending the `SET_I2C_ADDRESS` command and a parameter in the range of `1` to `127`.  The new address will take effect immediately and permanently without sending a `SAVE_SETTINGS` command.

Although I2C address of the device can be changed while in UART communication mode, the value of an address cannot be tested in UART mode. For that, the device must be in I2C communication mode. Then you can scan the I2C bus for the presence of the device's addres. The `TFMPI2C_changeI2C.ino` sketch in the example folder includes a `scanAddr()` function.

If the I2C device address is any other than the default value of `0x10`, that new, non-default address must be included with every subsequent command, includeing `getData()`, as the optional `addr` byte.

The `HARD_RESET` command (Restore Factory Settings) will reset the device to the default address of `0x10`. The `SOFT_RESET` command (System Reset) appears to have no effect on the I2C address.  The `HARD_RESET` command will **not** restore the device to the default, **UART** communication interface mode.  The **only** way to return the device to serial mode is to send the `SET_SERIAL_MODE` command.

Benewake is not forthcoming about the internals of the device, however they did share this:
>Some commands that modify internal parameters are processed within 1ms.  Some commands (that) require the MCU to communicate with other chips may take several ms.  And some commands, such as saving configuration and restoring the factory (default settings) need to erase the FLASH of the MCU, which may take several hundred ms.

Also, according to Benewake:
>The measuring frequency (frame-rate) of the module should be 2.5 times larger than the I2C reading frquency.<br />
>The I2C reading frequency should not exceed 100Hz.<br />

Benewake says the data frame-rate is limited to 1KHz, which would suggest a 400Hz data sampling limit in **I2C** mode.  But Benewake also says data sampling should not exceed 100Hz.  They don't say why; but you might keep those supposed limitations in mind while you are using the **I2C** interface.

Frame-rate changes should be followed by a `SAVE_SETTINGS` command or may be lost when power is removed.  There is no way to determine what the data frame-rate is actually set to.

<hr>

Also included in the repository are:
<br />&nbsp;&nbsp;&#9679;&nbsp; An Arduino sketch "TFMPI2C_example.ino" in the Example folder.
<br />&nbsp;&nbsp;&#9679;&nbsp; An Arduino sketch "TFMPI2C_changeI2C.ino" in the Example folder.
<br />&nbsp;&nbsp;&#9679;&nbsp; Recent copies of manufacturer's Datasheet and Product Manual in Documents.
<br />&nbsp;&nbsp;&#9679;&nbsp; A folder containing the Datasheet and Product Manual for the TFMini-S
<br />&nbsp;&nbsp;&#9679;&nbsp; General information regarding Time of Flight distance sensing and the Texas Instruments OPT3101 module in Documents in the TI OPT3101 sub-folder.

All of the code for this Library is richly commented to assist with understanding and in problem solving.
