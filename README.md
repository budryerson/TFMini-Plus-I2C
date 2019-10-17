# TFMini-Plus-I2C
An Arduino library for the Benewake TFMini-Plus using the optional I2C communication interface

The Benewake TFMini-Plus is a unique product and not an enhanced version of the TFMini. It has its own command and data structure.  This Arduino TFMini-Plus library is not compatible with the TFMini product.

With hardware v1.3.5 and firmware v1.9.0 and above, the TFMini-Plus can be configured to use the **I2C** (two-wire) protocol for its communication interface.  To configure the device for I2C communication, a command must be sent using the UART inteface.  Therefore, the reconfiguation should be made prior to the device's service installation either by using the TFMini-Plus library or by using the serial GUI test application supplied by the manufacturer.  Thereafter, this libarary may be used for further configuartion or communication with this device.

The internal measuring frame-rate is fixed at 4KHz.

The default format for each frame of data consists of three 16-bit measurement values:
<br />&nbsp;&nbsp;&#9679;&nbsp;  Distance to target in centimeters. Range: 10 - 1200
<br />&nbsp;&nbsp;&#9679;&nbsp;  Strength (voltage) or quality of returned signal in arbitrary units. Range: 0 - 65535
<br />&nbsp;&nbsp;&#9679;&nbsp;  Temperature of the device in code. Range: -25°C to 125°C

### Arduino Library Commands

`getData( dist, flux, temp)` passes back three measurement values.  It sets the `status` error code byte and returns a boolean value indicating 'pass/fail'.  If no serial data is received or no header sequence (`0x5959`) is detected within one (1) second, the function sets an appropriate `status` error code and 'fails'.  Given the asynchronous nature of the device, the serial buffer is flushed before reading and the `frame` and `reply` data arrays are zeroed out to delete any residual data.  This helps with valid data recognition and error discrimination.

`sendCommand( cmnd, param)` sends a 32bit command and a 32bit parameter to the device.  It sets the `status` error code byte and returns a boolean 'pass/fail' value.  A proper command (`cmnd`) must be selected from the library's list of twenty defined commands.  A parameter (`param`) may be entered directly as an unsigned number, but it is better to choose from the Library's defined parameters because **an erroneous parameter can block communication and there is no external means of resetting the device to factory defaults.**

In I2C mode, The device will function as an I2C slave device.  The default address is `0x10`.  The address is user-programable by sending the `SET_I2C_ADDRESS` command and a parameter in the range of `0x01` to `0x7F`.  A new address will not take effect until a subsequent `SAVE_SETTINGS` command is sent.  The use of any address other than the default address 

Benewake is not forthcoming about the internals of the device, however they did share this:
>Some commands that modify internal parameters are processed within 1ms.  Some commands require the MCU to communicate with other chips may take several ms.  And some commands, such as saving configuration and restoring the factory need to erase the FLASH of the MCU, which may take several hundred ms.

Also included:
<br />&nbsp;&nbsp;&#9679;&nbsp; An Arduino sketch "TFMP_example.ino" is in the Example folder.
<br />&nbsp;&nbsp;&#9679;&nbsp; Recent copies of the manufacturer's Datasheet and Product Manual are in Documents.
<br />&nbsp;&nbsp;&#9679;&nbsp; Valuable information regarding Time of Flight distance sensing in general and the Texas   Instruments OPT3101 module in particular are in a Documents sub-folder.

All of the code for this library is richly commented to assist with understanding and in problem solving.

According to Benewake:
>1- the measuring frequency of the module should be 2.5 times larger than the IIC reading frquency.
<br />2- the iic reading frequency should not exceed 100hz<br />

Because the Data Frame Rate is limited to 1000Hz, this condition implys a 400Hz data sampling limit in I2C mode.  Benewake says sampling should not exceed 100Hz.  They don't say why; but you might keep that limitation in mind when you consider using the I2C interface.

Any change of device settings (i.e. frame-rate or baud-rate) must be followed by a `SAVE_SETTINGS` command or else the modified values may be lost when power is removed.  `SYSTEM_RESET` and `RESTORE_FACTORY_SETTINGS` do not require a `SAVE_SETTINGS` command.

The `SET_I2C_MODE` command does not require a subsequent `SAVE_SETTINGS` command.  The device will remain in I2C mode after power has been removed and restored.  Even a `RESTORE_FACTORY_SETTINGS` command will not restore the device to its default, UART communication interface mode.  The only way to return to serial mode is with the `SET_SERIAL_MODE` command.

The Benewake TFMini-Plus device appears to employ a native UART communication mode internally, with an additional I2C communication device added onto it.  Upon initial application of power, the device will start up and remain in UART mode for about ten seconds.  Brief, necessary serial communication, for example a firmware update, can occur at this time.

I/O mode is not yet supported in this version of the library.  Please do not attempt to use the I/O commands that are defined in this library's header file.
