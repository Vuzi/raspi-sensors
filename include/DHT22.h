/**
 * @file DHT22.h
 * @brief Class for DHT22 sensors. A DHT22 uses the GPIO bus on the raspberry
 * @author Vuzi
 * @version 0.2
 */

#ifndef H_DHT22
#define H_DHT22

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
     * @class DHT22_sensor
     * @brief Class for DHT22 sensors
     */
    class DHT22_sensor : public gpio_sensor {

        public:
            /**
             *  @brief Constructor
             *  @param pin  Value of which pin to read data on
             *  @param name Name of the sensor
             */
            DHT22_sensor(unsigned, std::string);

            /**
             *  @brief Destructor
             */
            virtual ~DHT22_sensor();

            /**
             *  @brief Return the type of the sensor (DHT22)
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

#endif // H_DHT22

