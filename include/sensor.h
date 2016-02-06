/**
 * @file sensor.h
 * @brief Base class for all sensors
 * @author Vuzi
 * @version 0.2
 */

#ifndef H_SENSOR
#define H_SENSOR

#include <list>
#include <string>
#include <exception>
#include <stdexcept>
#include <functional>

#include "format.h"

/**
 * @namespace sensor
 *
 * Name space used to store every class and functions related to the sensors
 */
namespace sensor {

    class resultsOrError;
    class result;

    /**
     * @brief Enum of the different error code
     */
    enum sensorErrorCode {
        NONE,
        FILE_ERROR,
        I2C_ERROR,
        GPIO_ERROR,
        INVALID_VALUE
    };
    
    typedef enum sensorErrorCode sensorErrorCode;

    /**
     * @class sensorException
     * @brief Class for sensor exception
     */
    class sensorException: public std::runtime_error {

        public:
            sensorException();
            sensorException(const std::string&, sensorErrorCode);
            virtual const char* what();
            sensorErrorCode code();

        private:
            sensorErrorCode errorCode;
    };

    /**
     * @class sensor
     * @brief Base class for all the sensors
     *
     *  Base class for all the sensors. The class has three default function,
     *  that should be handled by any implementation
     */
    class sensor {

        public:
            /**
             *  @brief Constructor
             */
            sensor(std::string);

            /**
             *  @brief Empty constructor
             */
            sensor();

            /**
             *  @brief Destructor
             */
            virtual ~sensor() {};

            /**
             *  @brief Return the name of the sensor
             *  @return The name of the sensor
             */
            const std::string getName();

            /**
             *  @brief Return the type of the sensor
             *  @return The type of the sensor
             */
            virtual const std::string getType();

            /**
             *  @brief Get the results of the sensor. Will throw exception if the read fails
             *  @return The results of the sensor
             */
            virtual std::list<result> getResults() = 0;

            /**
             * @brief Get a result or an error if any error occured
             *  @return The results of the sensor, or the error
             */
            resultsOrError getResultsOrError();

        protected:
            /**
             *  @brief Prepare the sensor before fetching any result. This method must be called before any result
             *  is requested. May throw exception 
             */
            virtual void prepare() = 0;

            const std::string name; // Name of the sensor

    };

    /**
     * @brief Enum of the different sensor types
     */
    enum sensorType { GPIO, I2C };

    /**
     * @brief Struct of the necessary configuration values for a sensor, using in the file
     * wrapper_sensor.cc
     */
    struct sensorConf {
        std::string type;
        sensorType bus;
        std::function<sensor* (int, const std::string&)> factory;
    };
}

#include "sensor_result.h"

#endif // H_SENSOR

