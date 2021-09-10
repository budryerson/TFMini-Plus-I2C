/* File Name: TFMPI2C_example.ino
 * Developer: Bud Ryerson
 * Inception: 29JAN2019
 * Last work: 10SEP2021
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
 * The TFMini-Plus is switched from the default UART (serial)
 * interface mode to I2C Mode by a command from the TFMPlus library
 * or the factory supplied GUI.  The device will remain in I2C Mode
 * regardless of any reset commands or power cycling until the I2C
 * command 'SET_SERIAL_MODE' is sent.
 *
 * NOTE: If your your Arduino is capable of operating at "Fast" I2C
 * clock speeds (400KHz), then remove comment slashes from line #75.
 *
 * There are only two important functions: 'getData' and 'sendCommand'
 *
 *  NOTE: By default the I2C device address is set to 0x10. If you need
 *  to address multiple devices or need to change the default address for
 *  any reason, your code must thereafter include an unsigned, 8-bit
 *  'addr' value at the end of every call to 'getData()' or 'sendCommand()'.
 *
 *  'getData( dist, flux, temp, addr)' passes back measurement values in
 *  three signed, 16-bit variables:
 *     dist - distance to target in centimeters: 10cm - 1200cm
 *     flux - strength, voltage or quality of returned signal
 *            in arbitrary units: -1, 0 - 32767
 *     temp - chip temperature in Celsius: -25°C to 125°C
 *  and sends...
 *     addr - an optional unsigned 8-bit address value.
 *
 *  - If the default device address is used, the 'addr' value may be
 *  mitted.  Otherwise, a correct 'addr' value always must be sent.
 *  - If a function completes without error, it returns 'True' and sets
 *  a public, one-byte 'status' code to zero ('READY').  Otherwise, it
 *  returns 'False' and sets the 'status' code to a library defined error.
 *  
 *  NOTE: 'getData( dist)' is a simplified function that passes back
 *  distance data only; but assumes use of the default I2C address.
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
#include "printf.h"   // Modified to support Intel based Arduino
                      // devices such as the Galileo. Download from:
                      // https://github.com/spaniakos/AES/blob/master/printf.h

#include <TFMPI2C.h>  // TFMini-Plus I2C Library v1.6.0
TFMPI2C tfmP;         // Create a TFMini-Plus I2C object

void setup()
{
    Serial.begin( 115200);   // Initialize terminal serial port
    printf_begin();          // Initialize printf library.
    delay(20);

    Wire.begin();            // Initialize two-wire interface
//    Wire.setClock( 400000);  // Set I2C bus speed to 'Fast' if
                               // your Arduino supports 400KHz.

    printf( "\r\nTFMPlus I2C Library Example - 10SEP2021\r\n");  // say 'hello'

    // Send some example commands to the TFMini-Plus
    // - - Perform a system reset - - - - - - - - - - -
    printf( "System reset: ");
    if( tfmP.sendCommand( SOFT_RESET, 0))
    {
        printf( "passed.\r\n");
    }
    else tfmP.printReply();
    // - - Display the firmware version - - - - - - - - -
    printf( "Firmware version: ");
    if( tfmP.sendCommand( GET_FIRMWARE_VERSION, 0))
    {
        printf( "%1u.", tfmP.version[ 0]); // print three single numbers
        printf( "%1u.", tfmP.version[ 1]); // each separated by a dot
        printf( "%1u\r\n", tfmP.version[ 2]);
    }
    else tfmP.printReply();
    // - - Set the data frame-rate to 20 - - - - - - - - -
    printf( "Data-Frame rate: ");
    if( tfmP.sendCommand( SET_FRAME_RATE, FRAME_20))
    {
        printf( "%2uHz.\r\n", FRAME_20);
    }
    else tfmP.printReply();
    // - - - - - - - - - - - - - - - - - - - - - - - -

/*  // - - - - - - - - - - - - - - - - - - - - - - - -
    // The next two commands may be used to switch the device
    // into UART (serial) mode.  This sketch will no longer
    // receive I2C data.  The 'TFMPlus_example' sketch in the
    // TFMPlus Library can be used to switch the device back
    // to I2C mode.
    // Don't forget to switch the cables, too.
    // - - - - - - - - - - - - - - - - - - - - - - - -

    // - - Set Serial Mode - - - - - - - - - - -
    printf( "Set Serial Mode: ");
    if( tfmP.sendCommand( SET_SERIAL_MODE, 0))
    {
        printf( "mode set.\r\n");
    }
    else tfmP.printReply();
    printf( "Save Settings: ");
    if( tfmP.sendCommand( SAVE_SETTINGS, 0))
    {
        printf( "saved.\r\n");
    }
    else tfmP.printReply();
*/

    delay(500);            // And wait for half a second.
}

// Initialize data variables
int16_t tfDist = 0;       // Distance to object in centimeters
int16_t tfFlux = 0;       // Signal strength or quality of return signal
int16_t tfTemp = 0;       // Internal temperature of Lidar sensor chip

// = = = = = = = = = =  MAIN LOOP  = = = = = = = = = =
void loop()
{
    tfmP.getData( tfDist, tfFlux, tfTemp); // Get a frame of data
    if( tfmP.status == TFMP_READY)         // If no error...
    {
        printf( "Dist:%04icm ", tfDist);   // display distance,
        printf( "Flux:%05i ",   tfFlux);   // display signal strength/quality,
        printf( "Temp:%2i%s",  tfTemp, "°C" );   // display temperature,
        printf( "\r\n");                   // end-of-line.
    }
    else tfmP.printFrame(); // Otherwise, display error and data frame
    
    delay(50);    //  Run loop at approximately 20Hz.
}
// = = = = = = = = =  End of Main Loop  = = = = = = = = =
