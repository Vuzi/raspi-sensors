/**
 * @file BMP180.h
 * @brief Class for BMP180 sensors. A BMP180 uses the I2C bus on the raspberry
 * @author Vuzi
 * @version 0.2
 */

#ifndef H_BMP180
#define H_BMP180

#include "i2c_sensor.h"
#include "sensor_result.h"

/**
 * @namespace sensor
 *
 * Name space used to store every class and functions related to the sensors
 */
namespace sensor {

    /**
     * @class BMP180_sensor
     * @brief Class for BMP180 sensors
     */
    class BMP180_sensor : public i2c_sensor {

        public:
            /**
             *  @brief Constructor
             *  @param address Address of the sensor
             *  @param name    Name of the sensor
             */
            BMP180_sensor(uint16_t, std::string);

            /**
             *  @brief Return the type of the sensor (BMP180)
             */
            const std::string getType();

            /**
             *  @brief Destructor
             */
            virtual ~BMP180_sensor();

            /**
             *  @brief Prepare a result fetching. May throw an exception if any error occures
             */
            void prepare();
            
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
            int16_t ac1;
            int16_t ac2;
            int16_t ac3;
            uint16_t ac4;
            uint16_t ac5;
            uint16_t ac6;
            int16_t b1;
            int16_t b2;
            int16_t mb;
            int16_t mc;
            int16_t md;
        
            long PressureCompensate;
        
            uint16_t readUT();
            unsigned long readUP();
            
            float convertTemperature(uint16_t ut);
            long convertPressure(unsigned long up);
    };

}

#endif // H_TSL2561
