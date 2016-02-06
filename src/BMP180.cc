/**
 * @file BMP180.cc
 * @brief Class implementation for all BMP180 sensors
 * @author Vuzi
 * @version 0.2
 * Based on https://github.com/Seeed-Studio/Grove-RaspberryPi with tweaks and optimisations
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

    uint16_t BMP180_sensor::readUT() {
        writeRegister(0xF4, 0x2E);
        return readRegisterInt(0xF6);
    }

    unsigned long BMP180_sensor::readUP() {
        uint8_t msb, lsb, xlsb;

        // Write data at 0xF4
        writeRegister(0xF4, 0x34);

        // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
        msb = readRegister(0xF6);
        lsb = readRegister(0xF7);
        xlsb = readRegister(0xF8);

        return (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8);
    }

    float BMP180_sensor::convertTemperature(uint16_t ut) {
        long x1, x2;

        x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
        x2 = ((long)mc << 11)/(x1 + md);
        PressureCompensate = x1 + x2;

        float temp = ((PressureCompensate + 8)>>4);

        return temp / 10.0;
    }

    long BMP180_sensor::convertPressure(unsigned long up) {
        long x1, x2, x3, b3, b6, p;
        unsigned long b4, b7;
        b6 = PressureCompensate - 4000;
        x1 = (b2 * (b6 * b6)>>12)>>11;
        x2 = (ac2 * b6)>>11;
        x3 = x1 + x2;
        b3 = (((((long)ac1)*4 + x3)) + 2)>>2;

        // Calculate B4
        x1 = (ac3 * b6)>>13;
        x2 = (b1 * ((b6 * b6)>>12))>>16;
        x3 = ((x1 + x2) + 2)>>2;
        b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;

        b7 = ((unsigned long)(up - b3) * (50000));
        if (b7 < 0x80000000)
            p = (b7<<1)/b4;
        else
            p = (b7/b4)<<1;

        x1 = (p>>8) * (p>>8);
        x1 = (x1 * 3038)>>16;
        x2 = (-7357 * p)>>16;
        p += (x1 + x2 + 3791)>>4;

        return p;
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

