/*
KellerModbus.h

Written by Anthony Aufdenkampe

Only tested the Acculevel
- a Keller Series 30, Class 5, Group 20 sensor
- Software version 5.20-12.28 and later (i.e. made after the 2012 in the 28th week)

*/

#ifndef KellerModbus_h
#define KellerModbus_h

#include <Arduino.h>
#include <SensorModbusMaster.h>


class keller
{

public:

    // This function sets up the communication
    // It should be run during the arduino "setup" function.
    // The "stream" device must be initialized prior to running this.
    bool begin(byte modbusSlaveID, Stream *stream, int enablePin = -1);
    bool begin(byte modbusSlaveID, Stream &stream, int enablePin = -1);

    // This gets the modbus slave ID.
    // NOTE: NOT YET WORKING
    byte getSlaveID(void);

    // This sets a new modbus slave ID
    // NOTE: NOT YET WORKING
//    bool setSlaveID(byte newSlaveID);

    // This gets the instrument serial number as a String
    long getSerialNumber(void);

    // This gets the hardware and software version of the sensor
    // The float variables for the hardware and software versions must be
    // initialized prior to calling this function.
    // The reference (&) is needed when declaring this function so that
    // the function is able to modify the actual input floats rather than
    // create and destroy copies of them.
    // There is no need to add the & when actually using the function.
    // NOTE: NOT YET WORKING
//   bool getVersion(float &hardwareVersion, float &softwareVersion);


    // This gets values back from the sensor
    // The float variables for value1 and value2 and the byte for the error
    // code must be initialized prior to calling this function.
    // This function is overloaded so you have the option of getting:
    // 2 values - This will be the parameter and the temperature,
    //            with the parameter first and the temperature second
    // 2 values and an error code - As two values, but with error code
    bool getValues(float &valueP1, float &valueTOB1);
//    bool getValues(float &parmValue, float &tempValue, byte &errorCode);

    float calcWaterDepthM(float &waterPressureBar, float &waterTempertureC);

    // This sets a stream for debugging information to go to;
    void setDebugStream(Stream *stream){modbus.setDebugStream(stream);}
    void stopDebugging(void){modbus.stopDebugging();}


private:

    byte _slaveID;
    modbusMaster modbus;
};

#endif
