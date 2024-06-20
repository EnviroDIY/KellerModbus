/** =========================================================================
 * @example{lineno} GetValues.ino
 * @author Anthony Aufdenkampe
 * @copyright Stroud Water Research Center
 * @license This example is published under the BSD-3 license.
 *
 * @brief For testing individual functions in KellerModbus library.
 * - 2018-April initially based on YosemitechModbus/GetValues.ino
 * - 2024-04-16 update on latest YosemitechModbus/examples/GetValues/GetValues.ino
 *
 * @m_examplenavigation{example_get_values,}
 * @m_footernavigation
 * ======================================================================= */

// ---------------------------------------------------------------------------
// Include the base required libraries
// ---------------------------------------------------------------------------
#include <Arduino.h>
#include "KellerModbus.h"

#include <AltSoftSerial.h>

// Turn on debugging outputs (i.e. raw Modbus requests & responsds)
// by uncommenting next line (i.e. `#define DEBUG`)
#define DEBUG


// ==========================================================================
//  Sensor Settings
// ==========================================================================
// Define the sensor type
kellerModel model = Acculevel_kellerModel;

// Define the sensor's modbus address, or SlaveID
// NOTE: Many user manuals present SlaveID as an integer (decimal),
// whereas EnviroDIY and most other modbus systems present it in hexadecimal form.
// Use an online "HEX to DEC Converter".
byte modbusAddress = 0x01;  // The sensor's modbus address, or SlaveID
// Keller defines the following:
//   Address 0 is reserved for broadcasting.
//   Addresses 1 (default) ...249 can be used for bus mode.
//   Address 250 is transparent and reserved for non-bus mode. Every device can be
//   contacted with this address. Addresses 251...255 are reserved for subsequent
//   developments.

// Define pin number variables
const int PwrPin  = 22;  // The pin sending power to the sensor *AND* RS485 adapter
const int DEREPin = -1;  // The pin controlling Recieve Enable and Driver Enable
                         // on the RS485 adapter, if applicable (else, -1)
                         // Setting HIGH enables the driver (arduino) to send text
                         // Setting LOW enables the receiver (sensor) to send text

// Construct software serial object for Modbus
AltSoftSerial modbusSerial;  // On Mayfly, requires connection D5 & D6

// Construct the modbus instance
modbusMaster modbus;

// Construct the KellerModbus instance
keller sensor;
bool   success;


// ==========================================================================
// Working Functions
// ---------------------------------------------------------------------------

// Give values to variables;
// byte modbusSlaveID = modbusAddress;
// byte _slaveID = modbusSlaveID;


// ---------------------------------------------------------------------------
// Main setup function
// ---------------------------------------------------------------------------
void setup() {
    pinMode(PwrPin, OUTPUT);
    digitalWrite(PwrPin, HIGH);

    if (DEREPin > 0) pinMode(DEREPin, OUTPUT);

    Serial.begin(57600);       // Main serial port for debugging via USB Serial Monitor
    modbusSerial.begin(9600);  // The modbus serial stream - Baud rate MUST be 9600.

    // Start up the modbus sensor
    sensor.begin(model, modbusAddress, &modbusSerial, DEREPin);

// Turn on debugging
#ifdef DEBUG
    sensor.setDebugStream(&Serial);
#endif

    // Start up note
    Serial.println("Keller Acculevel (or other Series 30, Class 5, Group 20 sensor)");

    // Allow the sensor and converter to warm up
    Serial.println("\nWaiting for sensor and adapter to be ready.");
    Serial.print("    Warm up time (ms): ");
    Serial.println(WARM_UP_TIME);
    delay(WARM_UP_TIME);

    // Confirm Modbus Address
    Serial.println("\nSelected modbus address:");
    Serial.print("    integer: ");
    Serial.print(modbusAddress, DEC);
    Serial.print(", hexidecimal: ");
    Serial.println(sensorLocation(modbusAddress));

    Serial.println("Discovered modbus address.");
    Serial.print("    integer: ");
    byte id = sensor.getSlaveID();
    Serial.print(id, DEC);
    Serial.print(", hexidecimal: ");
    // Serial.print(id, HEX);
    Serial.println(sensorLocation(id));

    // Get the sensor serial number
    Serial.println("\nGetting sensor serial number.");
    long SN = sensor.getSerialNumber();
    Serial.print("    Serial Number: ");
    Serial.println(SN);

    // Get the sensor's hardware and software version
    // NOT YET IMPLEMENTED. See KellerTest.ino for work in progress
    // Tell the sensor to start taking measurements
    Serial.println("Starting sensor measurements");

    Serial.println("Allowing sensor to stabilize..");
    for (int i = 5; i > 0; i--)  // 4 second delay
    {
        Serial.print(i);
        delay(250);
        Serial.print(".");
        delay(250);
        Serial.print(".");
        delay(250);
        Serial.print(".");
        delay(250);
    }
    Serial.println("\n");

    // Print table headers
    Serial.print("Temp(°C)  ");
    Serial.print("Pressure(bar)  ");
    Serial.print("Depth (mWC)");
    Serial.println();
}

// Initialize variables
float waterPressureBar = -9999.0;
float waterTempertureC = -9999.0;
float waterDepthM      = -9999.0;

// ---------------------------------------------------------------------------
// Main loop function
// ---------------------------------------------------------------------------
void loop() {
    sensor.getValues(waterPressureBar, waterTempertureC);
    waterDepthM = sensor.calcWaterDepthM(
        waterPressureBar,
        waterTempertureC);  // float calcWaterDepthM(float waterPressureBar, float
                            // waterTempertureC)

    // Print values
    Serial.print(waterTempertureC);
    Serial.print("      ");
    Serial.print(waterPressureBar, 7);
    Serial.print("      ");
    Serial.print(waterDepthM, 6);
    Serial.println();

    delay(1500);
}
