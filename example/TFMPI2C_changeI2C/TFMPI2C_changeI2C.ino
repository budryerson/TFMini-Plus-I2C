/* File Name: TFMPI2C_changeI2C.ino 
 * Developer: Bud Ryerson
 * Inception: 16 FEB 2020
 * Last work: 27 SEP 2020 - Replaced every instance of `printf()` with Serial.print().
 *                          because some Arduinos, such as the Due and ESP32, require
 *                          a CR/LF with every instance. Might be better without them.
 *            04 OCT 2021 - Corrected typo in line 136
 *            15 JAN 2022 - Added timeout for unrecognized repsonse to prompt.
 *
 * Description: Run an I2C address search.
 *              Use first address found as old address.
 *              Request new I2C address.
 *              Send command to device at old address to accept new device address.
 *              Wait 4 seconds and restart program loop
 */

#include <Wire.h>     // Arduino standard I2C/Two-Wire Library
#include "printf.h"   // Modified to support Intel based Arduino
                      // devices such as the Galileo. Download from:
                      // https://github.com/spaniakos/AES/blob/master/printf.h

#include <TFMPI2C.h>  // TFMini-Plus I2C Library v1.7.3
TFMPI2C tfmP;         // Create a TFMini-Plus I2C object

// Declare variables
int I2C_total, I2C_error;
uint8_t oldAddr, newAddr;

bool scanAddr()
{
    Serial.println();
    Serial.println( "Show all I2C addresses in Decimal and Hex.");
    Serial.println( "Scanning...");
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
            Serial.print( "I2C device found at address ");
            printAddress( x);
            ++I2C_total;   //  Increment for each address returned.
            if( I2C_total == 1) oldAddr = x;
        }
        else if( I2C_error == 4)
        {
            Serial.print( "Unknown I2C error at address ");
            Serial.println( x);
        }
    }
    //  Display results and return boolean value.
    if( I2C_total == 0)
    {
      Serial.println( "No I2C devices found.");
      return false;
    }
    else return true;
}

// Print address in decimal and HEX
void printAddress( uint8_t adr)
{
    Serial.print( adr);
    Serial.print( " (0x");
    Serial.print( adr < 16 ? "0" : ""); 
    Serial.print( adr, HEX);
    Serial.println( " Hex)");
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
    Serial.println();
    Serial.println( "*****************************");
    Serial.println( "Will scan the I2C bus for all devices");
    Serial.println( "and display the first address found.");
    Serial.println( "Enter a new address in decimal format.");
    Serial.println( "Confirm 'Y/N' in 5 seconds. Default is 'N'.");
    Serial.println( "When done, close this window to halt program.");
    delay(1000);
}

// = = = = = = = = = =  MAIN LOOP  = = = = = = = = = =
void loop()
{
     // Scan for I2C addresses and if successful,
     // save first address found as 'old' address.
    if( scanAddr())
    {
      Serial.println();
      Serial.print( "First I2C address found: ");
      printAddress( oldAddr);
      Serial.print( "Enter new address from 1 to 127 decimal (not Hex): ");

      // parse integer from serial port input,
      // recast as a byte and save as 'new' address
      while( Serial.available() == 0);
      newAddr = uint8_t( Serial.parseInt());
      if( newAddr >= 1 && newAddr <= 127)
      {
          printAddress( newAddr);
          // Get Y/N response to continue
          Serial.print( "Change I2C address from ");
          Serial.print( oldAddr);
          Serial.print( " to ");
          Serial.print( newAddr);
          Serial.print( " ");
          if( tfmP.getResponse())
          {
            Serial.println();
            Serial.println( "*****************************");
            Serial.print( "Set I2C Address: ");
            //  Send command to change address
            if( tfmP.sendCommand( SET_I2C_ADDRESS, newAddr, oldAddr))
            {
                printAddress( newAddr);
            }
            else tfmP.printReply();
          }
          else  // If response is "N"
          {
            Serial.println();
            Serial.println( "No change to I2C address.");
          }
      }
      else
      {
        Serial.println();
        Serial.println( "Entry not recognized.");
      }
    }
    Serial.println();    
    Serial.println( "Program will restart in 5 seconds.");
    Serial.println( "*****************************");
    delay( 4000);           // And wait for 4 seconds
}
// = = = = = = = = =  End of Main Loop  = = = = = = = = =
