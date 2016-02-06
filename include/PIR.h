/**
 * @file PIR.h
 * @brief Class for PIR sensors. A PIR uses the GPIO bus on the raspberry,
 * and set it to HIGH if anything is detected
 * @author Vuzi
 * @version 0.1
 */

#ifndef H_PIR
#define H_PIR

#include <list>

#include "gpio_sensor.h"
#include "sensor_result.h"

/**
 * @namespace sensor
 *
 * Name space used to store every class and functions related to the sensors
 */
namespace sensor {

    /**
     * @class PIR_sensor
     * @brief Class for PIR sensors
     */
    class PIR_sensor : public gpio_sensor {

        public:
            /**
             *  @brief Constructor
             *  @param pin : Value of which pin to read data on
             */
            PIR_sensor(unsigned, std::string);

            /**
             *  @brief Destructor
             */
            virtual ~PIR_sensor();

            /**
             *  @brief Return the type of the sensor (PIR)
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
            int readData(int* piDetection);
    };

}

#endif // H_PIR

