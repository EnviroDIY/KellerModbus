/*****************************************************************************
Keller/GetValues.ino

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
#include "KellerModbus.h"


// ---------------------------------------------------------------------------
// Set up the sensor specific information
//   ie, pin locations, addresses, calibrations and related settings
// ---------------------------------------------------------------------------

// Define the sensor type
kellerModel model = Acculevel_kellerModel;

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

// Construct the Keller modbus instance
keller sensor;
bool success;


// ---------------------------------------------------------------------------
// Working Functions
// ---------------------------------------------------------------------------

// Give values to variables;
// byte modbusSlaveID = modbusAddress;
// byte _slaveID = modbusSlaveID;


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
    sensor.begin(model, modbusAddress, &modbusSerial, DEREPin);

    // Turn on debugging
    // sensor.setDebugStream(&Serial);

    // Start up note
    Serial.println("Keller Acculevel (or other Series 30, Class 5, Group 20 sensor)");

    Serial.println("Waiting for sensor and adapter to be ready.");
    delay(500);

    Serial.print("Device Address, as integer: ");
    Serial.println(sensor.getSlaveID());

    Serial.print("Serial Number: ");
    Serial.println(sensor.getSerialNumber());

    Serial.println("Starting sensor measurements");

    Serial.println("Allowing sensor to stabilize..");
    for (int i = 5; i > 0; i--)     // 4 second delay
    {
        Serial.print(i);
        delay (250);
        Serial.print(".");
        delay (250);
        Serial.print(".");
        delay (250);
        Serial.print(".");
        delay (250);
    }
    Serial.println("\n");

    Serial.print("Temp(°C)  ");
    Serial.print("Pressure(bar)  ");
    Serial.print("Depth (mWC)");
    Serial.println();

}

// Initialize variables
float waterPressureBar = -9999.0;
float waterTempertureC = -9999.0;
float waterDepthM = -9999.0;

// ---------------------------------------------------------------------------
// Main loop function
// ---------------------------------------------------------------------------
void loop()
{
    sensor.getValues(waterPressureBar, waterTempertureC);
    waterDepthM = sensor.calcWaterDepthM(waterPressureBar, waterTempertureC);  // float calcWaterDepthM(float waterPressureBar, float waterTempertureC)

    Serial.print(waterTempertureC);
    Serial.print("      ");
    Serial.print(waterPressureBar, 7);
    Serial.print("      ");
    Serial.print(waterDepthM, 6);
    Serial.println();

    delay(1500);

}
