/*****************************************************************************
KellerTest.ino

By Anthony Aufdenkampe, 
- 2018-April initially based on YosemitechModbus/GetValues.ino
- 2024-04-16 update based on GroPointModbus/extras/ChangeParity/ChangeParity.ino

For testing Keller functionality

This sketch does not depend on the KellerModbus library, but only on the
SensorModbusMaster library and is based on it's example sketch:
SensorModbusMaster/examples/readWriteRegister/readWriteRegister.ino


*****************************************************************************/

// ---------------------------------------------------------------------------
// Include the base required libraries
// ---------------------------------------------------------------------------
#include <Arduino.h>
#include <AltSoftSerial.h>
#include <SensorModbusMaster.h>

// Turn on debugging outputs (i.e. raw Modbus requests & responsds)
// by uncommenting next line (i.e. `#define DEBUG`)
#define DEBUG


// ==========================================================================
//  Sensor Settings
// ==========================================================================

// Define the sensor's modbus address, or SlaveID
// NOTE: Many user manuals present SlaveID and registers as decimal
// integers, whereas EnviroDIY and most other modbus systems present it in
// hexadecimal form. Use an online "HEX to DEC Converter".
byte defaultModbusAddress = 0x01;  // Keller ships sensors with a default ID of 0x01
// Keller defines the following:
//   Address 0 is reserved for broadcasting.
//   Addresses 1 (default) ...249 can be used for bus mode.
//   Address 250 is transparent and reserved for non-bus mode. Every device can be contacted with this address.
//   Addresses 251...255 are reserved for subsequent developments.

// The Modbus baud rate the sensor uses
int32_t modbusBaud     = 9600;  // 9600 baud is the default for Yosemitech & Keller.


// Sensor Timing. Edit these to explore!
#define WARM_UP_TIME 350  // milliseconds for sensor to respond to commands.

#define STABILIZATION_TIME 100  // milliseconds for readings to stablize.

#define MEASUREMENT_TIME 200  // milliseconds to complete a measurement.


// ==========================================================================
//  Data Logging Options
// ==========================================================================
const int32_t serialBaud = 115200;  // Baud rate for serial monitor

// Define pin number variables
const int sensorPwrPin = 10;  // The pin sending power to the sensor
const int adapterPwrPin = 22; // The pin sending power to the RS485 adapter
const int DEREPin = -1;   // The pin controlling Recieve Enable and Driver Enable
                          // on the RS485 adapter, if applicable (else, -1)
                          // Setting HIGH enables the driver (arduino) to send text
                          // Setting LOW enables the receiver (sensor) to send text

// Construct software serial object for Modbus
AltSoftSerial modbusSerial;  // On Mayfly, requires connection D5 & D6

// Construct a SensorModbusMaster class instance, from
// https://github.com/EnviroDIY/SensorModbusMaster
modbusMaster modbus;


// ==========================================================================
// Working Functions
// ==========================================================================

// A function for pretty-printing the Modbuss Address in Hexadecimal notation,
// from ModularSensors `sensorLocation()`
String prettyprintAddressHex(byte _modbusAddress) {
    String addressHex = F("0x");
    if (_modbusAddress < 0x10) addressHex += "0";
    addressHex += String(_modbusAddress, HEX);
    return addressHex;
}

// Give values to variables;
byte modbusSlaveID = defaultModbusAddress;
byte _slaveID = modbusSlaveID;

// Get modbus slave ID or Sensor Modbus Address
byte getSensorAddress(void) {
    return modbus.byteFromRegister(0x03, 0x020D, 2); 
    // byte byteFromRegister(byte regType, int regNum, int byteNum)
}

// Get sensor serial number
long getSerialNumber(void) {
    return modbus.uint32FromRegister(0x03, 0x0202); 
    // uint32_t uint32FromRegister(byte regType, int regNum, endianness endian=bigEndian);
}

// Calculate water depth from pressure and temperature
float calcWaterDepthM(float waterPressureBar, float waterTempertureC) {
    /// Initialize variables
    float waterPressurePa;
    float waterDensity;
    float waterDepthM;
    const float gravitationalConstant = 9.80665; // m/s2, meters per second squared

    waterPressurePa = 1e5 * waterPressureBar;
    // Water density (kg/m3) from equation 6 from JonesHarris1992-NIST-DensityWater.pdf
    waterDensity =  + 999.84847
                    + 6.337563e-2 * waterTempertureC
                    - 8.523829e-3 * pow(waterTempertureC,2)
                    + 6.943248e-5 * pow(waterTempertureC,3)
                    - 3.821216e-7 * pow(waterTempertureC,4)
                    ;
    waterDepthM = waterPressurePa/(waterDensity * gravitationalConstant);  // from P = rho * g * h

    return waterDepthM;
}


// ==========================================================================
//  Arduino Setup Function
// ==========================================================================
void setup() {

    // Setup power pins
    if (sensorPwrPin > 0) {
        pinMode(sensorPwrPin, OUTPUT);
        digitalWrite(sensorPwrPin, HIGH);
    }
    if (adapterPwrPin > 0) {
        pinMode(adapterPwrPin, OUTPUT);
        digitalWrite(adapterPwrPin, HIGH);
    }
    if (DEREPin > 0) {
        pinMode(DEREPin, OUTPUT);
    }

    // Turn on the "main" serial port for debugging via USB Serial Monitor
    Serial.begin(serialBaud);

    // Turn on your modbus serial port
    modbusSerial.begin(modbusBaud);  // The modbus serial stream - Baud rate MUST be 9600.

    // Setup the modbus instance
    modbus.begin(defaultModbusAddress, modbusSerial, DEREPin);

    // Turn on debugging
    #ifdef DEBUG
        modbus.setDebugStream(&Serial);
    #endif

    // Start up note
    Serial.println("\nChange Parity utility for GroPoint Profile sensors ");

    // Allow the sensor and converter to warm up
    Serial.println("Waiting for sensor and adapter to be ready.");
    Serial.print("  Warm up time (ms): ");
    Serial.println(WARM_UP_TIME);
    Serial.println();
    delay(WARM_UP_TIME);

    // Confirm Modbus Address
    Serial.println("Default modbus address:");
    Serial.print("  integer: ");
    Serial.print(defaultModbusAddress, DEC);
    Serial.print(", hexidecimal: ");
    Serial.println(prettyprintAddressHex(defaultModbusAddress));
    Serial.println();

    // Read Sensor Modbus Address
    Serial.println("Get sensor modbus address.");
    byte id = getSensorAddress();
    Serial.print("  integer: ");
    Serial.print(id, DEC);
    Serial.print(", hexidecimal: ");
    Serial.println(prettyprintAddressHex(id));
    Serial.println();

    // Get Sensor Serial Number
    Serial.print("Serial Number: ");
    Serial.println(getSerialNumber());

    // Get Sensor Firmware Version
    Serial.print("Firmware Version: ");
    Serial.print(modbus.byteFromRegister(0x03, 0x020E, 1));
    Serial.print(".");
    Serial.print(modbus.byteFromRegister(0x03, 0x020E, 2));
    Serial.print("-");
    Serial.print(modbus.byteFromRegister(0x03, 0x020F, 1));
    Serial.print(".");
    Serial.print(modbus.byteFromRegister(0x03, 0x020F, 2));
    Serial.println();

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

    // Calculate Water Depth in Meters
    waterDepthM = calcWaterDepthM(waterPressureBar, waterTempertureC);  // float calcWaterDepthM(float waterPressureBar, float waterTempertureC)
    Serial.print("Depth (mWC) by func: ");
    Serial.println(waterDepthM, 8);


    // Delay between readings
    delay(MEASUREMENT_TIME);
}
