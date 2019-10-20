# TFMini-Plus-I2C
### Arduino library for the Benewake TFMini-Plus using I2C communication interface

<i>The Benewake TFMini-Plus is a unique product, and it is not an enhanced version of the Benewake TFMini. It has its own command and data structures.  This library is not compatible with the TFMini product.</i>

Since hardware version 1.3.5 and firmware version 1.9.0, the TFMini-Plus can be configured to use the **I2C** (two-wire) protocol for its communication interface.  The command to configure the device for **I2C** communication must be sent using the **UART** interface.  Therefore, configuration should be made prior to the device's service installation either by using the TFMini-Plus library or by using a serial GUI test application available from the manufacturer.  Thereafter, this libarary can be used for all further communication with the device.

This library calls the Arduino standard I2C/Two-Wire Library.

### Arduino Library Commands

The `getData( dist, flux, temp, addr)` function passes back three, unsigned, 16-bit data values and sends an optional, unsigned, 8-bit address.  If the default device address is used, the optional `addr` value may be omitted.  Otherwise, a correct `addr` value always must be sent.  If the function completes without error, it returns 'True' and sets the public, one-byte 'status' code to zero.  Otherwise, it returns 'False' and sets the 'status' code to a library defined error code.

Passed-back data consists of three 16-bit measurement values:
<br />&nbsp;&nbsp;&#9679;&nbsp; `dist` Distance to target in centimeters. Range: 10 - 1200
<br />&nbsp;&nbsp;&#9679;&nbsp; `flux` Strength or quality of returned signal in arbitrary units. Range: 0 - 65535
<br />&nbsp;&nbsp;&#9679;&nbsp; `temp` Temperature of the device in code. Range: -25°C to 125°C

The `sendCommand( cmnd, param, addr)` function sends an unsigned, 32-bit command, an unsigned, 32-bit parameter and an optional, unsigned, 8-bit I2C address to the device.  If the function completes without error, it returns 'True' and sets a public, one-byte 'status' code to zero.  Otherwise, it returns 'False' and sets the 'status' to a Library defined error code.  A proper command (`cmnd`) must be chosen from the library's list of defined commands.  A parameter (`param`) can be entered directly (0x10, 250, etc.), or chosen from the Library's list of defined parameters.

**An erroneous command or parameter can block communication, and there is no external means of resetting the device to factory defaults.**

In I2C mode, the TFMini-Plus functions as an I2C slave device.  The default address is `0x10`, but is user-programable by sending the `SET_I2C_ADDRESS` command and a parameter in the range of `0x01` to `0x7F`.  A new device address will not take effect until a subsequent `SAVE_SETTINGS` command is sent.  If the device is set to any address other than the default address, the optional address byte must be included with every function.

Benewake is not forthcoming about the internals of the device, however they did share this:
>Some commands that modify internal parameters are processed within 1ms.  Some commands (that) require the MCU to communicate with other chips may take several ms.  And some commands, such as saving configuration and restoring the factory (default settings) need to erase the FLASH of the MCU, which may take several hundred ms.

Also, according to Benewake:
>1- the measuring frequency of the module should be 2.5 times larger than the IIC reading frquency.
<br />2- the iic reading frequency should not exceed 100hz<br />

Because the data frame-rate is limited to 1000Hz, this condition implies a 400Hz data sampling limit in **I2C** mode.  Benewake says sampling should not exceed 100Hz.  They don't say why; but you might keep that limitation in mind when you consider using the **I2C** interface.

Any change of device settings (frame-rate, baud-rate, etc.) must be followed by a `SAVE_SETTINGS` command or else the modified values may be lost when power is removed.  `SYSTEM_RESET` and `RESTORE_FACTORY_SETTINGS` do not require a `SAVE_SETTINGS` command.

The `SET_I2C_MODE` command does not require a subsequent `SAVE_SETTINGS` command, and the device will remain in **I2C** mode after power has been removed and restored.  Even a `RESTORE_FACTORY_SETTINGS` command will not restore the device to its default, UART communication interface mode.  The only way to return to serial mode is to send the `SET_SERIAL_MODE` command.

The Benewake TFMini-Plus device appears to employ native UART communication internally, with an I2C communication device added downstream from it.  Upon initial application of power, the device will start up and remain in UART mode for about ten seconds.  Brief serial communication, such as a firmware update, can occur during this time.

The **UART** and **I/O mode** communication modes of the TFMini-Plus are not supported by this library.

Also included in the repository are:
<br />&nbsp;&nbsp;&#9679;&nbsp; An Arduino sketch "TFMPI2C_example.ino" in the Example folder.
<br />&nbsp;&nbsp;&#9679;&nbsp; Recent copies of manufacturer's Datasheet and Product Manual in Documents.
<br />&nbsp;&nbsp;&#9679;&nbsp; General information regarding Time of Flight distance sensing and the Texas Instruments OPT3101 module in Documents in the TI OPT3101 sub-folder.

All of the code for this Library is richly commented to assist with understanding and in problem solving.
