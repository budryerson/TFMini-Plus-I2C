/* File Name: TFMPI2C_changeI2C.ino 
 * Developer: Bud Ryerson
 * Inception: 16 FEB 2020
 * Last work: 03 SEP 2020
 *
 * Description:
 *  Run an I2C address search.
 *  Use first address found as old address.
 *  Request new I2C address.
 *  Send command to device at old address to accept new device address.
 *  Wait 5 seconds and restart program loop
 */

#include <Wire.h>     // Arduino standard I2C/Two-Wire Library
#include "printf.h"   // Modified to support Intel based Arduino
                      // devices such as the Galileo. Download from:
                      // https://github.com/spaniakos/AES/blob/master/printf.h

#include <TFMPI2C.h>  // TFMini-Plus I2C Library v1.5.0
TFMPI2C tfmP;         // Create a TFMini-Plus I2C object

// Declare variables
int I2C_total, I2C_error;
uint8_t oldAddr, newAddr;

bool scanAddr()
{
    printf("\r\nShow all I2C addresses in Decimal and Hex.");
    printf("\r\nScanning...\r\n");
    I2C_total = 0;
    I2C_error = 0;
    oldAddr = 0x10; // default address
    for( uint8_t x = 1; x < 127; x++ )
    {
        Wire.beginTransmission( x);
        // Use return value of Write.endTransmisstion() to
        // see if a device did acknowledge the I2C address.
        I2C_error = Wire.endTransmission();

        if( I2C_error == 0)
        {
            printf( "I2C device found at address %03i (0x%02X HEX).\r\n", x, x);
            ++I2C_total;   //  Increment for each address returned.
            if( I2C_total == 1) oldAddr = x;
        }
        else if( I2C_error == 4)
        {
            printf( "Unknown I2C error at address %03i (0x%02X HEX).\r\n", x, x);
        }
    }
    //  Display results and return boolean value.
    if( I2C_total == 0)
    {
      printf( "No I2C devices found.");
      return false;
    }
    else return true;
}


void setup()
{
    Wire.begin();            // Initialize two-wire interface
    Serial.begin( 115200);   // Initialize terminal serial port
    printf_begin();          // Initialize printf library.
	  delay(20);

    Serial.flush();          // Flush serial write buffer
    while( Serial.available())Serial.read();  // flush serial read buffer

    // Say hello
    printlnf( "\r\n*****************************\r\n");
    printf( "\r\nThis sketch will command a TFMini-Plus device in");
    printf( "\r\nI2C communications mode to change the I2C address.");
    printf( "\r\nClose terminal window to halt program loop.\r\n");
    delay(1000);
}

// = = = = = = = = = =  MAIN LOOP  = = = = = = = = = =
void loop()
{
     // Scan for I2C addresses and if successful,
     // save first address found as 'old' address.
    if( scanAddr())
    {
      printf( "\r\nFirst I2C address found: %02i.", oldAddr);
      printf( "\r\nEnter new address from 1 to 127 decimal (not Hex): ");

      // parse integer from serial port input,
      // recast as a byte and save as 'new' address
      while( Serial.available() == 0);
      newAddr = uint8_t( Serial.parseInt());
      Serial.println( newAddr);

      // Get Y/N response to continue
      printf( "Change I2C address from %i to %i: ", oldAddr, newAddr);
      if( tfmP.getResponse())
      {
        printf( "\r\n*****************************\r\n");
        printf( "\r\nSet I2C Address: ");
        //  Send command to change address
        if( tfmP.sendCommand( SET_I2C_ADDRESS, newAddr, oldAddr))
        {
            printf( "%02i.\r\n", newAddr);
        }
        else tfmP.printErrorStatus();
      }
      else  // If response is "N"
      {
        printf( "\r\nDo not change I2C address.");
      }
    }
    printf( "\r\n\nProgram will halt for 4 seeconds and restart.");
    printf( "\r\n*****************************\r\n");
    delay( 4000);           // And wait for 4 seconds
}
// = = = = = = = = =  End of Main Loop  = = = = = = = = =

/*

// Test to get data from device in I2C mode
void getI2CData( uint8_t I2C_addr)
{
    // Initialize getI2CData() variables
    static uint16_t tfDist = 0;       // Distance to object in centimeters
    static uint16_t tfFlux = 0;       // Signal strength or quality of return signal
    static uint16_t tfTemp = 0;       // Internal temperature of Lidar sensor chip

    tfmP.getData( tfDist, tfFlux, tfTemp, I2C_addr); // Get a frame of data
    if( tfmP.status == TFMP_READY)         // If no I2C_err...
    {
        printf( "Dist:%04ucm ", tfDist);   // display distance,
        printf( "Flux:%05u ",   tfFlux);   // display signal strength/quality,
        printf( "Temp:%2u\°C",  tfTemp);   // display temperature,
        printf( "\r\n");                   // end-of-line.
    }
    else tfmP.printErrorStatus();          // Otherwise, display I2C_err.
}
*/
