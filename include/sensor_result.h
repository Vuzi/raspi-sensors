/**
 * @file sensor_result.h
 * @brief Sensor result class
 * @author Vuzi
 * @version 0.2
 */

#ifndef H_SENSOR_RESULT
#define H_SENSOR_RESULT

#include <chrono>
#include <iostream>

#include "sensor.h"

/**
 * @namespace sensor
 *
 * Name space used to store every class and functions related to the sensors
 */
namespace sensor {

    /**
     * @brief Enum of the different type of result
     */
    enum resultType {
        TEMPERATURE, // Temperature, in Celcius
        PRESSURE,    // Pressure, in hPa
        HUMIDITY,    // Humidity, in %
        LIGHT,       // Light, in lux
        DETECTION,   // Detection, 1 for detected otherwise 0
        PHOTO,       // Photo
        OTHER
    };
    
    typedef enum resultType resultType;

    /**
     * @struct resultValue
     */
    union resultValue {
        int i;
        float f;
        double d;
        // TODO : photo
    };
    
    typedef union resultValue resultValue;

    typedef std::chrono::time_point<std::chrono::system_clock> resultTimestamp;

    /**
     * @class result
     * @brief Class for sensor result
     */
    class result {
        public:
            result(resultType, resultValue);
            ~result();
            
            resultValue getValue();
            resultType getType();
            resultTimestamp getTime();
            long long getTimestamp();

        private:
            resultValue r;     // Data of the result
            resultType t;      // Type of the data
            resultTimestamp date;  // Date of creation
    };

    class sensorException;

    /**
     * @class resultsOrError
     * @brief Class for result or error
     */
    class resultsOrError {
        public:
            resultsOrError();
            resultsOrError(sensorException);
            resultsOrError(std::list<result>);
            ~resultsOrError();
            
            sensorException getError();
            std::list<result> getResults();
            bool hasError();
            bool hasResults();

        private:
            sensorException error;     // Error, if any occured
            std::list<result> results; // Result, if no error
    };
}

#endif // H_SENSOR_RESULT
