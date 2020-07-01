# TFMini-Plus-I2C
### PLEASE NOTE:
With version v1.4.0, data variables are changed from unsigned to signed 16bit integers in order to support `dist` (distance) and `flux` (signal strength) error codes such as -1, -2 & -4. These codes are listed in the TFMini-S Product Manual contained in the **documents** folder.

In the example code, `printStatus()` or `printErrorStatus()` has been replaced with `printFrame()` in response to a failed `getData()` or `printReply()` if responding to `sendCommand()`.
<hr />

### Arduino library for the Benewake TFMini-Plus using I2C communication interface

The **TFMini-S** is said to be compatible with the **TFMini-Plus** and therefore able to use this library.  However, this library is *not compatible* with the **TFMini**, which is a different product with its own command and data structure.

Since hardware version 1.3.5 and firmware version 1.9.0, the TFMini-Plus can be configured to use the **I2C** (two-wire) protocol for its communication interface.  The command to configure the device for **I2C** communication must be sent using the **UART** interface.  Therefore, configuration should be made prior to the device's service installation either by using the TFMini-Plus library or by using a serial GUI test application available from the manufacturer.  Thereafter, this libarary can be used for all further communication with the device.

This library calls the Arduino standard I2C/Two-Wire Library.
<hr />

### Arduino Library Commands

The `getData( dist, flux, temp, addr)` function passes back three, unsigned, 16-bit data values and sends an optional, unsigned, 8-bit address.  If the default device address is used, the optional `addr` value may be omitted.  Otherwise, a correct `addr` value always must be sent.  If the function completes without error, it returns 'True' and sets the public, one-byte 'status' code to zero.  Otherwise, it returns 'False' and sets the 'status' code to a library defined error code.

Measurement data values are passed-back in three, 16-bit, unsigned integer variables:
<br />&nbsp;&nbsp;&#9679;&nbsp; `dist` Distance to target in centimeters. Range: 10 - 1200
<br />&nbsp;&nbsp;&#9679;&nbsp; `flux` Strength or quality of returned signal in arbitrary units. Range: 0 - 65535
<br />&nbsp;&nbsp;&#9679;&nbsp; `temp` Temperature of the device in code. Range: -25°C to 125°C

For further convenience and simplicity, a `getData( dist)` function is included. This function passes back data only to the `dist` variable and requires use of the default I2C address.

The `sendCommand( cmnd, param, addr)` function sends an unsigned, 32-bit command, an unsigned, 32-bit parameter and an optional, unsigned, 8-bit I2C address to the device.  A proper command (`cmnd`) must be chosen from the library's list of defined commands.  A parameter (`param`) can be entered directly (0x10, 250, etc.), or chosen from the Library's list of defined parameters.  If the default device address is used, the optional `addr` value may be omitted.  If the function completes without error, it returns 'True' and sets a public, one-byte 'status' code to zero.  Otherwise, it returns 'False' and sets the 'status' to a Library defined error code.

**An erroneous command or parameter can block communication, and there is no external means of resetting the device to factory defaults.**
<hr>

In **I2C** mode, the TFMini-Plus functions as an I2C slave device.  The default address is `0x10` (16 decimal), but is user-programable by sending the `SET_I2C_ADDRESS` command and a parameter in the range of `1` to `127`.  The new address will take effect immediately and permanently without sending a `SAVE_SETTINGS` command.

Although I2C address of the device can be changed while still in UART communication mode, the value of an I2C address cannot be tested in UART mode. For that, the devic must be in I2C communication mode and then you can scan the I2C bus for the presence of the device's addres. The `TFMPI2C_changeI2C.ino` sketch in the example folder includes a `scanAddr()` function.

If the I2C device address is any other than the default value of `0x10`, that new, non-default address must be included with every subsequent command, includeing `getData()`, as the optional `addr` byte.

The `RESTORE_FACTORY_SETTINGS` command will reset the device to the default address of `0x10`. The `SYSTEM_RESET` command appears to have no effect on the I2C address.  The `RESTORE_FACTORY_SETTINGS` command will **not** restore the device to the default, **UART** communication interface mode.  The **only** way to return the device to serial mode is to send the `SET_SERIAL_MODE` command.

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
<br />&nbsp;&nbsp;&#9679;&nbsp; General information regarding Time of Flight distance sensing and the Texas Instruments OPT3101 module in Documents in the TI OPT3101 sub-folder.

All of the code for this Library is richly commented to assist with understanding and in problem solving.
