/**
 * @file gpio_sensor.cpp
 * @brief Base class implementation for all GPIO sensors. A GPIO sensor is using the GPIO bus on the raspberry
 * @author Vuzi
 * @version 0.2
 */

#include "gpio_sensor.h"

namespace sensor {

    bool gpio_sensor::isGPIOInitialized = false;

    void gpio_sensor::prepare() {
        if(!isGPIOInitialized) {
            if(wiringPiSetup() < 0) {
                throw sensorException("Failed to init WiringPi", sensorErrorCode::GPIO_ERROR);
            } else {
                isGPIOInitialized = true;
            }
        }
    }

    gpio_sensor::gpio_sensor(unsigned _pin, std::string _name):sensor(_name) {
        pin = _pin;
    }

    gpio_sensor::~gpio_sensor() {}

    unsigned gpio_sensor::getPin() {
        return pin;
    }

    double gpio_sensor::timeDiff(struct timeval x , struct timeval y) {
        double x_ms , y_ms , diff;

        x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
        y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;

        diff = (double)y_ms - (double)x_ms;

        return diff;
    }

    int gpio_sensor::getTime(uint8_t* laststate) {
        struct timeval start, stop;
        int counter = 0;
        gettimeofday(&start, NULL);

        while (digitalRead(pin) == *laststate) {
            delayMicroseconds(1);
            counter++;
            if (counter == 255) {
                return -1;
            }
        }

        gettimeofday(&stop, NULL);
        *laststate = digitalRead(pin);

        return (int) timeDiff(start, stop);
    }

}

