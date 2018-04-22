/*
KellerModbus.cpp

Written by Anthony Aufdenkampe

Only tested the Acculevel
- a Keller Series 30, Class 5, Group 20 sensor
- Software version 5.20-12.28 and later (i.e. made after the 2012 in the 28th week)

*/

#include "KellerModbus.h"


//----------------------------------------------------------------------------
//                          PUBLIC SENSOR FUNCTIONS
//----------------------------------------------------------------------------


// This function sets up the communication
// It should be run during the arduino "setup" function.
// The "stream" device must be initialized and begun prior to running this.
bool keller::begin(byte modbusSlaveID, Stream *stream, int enablePin)
{
    // Give values to variables;
    _slaveID = modbusSlaveID;
    // Start up the modbus instance
    bool success = modbus.begin(modbusSlaveID, stream, enablePin);
    return success;
}
bool keller::begin(byte modbusSlaveID, Stream &stream, int enablePin)
{return begin(modbusSlaveID, &stream, enablePin);}


// This gets the modbus slave ID.
// For Keller, slaveID is in register 0x020D (525), or regNum = 0x020D
// regType = 0x03 for all Keller Modbus Register Read functions
// NOTE: NOT YET WORKING
// byte keller::getSlaveID(void)
// {
//    return modbus.byteFromRegister(0x03, 0x020D, 0); // byte byteFromRegister(byte regType, int regNum, int byteNum)
//    return modbus.uint16FromRegister(0x03, 0x020D);// uint16_t uint16FromRegister(byte regType, int regNum, endianness endian=bigEndian);

// Uses the transparent DevAddr = 250 (or 0xFA in HEX)
//     byte command[8] = {0xFA, 0x03, 0x02, 0x0D, 0x00, 0x01, 0x23, 0x3A};
//     int respSize = modbus.sendCommand(command, 8);
//
//     if (respSize == 9) return modbus.responseBuffer[4];
//     else return -9999;  // This is the default address
// }


// This sets a new modbus slave ID
// For Keller, The slaveID is in register 0x020D (525), or regNum = 0x020D
// NOTE: NOT YET WORKING
// bool keller::setSlaveID(byte newSlaveID)
// {
//     return modbus.byteToRegister(0x020D, 2, newSlaveID); //bool byteToRegister(int regNum, int byteNum, byte value, bool forceMultiple=false);
// }


// This gets the instrument serial number as a 32-bit unsigned integer (as specified by Keller)
// For Keller, Serial number is in holding registers 0x0202 and 0x0203 (2304) and occupies 4 registers (8 characters)
long keller::getSerialNumber(void)
{
    return modbus.uint32FromRegister(0x03, 0x0202); // uint32_t uint32FromRegister(byte regType, int regNum, endianness endian=bigEndian);
}


// This gets the hardware and software version of the sensor
// This data begins in holding register 0x0700 (1792) and continues for 2 registers
// bool yosemitech::getVersion(float &hardwareVersion, float &softwareVersion)
// {
//     // Parse into version numbers
//     // These aren't actually little endian responses.  The first byte is the
//     // major version and the second byte is the minor version.
//     if (modbus.getRegisters(0x03, 0x0700, 2))
//     {
//         hardwareVersion = modbus.byteFromFrame(3) + (float)modbus.byteFromFrame(4) / 100;
//         softwareVersion = modbus.byteFromFrame(5) + (float)modbus.byteFromFrame(6) / 100;
//         return true;
//     }
//     else return false;
// }


// This gets values back from the sensor
// Uses Keller Process Value Read Range (0x0100) 32bit floating point,
// which is Same as 0x0000 .. 0x000B but different mapping for accessing data in one cycle (e.g. P1 and TOB1)
// P1 is in register 0x0100 & TOB1 (Temperature of sensor1) is in 0x0102
bool keller::getValues(float &valueP1, float &valueTOB1)
{
    // Set values to -9999 and error flagged before asking for the result
    valueP1   = -9999;  // Initialize with an error value
    valueTOB1 = -9999;  // Initialize with an error value

    modbus.getRegisters(0x03, 0x0100, 4);

    valueP1   = modbus.float32FromFrame(littleEndian, 3);   // Pressure of sensor1 [bar?]
    valueTOB1 = modbus.float32FromFrame(littleEndian, 7);   // Temperature of sensor1 [°C]
}
