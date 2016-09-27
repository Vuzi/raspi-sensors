/**
 * @file DHT11.h
 * @brief Class for DHT11 sensors. A DHT11 uses the GPIO bus on the raspberry
 * @author Vuzi
 * @version 0.2
 */

#ifndef H_DHT11
#define H_DHT11

#include <chrono>
#include <thread>
#include <list>
#include <unistd.h>

#include "gpio_sensor.h"
#include "sensor_result.h"

#define MAXTIMINGS 85

/**
 * @namespace sensor
 *
 * Name space used to store every class and functions related to the sensors
 */
namespace sensor {

    /**
     * @class DHT11_sensor
     * @brief Class for DHT11 sensors
     */
    class DHT11_sensor : public gpio_sensor {

        public:
            /**
             *  @brief Constructor
             *  @param pin  Value of which pin to read data on
             *  @param name Name of the sensor
             */
            DHT11_sensor(unsigned, std::string);

            /**
             *  @brief Destructor
             */
            virtual ~DHT11_sensor();

            /**
             *  @brief Return the type of the sensor (DHT11)
             */
            const std::string getType();

            /**
             *  @brief Fetch a result. May throw an exception if any error occures
             *
             * @return The list of results fetched from the sensor.
             */
            std::list<result> getResults();

            /**
             * @brief Static creator for the sensor
             *
             * @return A pointer to the created sensor
             */
            static sensor* create(int, const std::string&);

         private:
            int readData(int* piHumidity, int* piTemp);
    };

}

#endif // H_DHT11
