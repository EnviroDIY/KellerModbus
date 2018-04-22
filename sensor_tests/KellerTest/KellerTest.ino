/*****************************************************************************
Modified by Anthony Aufdenkampe, from YosemitechModbus/GetValues.ino
2018-April

For testing individual functions in KellerModbus library

*****************************************************************************/

// ---------------------------------------------------------------------------
// Include the base required libraries
// ---------------------------------------------------------------------------
#include <Arduino.h>
#include <AltSoftSerial.h>
#include <SensorModbusMaster.h>
//#include "KellerModbus.h"


// ---------------------------------------------------------------------------
// Set up the sensor specific information
//   ie, pin locations, addresses, calibrations and related settings
// ---------------------------------------------------------------------------

// Define the sensor's modbus address
byte modbusAddress = 0x01;  // The sensor's modbus address, or SlaveID
// Keller defines the following:
//   Address 0 is reserved for broadcasting.
//   Addresses 1 (default) ...249 can be used for bus mode.
//   Address 250 is transparent and reserved for non-bus mode. Every device can be contacted with this address.
//   Addresses 251...255 are reserved for subsequent developments.

// Define pin number variables
const int PwrPin = 22;  // The pin sending power to the sensor *AND* RS485 adapter
const int DEREPin = -1;   // The pin controlling Recieve Enable and Driver Enable
                          // on the RS485 adapter, if applicable (else, -1)
                          // Setting HIGH enables the driver (arduino) to send text
                          // Setting LOW enables the receiver (sensor) to send text

// Construct software serial object for Modbus
AltSoftSerial modbusSerial;  // On Mayfly, requires connection D5 & D6

// Construct the modbus instance
modbusMaster modbus;


// ---------------------------------------------------------------------------
// Working Functions
// ---------------------------------------------------------------------------

// Give values to variables;
byte modbusSlaveID = modbusAddress;
byte _slaveID = modbusSlaveID;


int getSlaveID(void)
{
//    return modbus.byteFromRegister(0x03, 0x020D, 0); // byte byteFromRegister(byte regType, int regNum, int byteNum)
//    return modbus.uint16FromRegister(0x03, 0x020D);// uint16_t uint16FromRegister(byte regType, int regNum, endianness endian=bigEndian);

// Uses the transparent DevAddr = 250 (or 0xFA in HEX)
    byte command[8] = {0xFA, 0x03, 0x02, 0x0D, 0x00, 0x01, 0x23, 0x3A};
    int respSize = modbus.sendCommand(command, 8);

    if (respSize == 9) return modbus.responseBuffer[4];
    else return -9999;  // This is the default address


}

long getSerialNumber(void)
{
    return modbus.uint32FromRegister(0x03, 0x0202); // uint32_t uint32FromRegister(byte regType, int regNum, endianness endian=bigEndian);
}

// float getWaterDepth(float waterPressureBar, float waterTempertureC)
// {
//
// }

// ---------------------------------------------------------------------------
// Main setup function
// ---------------------------------------------------------------------------
void setup()
{

    pinMode(PwrPin, OUTPUT);
    digitalWrite(PwrPin, HIGH);

    if (DEREPin > 0) pinMode(DEREPin, OUTPUT);

    Serial.begin(57600);  // Main serial port for debugging via USB Serial Monitor
    modbusSerial.begin(9600);  // The modbus serial stream - Baud rate MUST be 9600.

    // Start up the modbus sensor
    modbus.begin(modbusAddress, &modbusSerial, DEREPin);

    // Turn on debugging
    // modbus.setDebugStream(&Serial);


    Serial.println("Starting!");

    Serial.print("Device Address, as integer: ");
    Serial.println(getSlaveID());

    Serial.print("Serial Number: ");
    Serial.println(getSerialNumber());

    Serial.println("Started!");
}

// Initialize variables
float waterPressureBar = -9999.0;
float waterPressurePa  = -9999.0;
float waterTempertureC = -9999.0;
float waterDensity = -9999.0;
float waterDepthM = -9999.0;
const float gravitationalConstant = 9.80665; // m/s2, meters per second squared

// ---------------------------------------------------------------------------
// Main loop function
// ---------------------------------------------------------------------------
void loop()
{
    Serial.println("Reading!");

    waterPressureBar = modbus.float32FromRegister(0x03,  0x0100);
    Serial.print("Presure (mbar): ");
    Serial.println(waterPressureBar, 6);

    waterTempertureC = modbus.float32FromRegister(0x03,  0x0102);
    Serial.print("Temperature (C): ");
    Serial.println(waterTempertureC, 4);

    waterPressurePa = 1e5 * waterPressureBar;
    // Water density (kg/m3) from equation 6 from JonesHarris1992-NIST-DensityWater.pdf
    waterDensity =  + 999.84847
                    + 6.337563e-2 * waterTempertureC
                    - 8.523829e-3 * pow(waterTempertureC,2)
                    + 6.943248e-5 * pow(waterTempertureC,3)
                    - 3.821216e-7 * pow(waterTempertureC,4)
                    ;
    waterDepthM = waterPressurePa/(waterDensity * gravitationalConstant);  // from P = rho * g * h

    Serial.print("Depth (mWC): ");
    Serial.println(waterDepthM, 8);


    delay(5000);

}
