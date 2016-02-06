/**
 * @file PIR.cc
 * @brief Class implementation for all PIR sensors
 * @author Vuzi
 * @version 0.2
 */

#include "PIR.h"

namespace sensor {

    PIR_sensor::PIR_sensor(unsigned _pin, std::string _name):gpio_sensor(_pin, _name) {}

    PIR_sensor::~PIR_sensor() {}

    const std::string PIR_sensor::getType() {
        return "PIR";
    }

    int PIR_sensor::readData(int* piDetection) {
        // Prepare to read the pin
        pinMode(getPin(), INPUT);

        // Read the pin
        uint8_t state = digitalRead(getPin());

        if(state == HIGH)
            *piDetection = 1;
        else
            *piDetection = 0;

        return 1;
    }

    std::list<result> PIR_sensor::getResults() {
        std::list<result> results;
        int iDetection = -1;

        readData(&iDetection);

        // Detection
        resultValue captureValue;
        captureValue.i = iDetection;
        result detection(resultType::DETECTION, captureValue);

        // Add to the list and return
        results.push_back(detection);

        return results;
    }

    sensor* PIR_sensor::create(int pin, const std::string& name) {
        return (sensor*) new PIR_sensor((unsigned) pin, name);
    }
}

