
/**
 * @file KellerModbus.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY KellerModbus library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Anthony Aufdenkampe and Neil Hancock
 *
 * @brief Contains the KellerModbus class declarations.
 *
 * Tested with Acculevel, Nanolevel
 * - a Keller Series 30, Class 5, Group 20 sensor
 * - Software version 5.20-12.28 and later (i.e. made after the 2012 in the 28th week)
 */

#ifndef KellerModbus_h
#define KellerModbus_h

#include <Arduino.h>
#include <SensorModbusMaster.h>

//

/**
 * @brief The various Keller sensors.
 */
typedef enum kellerModel {
    Acculevel_kellerModel = 0,  ///< Use for a Keller AccuLevel
    Nanolevel_kellerModel = 1,  ///< Use for a Keller NanoLevel
    OTHER                       ///< Use if the sensor model is another model.
} kellerModel;

/**
 * @brief The class for communicating with Keller Sensors
 */
class keller {

 public:

    /**
     * @brief This function sets up the communication.
     *
     * It should be run during the arduino "setup" function.
     * The "stream" device must be initialized prior to running this.
     *
     * @param model The model of the Keller sensor, from #kellerModel
     * @param modbusSlaveID The byte identifier of the modbus slave device.
     * @param stream A pointer to the Arduino stream object to communicate with.
     * @param enablePin A pin on the Arduino processor to use to send an enable signal
     * to an RS485 to TTL adapter. Use a negative number if this does not apply.
     * Optional with a default value of -1.
     * @return True if the starting communication was successful, false if not.
     */
    bool begin(kellerModel model, byte modbusSlaveID, Stream* stream,
               int enablePin = -1);
    /**
     * @brief This function sets up the communication.
     *
     * It should be run during the arduino "setup" function.
     * The "stream" device must be initialized prior to running this.
     *
     * @param model The model of the Keller sensor, from #kellerModel
     * @param modbusSlaveID The byte identifier of the modbus slave device.
     * @param stream A reference to the Arduino stream object to communicate with.
     * @param enablePin A pin on the Arduino processor to use to send an enable signal
     * to an RS485 to TTL adapter. Use a negative number if this does not apply.
     * Optional with a default value of -1.
     * @return True if the starting communication was successful, false if not.
     */
    bool begin(kellerModel model, byte modbusSlaveID, Stream& stream,
               int enablePin = -1);

    /**
     * @brief Gets the modbus slave ID.
     *
     * For Keller, slaveID is in register 0x020D (525), or regNum = 0x020D regType =
     * 0x03 for all Keller Modbus Register Read functions
     *
     * @return *byte* The slave id of the Keller sensor
     *
     * @warning NOT YET WORKING
     */
    byte getSlaveID(void);

    /*
     * @brief Sets the modbus slave ID.
     *
     * For Keller, The slaveID is in register 0x020D (525), or regNum = 0x020D
     *
     * @return *byte* The slave id of the Keller sensor
     *
     * @warning NOT YET WORKING
     */
    //    bool setSlaveID(byte newSlaveID);

    /**
     * @brief Get the instrument serial number as a 32-bit signed integer (as specified
     * by Keller).
     *
     * For Keller, Serial number is in holding registers 0x0202 and 0x0203 (2304) and
     * occupies 4 registers (8 characters)
     *
     * @return *long* The instrument serial number
     */
    long getSerialNumber(void);

    /*
     * @brief Get the hardware and software version of the sensor.
     *
     * This data begins in holding register 0x020E (??) and continues for 2 registers
     *
     * The float variables for the hardware and software versions must be initialized
     * prior to calling this function.
     *
     * The reference (&) is needed when declaring this function so that the function is
     * able to modify the actual input floats rather than create and destroy copies of
     * them.
     *
     * There is no need to add the & when actually using the function.
     *
     * @param hardwareVersion A reference to a float object to be modified with the
     * hardware version.
     * @param softwareVersionA reference to a float object to be modified with the
     * software version.
     * @return True if the hardware and software versions were successfully updated,
     * false if not.
     *
     * @warning NOT YET WORKING
     */
    // bool getVersion(float& hardwareVersion, float& softwareVersion);


    // This gets values back from the sensor
    /**
     * @brief Get values from the sensor
     *
     *  Uses Keller Process Value Read Range (0x0100) 32bit floating point, which is
     * Same as 0x0000 .. 0x000B but different mapping for accessing data in one cycle
     * (e.g. P1 and TOB1) P1 is in register 0x0100 & TOB1 (Temperature of sensor1) is in
     * 0x0102
     *
     * @param valueP1 A reference to a float object to be modified with the pressure
     * value.
     * @param valueTOB1 A reference to a float object to be modified with the
     * temperature value.
     * @return True if the values were successfully updated, false if not.
     */
    bool getValues(float& valueP1, float& valueTOB1);
    /**
     * @brief Get the previously fetched temperature value without communicating with
     * the sensor.
     *
     * @param value A reference to a float object to be modified with the previous
     * temperature value.
     * @return True if the previous temperature value was successfully updated, false if
     * not.
     */
    bool getValueLastTempC(float& value);
    //    bool getValues(float &parmValue, float &tempValue, byte &errorCode);

    /**
     * @brief Calculates water depth based on water pressure and temperature
     *
     * @param waterPressureBar The pressure of the water in bar. This value should be
     * acquired with keller::getValues(float& valueP1, float& valueTOB1)
     * @param waterTempertureC The temperature of the water in degrees celsius. This
     * value should be acquired with keller::getValues(float& valueP1, float& valueTOB1)
     * @return *float* The water depth in meters.
     */
    float calcWaterDepthM(float& waterPressureBar, float& waterTempertureC);

    // This sets a stream for debugging information to go to;

    /**
     * @brief Set a stream for debugging information to go to.
     *
     * @param stream An Arduino stream object
     */
    void setDebugStream(Stream* stream) {
        _debugStream = stream;
    }
    /**
     * @copydoc keller::setDebugStream(Stream* stream)
     */
    void setDebugStream(Stream& stream) {
        _debugStream = &stream;
    }

    /**
     * @brief Un-set the stream for debugging information to go to; stop debugging.
     */
    void stopDebugging(void) {
        modbus.stopDebugging();
    }


 private:
    byte  _model;     ///< the sensor model
    byte  _slaveID;   ///< the sensor slave id
    float _LastTOB1;  ///< the last temperature value from a request.

    modbusMaster modbus;  ///< an internal reference to the modbus communication object.
};

#endif
