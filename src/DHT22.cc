/**
 * @file DHT22.cc
 * @brief Class implementation for all DHT22 sensors
 * @author Vuzi
 * @version 0.2
 * Based on https://github.com/adafruit/DHT-sensor-library and on https://github.com/Seeed-Studio/Grove-RaspberryPi
 */

#include "DHT22.h"
 
namespace sensor {

    DHT22_sensor::DHT22_sensor(unsigned _pin, std::string _name):gpio_sensor(_pin, _name) {}

    DHT22_sensor::~DHT22_sensor() {}

    const std::string DHT22_sensor::getType() {
        return "DHT22";
    }

    int DHT22_sensor::readData(int* piHumidity, int* piTemp) {
        uint8_t laststate = HIGH, j = 0, i;
        int dht22_dat[5];

        dht22_dat[0] = dht22_dat[1] = dht22_dat[2] = dht22_dat[3] = dht22_dat[4] = 0;

        // Pull pin down for 18 milliseconds
        pinMode(getPin(), OUTPUT);
        digitalWrite(getPin(), LOW);
        delay(18);

        // Then pull it up for 40 microseconds
        digitalWrite(getPin(), HIGH);
        delayMicroseconds(40);

        // Prepare to read the pin
        pinMode(getPin(), INPUT);

        getTime(&laststate);
        getTime(&laststate);
        getTime(&laststate);

        // Detect change and read data
        for (i = 0; i < 40; i++) {
            int timer = getTime(&laststate);

            if(timer < 0)
                    break;

            timer = getTime(&laststate);
            if(timer < 0)
               break;

            // Ignore first 3 transitions
            // Shove each bit into the storage bytes
            dht22_dat[i/8] <<= 1;
            if (timer > 28)
                dht22_dat[i/8] |= 1;
            j++;
        }

        // Check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
        if ((dht22_dat[4] == ((dht22_dat[0] + dht22_dat[1] + dht22_dat[2] + dht22_dat[3]) & 0xFF))) {
            *piHumidity = ((int)dht22_dat[0] << 8) + (int)dht22_dat[1];
            *piTemp = ((int)dht22_dat[2] << 8 ) + dht22_dat[3];

            if ((dht22_dat[2] & 0x80) != 0)
                *piTemp *= -1;

            return 1;
        } else
            return 0;
    }

    std::list<result> DHT22_sensor::getResults() {
        std::list<result> results;

        int iHumidity = -1;
        int iTemp = -1;

        for(int i = 0; i < 10; i++) {
            if (readData(&iHumidity, &iTemp) == 1) {
                // Humidity
                resultValue humidityValue;
                humidityValue.f = (float)(iHumidity/10.0);

                result humidity(resultType::HUMIDITY, humidityValue);

                // Temperature
                resultValue tempValue;
                tempValue.f = (float)(iTemp/10.0);

                result temp(resultType::TEMPERATURE, tempValue);

                // Add to the list and return
                results.push_back(humidity);
                results.push_back(temp);

                return results;
            } else {
                // In some case, no data will be read. Waiting a few seconds usually allow
                // to read ne data from the sensor
		        usleep(1000000); // Sleep 1s
            }
        }

        // Error : no data could be read
        throw sensorException("Failed to read GPIO data", sensorErrorCode::INVALID_VALUE);
        return results;
    }

    sensor* DHT22_sensor::create(int pin, const std::string& name) {
        return (sensor*) new DHT22_sensor((unsigned) pin, name);
    }

}

