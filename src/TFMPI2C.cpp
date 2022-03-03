/* File Name: TFMPI2C.cpp
 * Developer: Bud Ryerson
 * Date:      14 JAN 2021
 * Version:   1.7.2
 * Described: Arduino Library for the Benewake TFMini-Plus LiDAR sensor
 *            configured for the I2C interface
 *
 * Default settings for the TFMini-Plus I2C are:
 *    0x10  -  slave device address
 *    100Mhz - bus clock speed
 *    100Hz  - data frame-rate
 *    Centimeter - distance measurement format
 *    Celsius - temperature measurement scale
 *
 *  There are only two important functions: `getData` and `sendCommand`
 *
 *  NOTE: By default the I2C device address is set to 0x10. If you need
 *  to address multiple devices or need to change the default address for
 *  any reason, your code must thereafter include an unsigned, 8-bit
 *  `addr` value at the end of every call to `getData()` or `sendCommand()`.
 *
 *  `getData( dist, flux, temp, addr)` passes back measurement values in
 *  three signed, 16-bit variables:
 *     dist - distance to target in centimeters: 10cm - 1200cm
 *     flux - strength, voltage or quality of returned signal
 *            in arbitrary units: -1, 0 - 32767
 *     temp - chip temperature in degrees Celsius: -25°C to 125°C
 *  and sends;
 *     addr - optional unsigned 8-bit address value.
 *  - If default device address is used unchanged, the `addr` value may
 *  be omitted.  Otherwise, a correct `addr` value always must be sent.
 *  - If the function completes without error, it returns `True` and sets
 *  a public, one-byte `status` code to zero.  Otherwise. it returns
 *  `False` and sets the `status` code to a library defined error code.
 *  
 *  NOTE: This library also includes a two simple `getData( dist)` functions
 *  that pass back distance data only. One assumes the default I2C address
 *  and the other requires an explicit address.
 *
 * `sendCommand( cmnd, param, addr)`
 *  The function sends an unsigned 32-bit command and an unsigned 32-bit
 *  parameter value plus an optional, unsigned, 8-bit I2C device address.
 *  If the function completes without error it returns `true` and sets
 *  a public one-byte `status` code to zero.  Otherwise it returns `false`
 *  and sets the `status` code to a library defined error code.
 *
 *  NOTE: The `cmmd` value must be chosen from the library's list of defined
 *  commands. Parameters can be entered directly (0x10, 250, etc.) or chosen
 *  from the library's lists of defined parameters.
 
 * v1.4.0 - 15JUN20 - Changed all data variables from unsigned
             to signed integers.  Defined abnormal data codes
             as per TFMini-S Product Manual
          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
            Dist | Strength    |  Comment
             -1    Other value   Strength ≤ 100
             -2    -1            Signal strength saturation
             -4    Other value   Ambient light saturation
           -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
 * v1.4.1 - 22JUL20 - Fixed bug in sendCommand() checksum calculation
          - Changed two `printf()`s to `Serial.print()`s
	        - Fixed printReply() to show data from `reply` rather than `frame`
 * v1.4.2 - 09AUG20- Added `true` parameter to `Wire.endTransmission()`
             and added explicit I2C address to short getData()
             functions in TFMPI2C.cpp.
 * v1.4.3 - 21AUG20 - Deleted all `Wire.endTransmission()` functions
             after a `Wire.requestFrom(true)` in TFMPI2C.cpp.
 * v1.5.0 - 03SEP20 - Added recoverI2CBus() function to free locked I2C bus
 * v1.5.1 - 19MAY21 - Changed command parameter `FRAME_5` to correct value.
            It was set to 0x0003.  Now it's set to 0x0005
 * v1.6.0 - 06SEP21 - Corrected (reversed) Enable/Disable commands in `TFMPI2C.h`
 * v1.7.0 - 03NOV21 - No longer need pin numbers for `recoverI2C()` routine.
            Instead use pins defined by the `variants.h` file for each board.
 * v1.7.1 - 16NOV21 - Shortened bus recovery delay to 300ms.
            Corrected some typos in comments.
 * v1.7.2 - 13JAN21 - Eliminated all delays in bus recovery
 */

#include <TFMPI2C.h>       //  TFMini-Plus I2C library header
#include <Wire.h>          //  Arduino I2C/Two-Wire Library

// Constructor/Destructor
TFMPI2C::TFMPI2C(){}
TFMPI2C::~TFMPI2C(){}

// = = = = =  GET A FRAME OF DATA FROM THE DEVICE  = = = = = = = = = =
//
bool TFMPI2C::getData( int16_t &dist, int16_t &flux, int16_t &temp, uint8_t addr)
{
    // `frame` data array is declared in TFMPI2C.h
    status = TFMP_READY;    // clear status of any error condition

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 0 - Command device to ready distance data in centimeters
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // The device can also return data in millimeters, but its
    // resolution is only 5mm (o.5cm) and its accuracy is ±5cm.
    if( sendCommand( I2C_FORMAT_CM, 0, addr) != true) return false;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 1 - Get data from the device.
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Request one data-frame from the slave device address
    // and close the I2C interface.
    Wire.requestFrom( (int)addr, TFMP_FRAME_SIZE, (int)true);

    memset( frame, 0, sizeof( frame));     // Clear the data-frame buffer.
    for( uint8_t i = 0; i < TFMP_FRAME_SIZE; i++)
    {
      if( Wire.peek() == -1)     // If there is no next byte...
      {
        status = TFMP_I2CREAD;   // then set error...
        return false;            // and return "false."
      }
      else frame[ i] = uint8_t( Wire.read());
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 2 - Perform a checksum test.
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Clear the `chkSum` variable declared in `TFMPI2C.h`
    chkSum = 0;
    // Add together all bytes but the last.
    for( uint8_t i = 0; i < ( TFMP_FRAME_SIZE - 1); i++) chkSum += frame[ i];
    //  If the low order byte does not equal the last byte...
    if( ( uint8_t)chkSum != frame[ TFMP_FRAME_SIZE - 1])
    {
      status = TFMP_CHECKSUM;  // then set error...
      return false;            // and return "false."
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 3 - Interpret the frame data
    //          and if okay, then go home
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    dist = frame[ 2] + ( frame[ 3] << 8);
    flux = frame[ 4] + ( frame[ 5] << 8);
    temp = frame[ 6] + ( frame[ 7] << 8);
    // Convert temp code to degrees Celsius.
    temp = ( temp >> 3) - 256;
    // Convert Celsius to degrees Fahrenheit
    // temp = uint8_t( temp * 9 / 5) + 32;
    
    // - - Evaluate Abnormal Data Values - -
    // Values are from the TFMini-S Product Manual
    // Signal strength <= 100
    if( dist == -1) status = TFMP_WEAK;
    // Signal Strength saturation
    else if( flux == -1) status = TFMP_STRONG;
    // Ambient Light saturation
    else if( dist == -4) status = TFMP_FLOOD;
    // Data is apparently okay
    else status = TFMP_READY;
    
    if( status != TFMP_READY) return false;
    else return true;
}

// Pass back data using default I2C address.
bool TFMPI2C::getData( int16_t &dist, int16_t &flux, int16_t &temp)
{
  return getData( dist, flux, temp, TFMP_DEFAULT_ADDRESS);
}

// Pass back only distance data using given I2C address.
bool TFMPI2C::getData( int16_t &dist, uint8_t addr)
{
  static int16_t flux, temp;
  return getData( dist, flux, temp, addr);
}

// Pass back only distance data using default I2C address.
bool TFMPI2C::getData( int16_t &dist)
{
  static int16_t flux, temp;
  return getData( dist, flux, temp, TFMP_DEFAULT_ADDRESS);
}
//
// - - - - - - End of Get a Frame of Data  - - - - - - - - - -


// = = = = =  SEND A COMMAND TO THE DEVICE  = = = = = = = = = =0
//
// Create a proper command byte array, send the command,
// get a response, and return the status.
bool TFMPI2C::sendCommand( uint32_t cmnd, uint32_t param, uint8_t addr)
{
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 1 - Build the command data to send to the device
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // `reply` data array, `replyLen`, `cmndLen` and `cmndData`
    // variables are all declared in TFMPI2C.h

    // Clear the send command data array.
    memset( cmndData, 0, sizeof( cmndData));
    memcpy( &cmndData[ 0], &cmnd, 4);   // Copy 4 bytes of data: reply length,
                                        // command length, command number and
                                        // a one byte parameter, all encoded as
                                        // a 32 bit unsigned integer.

    replyLen = cmndData[ 0];            // Save the first byte as reply length.
    cmndLen = cmndData[ 1];             // Save the second byte as command length.
    cmndData[ 0] = 0x5A;                // Set the first byte to the header character.

    if( cmnd == SET_FRAME_RATE)           // If the command is to Set Frame Rate...
    {
      memcpy( &cmndData[ 3], &param, 2);  // add the 2 byte Frame Rate parameter.
    }
    else if( cmnd == SET_BAUD_RATE)       // If the command is to Set Baud Rate...
    {
      memcpy( &cmndData[ 3], &param, 4);  // add the 3 byte Baud Rate parameter.
    }
    else if( cmnd == SET_I2C_ADDRESS)     // If the command to set I2C address...
    {
      memcpy( &cmndData[ 3], &param, 1);  // copy the 1 byte Address parameter.
    }

    // Create a checksum byte for the command data array.
    // chkSum variable declared in `TFMPI2C.h`
    chkSum = 0;
    // Add together all bytes but the last...
    for( uint8_t i = 0; i < ( cmndLen - 1); i++) chkSum += cmndData[ i];
    // and save it as the last byte of command data.
    cmndData[ cmndLen - 1] = uint8_t( chkSum);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 2 - Send the command data array to the device
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Begin transmission to the I2C slave device
    Wire.beginTransmission( addr);
    // Queue command data array for transmission to the I2C device
    if( Wire.write( cmndData, (size_t)cmndLen) != cmndLen)
    {
        status = TFMP_I2CLENGTH;  // then set status code...
        Wire.write( 0);           // Put a zero in the xmit buffer.
        Wire.endTransmission( true);   // Send and Close the I2C interface.
        return false;             // and return "false."
    }

    // Transmit the bytes and a stop message to release the I2C bus.
    if( Wire.endTransmission( true) != 0)  // If write error...
    {
        status = TFMP_I2CWRITE;       // then set status code...
        return false;                 // and return "false."
    }

    // + + + + + + + + + + + + + + + + + + + + + + + + +
    // If no reply data expected, then go home. Otherwise,
    // wait for device to process the command and continue.
    if( replyLen == 0) return true;
        else delay( 500);
    // + + + + + + + + + + + + + + + + + + + + + + + + +

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 3 - Get command reply data back from the device.
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    //  An I2C address change will take effect immediately
    //  so use the new `param` address for the reply.
    if( cmnd == SET_I2C_ADDRESS) addr = uint8_t(param);

    // Request reply data from the device and
    // close the I2C interface.
    Wire.requestFrom( (int)addr, (int)replyLen, (int)true);

    memset( reply, 0, sizeof( reply));   // Clear the reply data buffer.
    for( uint8_t i = 0; i < replyLen; i++)
    {
      reply[ i] = (uint8_t)Wire.read();
    }
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 4 - Perform a checksum test.
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Clear the `chkSum` variable declared in `TFMPI2C.h`
    chkSum = 0;
    // Add together all bytes but the last...
    for( uint8_t i = 0; i < ( replyLen - 1); i++) chkSum += reply[ i];
    // If the low order byte of the Sum does not equal the last byte...
    if( reply[ replyLen - 1] != (uint8_t)chkSum)
    {
      status = TFMP_CHECKSUM;  // then set error...
      return false;            // and return "false."
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 5 - Interpret different command responses.
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if( cmnd == GET_FIRMWARE_VERSION)
    {
        version[ 0] = reply[5];  // set firmware version.
        version[ 1] = reply[4];
        version[ 2] = reply[3];
    }
    else
    {
        if( cmnd == SOFT_RESET ||
            cmnd == HARD_RESET ||
            cmnd == SAVE_SETTINGS )
        {
            if( reply[ 3] == 1)      // If PASS/FAIL byte not zero ...
            {
                status = TFMP_FAIL;  // set status `FAIL`...
                return false;        // and return `false`.
            }
        }
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 6 - Go home
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    return true;
}

// Send a command using default I2C address
bool TFMPI2C::sendCommand( uint32_t cmnd, uint32_t param)
{
  return sendCommand( cmnd, param, TFMP_DEFAULT_ADDRESS);
}
//
// - - - - - - -  End of Send a Command  - - - - - - - - - - - -

// = = = = = = =   RECOVER I2C BUS   = = = = = = = = = =
// An I2C device that quits unexpectedly can leave the I2C bus hung,
// waiting for a transfer to finish.  This function bypasses the Wire
// library and sends 8 phony clock cycles, a NAK, and a STOP signal to
// the SDA and SCL pin numbers.  This flushes any I2C data transfer
// that had been in progress.  It concludes by calling `Wire.begin()`.
//
void TFMPI2C::recoverI2CBus( uint8_t dataPin, uint8_t clockPin)
{
    // try I2C bus recovery at 100kHz: 5us high, 5us low
    // keep SDA high during recovery    
    pinMode( dataPin, OUTPUT);
    digitalWrite( dataPin, HIGH);

    pinMode( clockPin, OUTPUT);
    // 9th cycle acts as NACK
    for (int i = 0; i < 10; i++)
    {
      digitalWrite( clockPin, HIGH);
      delayMicroseconds( 5);
      digitalWrite( clockPin, LOW);
      delayMicroseconds( 5);
    }

    //STOP signal: SDA low to high while CLK high
    digitalWrite( dataPin, LOW);
    delayMicroseconds( 5);
    digitalWrite( clockPin, HIGH);
    delayMicroseconds( 2);
    digitalWrite( dataPin, HIGH);
    delayMicroseconds( 2);

    // return pins to INPUT mode
    pinMode( dataPin, INPUT);
    pinMode( clockPin, INPUT);
    //  restore Wire library
    Wire.begin();
}
//
//  Recover I2C bus using default pin numbers
//  defined in every board's 'variants.h` file.
void TFMPI2C::recoverI2CBus()
{
    Serial.println( "Recover default I2C bus.");
    recoverI2CBus( PIN_WIRE_SDA, PIN_WIRE_SCL);

    // If the Arduino has a second I2C interface...
    #if WIRE_INTERFACES_COUNT > 1
        Serial.println( "Second I2C bus detected.");
        recoverI2CBus( PIN_WIRE1_SDA, PIN_WIRE1_SCL);
    #endif
}
//
// - - - - -  End of Recover I2C Bus function  - - - - - -


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - -   The following are for testing purposes    - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Called by either `printFrame()` or `printReply()`
// Print status condition either `READY` or error type
void TFMPI2C::printStatus()
{
    Serial.print(" Status: ");
    if( status == TFMP_READY)          Serial.print( "READY");
    else if( status == TFMP_SERIAL)    Serial.print( "SERIAL");
    else if( status == TFMP_HEADER)    Serial.print( "HEADER");
    else if( status == TFMP_CHECKSUM)  Serial.print( "CHECKSUM");
    else if( status == TFMP_TIMEOUT)   Serial.print( "TIMEOUT");
    else if( status == TFMP_PASS)      Serial.print( "PASS");
    else if( status == TFMP_FAIL)      Serial.print( "FAIL");
    else if( status == TFMP_I2CREAD)   Serial.print( "I2C-READ");
    else if( status == TFMP_I2CWRITE)  Serial.print( "I2C-WRITE");
    else if( status == TFMP_I2CLENGTH) Serial.print( "I2C-LENGTH");
    else if( status == TFMP_WEAK)      Serial.print( "Signal weak");
    else if( status == TFMP_STRONG)    Serial.print( "Signal saturation");
    else if( status == TFMP_FLOOD)     Serial.print( "Ambient light saturation");
    else Serial.print( "OTHER");
   // Serial.println();
}

// Print error type and HEX values
// of each byte in the data frame
void TFMPI2C::printFrame()
{
    printStatus();
    // Print the Hex value of each byte of data
    Serial.print(" Data:");
    for( uint8_t i = 0; i < TFMP_FRAME_SIZE; i++)
    {
      Serial.print(" ");
      Serial.print( frame[ i] < 16 ? "0" : "");
      Serial.print( frame[ i], HEX);
    }
    Serial.println();
}

// Print error type and HEX values of
// each byte in the command response frame
void TFMPI2C::printReply()
{
    printStatus();
    // Print the Hex value of each byte
    for( uint8_t i = 0; i < TFMP_REPLY_SIZE; i++)
    {
      Serial.print(" ");
      Serial.print( reply[ i] < 16 ? "0" : "");
      Serial.print( reply[ i], HEX);
    }
    Serial.println();
}

// This is Prompt for Y/N response
bool TFMPI2C::getResponse()
{
    // Five second timer, return `false`
    // if serial read never occurs
    uint32_t serialTimeout = millis() + 5000;
    static char charIn;
    Serial.print("Y/N? ");
    while( Serial.available() || ( millis() <  serialTimeout))
    {
      charIn = Serial.read();
      if( charIn == 'Y' || charIn == 'y') return true;
          else if( charIn == 'N' || charIn == 'n') return false;
    }
    return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
