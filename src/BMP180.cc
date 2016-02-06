/**
 * @file BMP180.cc
 * @brief Class implementation for all BMP180 sensors
 * @author Vuzi
 * @version 0.2
 * Based on https://github.com/Seeed-Studio/Grove-RaspberryPi and
 * https://github.com/BoschSensortec/BMP180_driver/blob/master/bmp180.c
 * with tweaks and optimisations
 */

#include "BMP180.h"

namespace sensor {

    BMP180_sensor::BMP180_sensor(uint16_t deviceAddress, std::string _name):i2c_sensor(deviceAddress, _name) {}

    BMP180_sensor::~BMP180_sensor() {};

    const std::string BMP180_sensor::getType() {
        return "BMP180";
    }

    void BMP180_sensor::prepare() {
        ac1 = readRegisterInt(0xAA);
        ac2 = readRegisterInt(0xAC);
        ac3 = readRegisterInt(0xAE);
        ac4 = readRegisterInt(0xB0);
        ac5 = readRegisterInt(0xB2);
        ac6 = readRegisterInt(0xB4);
        b1 = readRegisterInt(0xB6);
        b2 = readRegisterInt(0xB8);
        mb = readRegisterInt(0xBA);
        mc = readRegisterInt(0xBC);
        md = readRegisterInt(0xBE);
    }

    /**
     * @brief Read the uncompensated temperature
     * @return The uncompensated temperature
     */
    int32_t BMP180_sensor::readUT() {
        // Write 0x2E at register 0xF4
        writeRegister(0xF4, 0x2E);

        // Wait for 4.5ms / 4500 microsecondes
        microsecondSleep(4500);

        // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
        uint16_t msb = readRegister(0xF6);
        uint16_t lsb = readRegister(0xF7);

        return (msb << 8) + lsb;
    }

    int32_t BMP180_sensor::readUP() {
        // Write 0x34 at register 0xF4
        writeRegister(0xF4, 0x34 + (oss << 6));

        // Sleep for 2ms + 3ms << oss
        microsecondSleep((2 + (3 << oss)) * 1000);

        // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
        uint32_t msb = readRegister(0xF6);
        uint32_t lsb = readRegister(0xF7);
        uint32_t xlsb = readRegister(0xF8);

        return ((msb << 16) + (lsb << 8) + xlsb) >> (8 - oss);
    }

    float BMP180_sensor::convertTemperature(uint16_t ut) {
        int32_t x1, x2;

        x1 = ((ut - ac6) * ac5) >> 15;
        x2 = (mc << 11) / (x1 + md);

        b5 = x1 + x2;

        return ((b5 + 8) >> 4) / 10.0;
    }

    long BMP180_sensor::convertPressure(unsigned long up) {
        int32_t x1, x2, x3, b3, b6, p;
        uint32_t b4, b7;

        b6 = b5 - 4000;
        x1 = (b2 * (b6 * b6) >> 12) >> 11;
        x2 = (ac2 * b6) >> 11;
        x3 = x1 + x2;
        b3 = (((ac1 * 4 + x3) << oss) + 2) >> 2;

        x1 = (ac3 * b6) >> 13;
        x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
        x3 = ((x1 + x2) + 2) >> 2;
        b4 = (ac4 * (uint32_t)(x3 + 32768)) >> 15;

        b7 = ((uint32_t)up - b3) * (50000 >> oss);

        if(b7 < 0x80000000)
            p = (b7 << 1) / b4;
        else
            p = (b7/b4) << 1;

        x1 = (p >> 8) * (p >> 8);
        x1 = (x1 * 3038) >> 16;
        x2 = (-7357 * p) >> 16;

        return p + ((x1 + x2 + 3791) >> 4);
    }

    std::list<result> BMP180_sensor::getResults() {
        std::list<result> results;

        // Read the result from the sensor
        float temp = convertTemperature(readUT());
        float pressure = convertPressure(readUP());

        // Init the data
        resultValue tempValue;
        tempValue.f = temp;
        result tempResult(resultType::TEMPERATURE, tempValue);

        resultValue pressureValue;
        pressureValue.f = pressure;
        result pressureResult(resultType::PRESSURE, pressureValue);

        // Return
        results.push_back(tempResult);
        results.push_back(pressureResult);

        return results;
    }

    sensor* BMP180_sensor::create(int pin, const std::string& name) {
        return (sensor*) new BMP180_sensor((uint16_t) pin, name);
    }
}

