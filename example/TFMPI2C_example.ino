/* File Name: TFMPI2C_example.ino
 * Inception: 29 JAN 2019
 * Last work: 17 FEB 2020
 * Developer: Bud Ryerson
 *
 * Description: This Arduino sketch is used to test the Benewake
 * TFMini-Plus time-of-flight Lidar ranging sensor in the I2C
 * communication interface mode with the default I2C address
 * using the TFMPI2C library.
 *
 * Default settings for the TFMini-Plus I2C are:
 *    0x10  -  slave device address
 *    100Mhz - bus clock speed
 *    100Hz  - data frame-rate
 *    Centimeter - distance measurement format
 *    Celsius - temperature measurement scale
 *
 * The TFMini-Plus is switched to I2C mode from the default UART
 * (serial) interface mode by a command from the TFMPlus library
 * or the factory supplied GUI.  The device will remain in I2C mode
 * regardless of any reset commands or power cycling until the I2C
 * command 'SET_UART_MODE' is sent.
 *
 * NOTE: Remove comment slashes from line# 74 if your your Arduino
 * is capable of operating at "Fast" I2C clock speeds (up to 400KHz).
 *
 * There are only two important functions: 'getData' and 'sendCommand'
 *
 *  NOTE: By default the I2C device address is set to 0x10. If you need
 *  to address multiple devices or need to change the default address for
 *  any reason, your code must thereafter include an unsigned, 8-bit
 *  'addr' value at the end of every call to 'getData()' or 'sendCommand()'.
 *
 *  'getData( dist, flux, temp, addr)' passes back measurement values in
 *  three unsigned, 16-bit variables:
 *     dist - distance to target in centimeters: 10cm - 1200cm
 *     flux - strength, voltage or quality of returned signal
 *            in arbitrary units: 0 - 65535Z
 *     temp - chip temperature in degrees Celsius: -25°C to 125°C
 *  and sends;
 *     addr - optional unsigned 8-bit address value.
 *  - If default device address is used unchanged, the 'addr' value may
 *  be omitted.  Otherwise, a correct 'addr' value always must be sent.
 *  - If the function completes without error, it returns 'True' and sets
 *  a public, one-byte 'status' code to zero.  Otherwise. it returns
 *  'False' and sets the 'status' code to a library defined error code.
 *  
 *  NOTE: This library also includes a simple 'getData( dist)' function that
 *  passes back distance data only. It assumes use of the default I2C address.
 *
 * 'sendCommand( cmnd, param, addr)'
 *  The function sends an unsigned 32-bit command and an unsigned 32-bit
 *  parameter value plus an optional, unsigned, 8-bit I2C device address.
 *  If the function completes without error it returns 'True' and sets
 *  a public one-byte 'status' code to zero.  Otherwise it returns 'False'
 *  and sets the 'status' code to a library defined error code.
 *
 *  NOTE: The 'cmmd' value must be chosen from the library's list of defined
 *  commands. Parameters can be entered directly (0x10, 250, etc.) or chosen
 *  from the library's lists of defined parameters.
 */

#include <Wire.h>     // Arduino standard I2C/Two-Wire Library
#include "printf.h"   // Modified by to support Intel based Arduino
                      // devices such as the Galileo. Download from:
                      // https://github.com/spaniakos/AES/blob/master/printf.h

#include <TFMPI2C.h>  // TFMini-Plus I2C Library v0.2.1
TFMPI2C tfmP;         // Create a TFMini-Plus I2C object

void setup()
{
    Wire.begin();            // Initialize two-wire interface
//    Wire.setClock( 400000);  // Set I2C bus speed to 'Fast' if
                               // your Arduino supports 400KHz.
    Serial.begin( 115200);   // Initialize terminal serial port
    printf_begin();          // Initialize printf library.
    delay(20);

    printf( "\r\nTFMPlus I2C Library Example\r\n");  // say 'hello'

    // Send some example commands to the TFMini-Plus
    // - - - - - - - - - - - - - - - - - - - - - - - -
    printf( "System reset: ");
    if( tfmP.sendCommand( SYSTEM_RESET, 0))
    {
        printf( "passed.\r\n");
    }
    else tfmP.printErrorStatus();
    // - - - - - - - - - - - - - - - - - - - - - - - -
    printf( "Firmware version: ");
    if( tfmP.sendCommand( OBTAIN_FIRMWARE_VERSION, 0))
    {
        printf( "%1u.", tfmP.version[ 0]); // print three single numbers
        printf( "%1u.", tfmP.version[ 1]); // each separated by a dot
        printf( "%1u\r\n", tfmP.version[ 2]);
    }
    else tfmP.printErrorStatus();
    // - - - - - - - - - - - - - - - - - - - - - - - -
    printf( "Data-Frame rate: ");
    if( tfmP.sendCommand( SET_FRAME_RATE, FRAME_250))
    {
        printf( "%2uHz.\r\n", FRAME_250);
    }
    else tfmP.printErrorStatus();
    // - - - - - - - - - - - - - - - - - - - - - - - -

    delay(500);            // And wait for half a second.
}

// Initialize variables
uint16_t tfDist = 0;       // Distance to object in centimeters
uint16_t tfFlux = 0;       // Signal strength or quality of return signal
uint16_t tfTemp = 0;       // Internal temperature of Lidar sensor chip

// = = = = = = = = = =  MAIN LOOP  = = = = = = = = = =
void loop()
{
    delay(50);    //  Run loop at approximately 20Hz.

    tfmP.getData( tfDist, tfFlux, tfTemp); // Get a frame of data
    if( tfmP.status == TFMP_READY)         // If no error...
    {
        printf( "Dist:%04ucm ", tfDist);   // display distance,
        printf( "Flux:%05u ",   tfFlux);   // display signal strength/quality,
        printf( "Temp:%2u\°C",  tfTemp);   // display temperature,
        printf( "\r\n");                   // end-of-line.
    }
    else tfmP.printErrorStatus();          // Otherwise, display error.
}
// = = = = = = = = =  End of Main Loop  = = = = = = = = =
