/**
 * @file i2c_sensor.h
 * @brief Class for TSL2561 sensors. A TSL2561 use the I2C bus on the raspberry
 * @author Vuzi
 * @version 0.1
 */

#include "i2c_sensor.h"

#ifndef I2C_BUS_FILE
    // Where to read on the i2c bus, should be setted from from node-gyp
    #define I2C_BUS_FILE "/dev/i2c-1"
#endif

/**
 * @namespace sensor
 *
 * Name space used to store every class and functions related to the sensors
 */
namespace sensor {

    i2c_sensor::i2c_sensor(uint16_t _deviceAddress, std::string _name):sensor(_name) {
        deviceAddress = _deviceAddress;
        i2c_fd = -1;
    }

    i2c_sensor::~i2c_sensor() {}

    /**
     * Open the bus to read data from. If the opening failed, the filed
     * descriptor is set to -1
     * @return          The bus descriptor, or -1 on error
     */
    void i2c_sensor::openBus() {
        if ((i2c_fd = open(I2C_BUS_FILE, O_RDWR)) < 0) {
            i2c_fd = -1;
            throw sensorException(fmt::format("Failed to open the bus : {0}", strerror(errno)), sensorErrorCode::FILE_ERROR);
        }
    }

    /**
     * Close the bus
     */
    void i2c_sensor::closeBus() {
        if(i2c_fd > 0) {
            close(i2c_fd);
            i2c_fd = -1;
        }
    }

    /**
     * Return the file descriptor of the bus, or -1
     * @return The bus file descriptor
     */
    int i2c_sensor::getBus() {
        return i2c_fd;
    }

    /**
     * Return the address of the device
     * @return The address of the device
     */
    uint16_t i2c_sensor::getDeviceAddress() {
        return deviceAddress;
    }

    /**
     * Read and return data on the specified register
     * @param  uint16_t The instruction where to read
     * @return          The value read
     */
    uint16_t i2c_sensor::readRegister(uint16_t address) {
        uint16_t response = 0x0;

        // Open the bus
        openBus();

        // Try to acquire the bus access
        if(ioctl(getBus(), I2C_SLAVE, getDeviceAddress()) < 0)
            goto error;

        // send the register address which want to read,
        // and read the response
        if(write(getBus(), &address, 1) != 1)
            goto error;
        if(read(getBus(), &response, 1) != 1)
            goto error;

        goto end;

        error:
        closeBus();
        throw sensorException(fmt::format("Read register operation failed : {0}", strerror(errno)), sensorErrorCode::I2C_ERROR);
        return 0x0;

        end:
        closeBus();
        return response;
    }
    
    /**
     * Read and return integer data on the specified register
     * @param  uint16_t The instruction where to read
     * @return          The integer read
     */
    uint16_t i2c_sensor::readRegisterInt(uint16_t address) {
        uint16_t response = 0x0;
        uint8_t buffer[2] = {0,0};

        // Open the bus
        openBus();

        // Try to acquire the bus access
        if(ioctl(getBus(), I2C_SLAVE, getDeviceAddress()) < 0)
            goto error;

        // send the register address which want to read,
        // and read the response
        if(write(getBus(), &address, 1) != 1)
            goto error;
        if(read(getBus(), buffer, 2) != 2)
            goto error;

        response = (int16_t) buffer[0]<<8 | buffer[1];

        goto end;

        error:
        closeBus();
        throw sensorException(fmt::format("Read register operation failed : {0}", strerror(errno)), sensorErrorCode::I2C_ERROR);
        return 0x0;

        end:
        closeBus();
        return response;
    }
    
    /**
     * Write a value in the specified register
     * @param  uint16_t The instruction where to write at
     * @param  uint16_t The value to write
     */
    void i2c_sensor::writeRegister(uint16_t address, uint16_t value) {
        int8_t buf[2] = {0}; // buffer for write.

        // Open the bus
        openBus();

        // Try to acquire the bus access
        if(ioctl(getBus(), I2C_SLAVE, getDeviceAddress()) < 0)
            goto error;

        buf[0] = (int8_t)address;
        buf[1] = (int8_t)value;

        // Send the register and the data
        if(write(getBus(), buf, 2) != 2)
            goto error;

        goto end;

        error:
        closeBus();
        throw sensorException(fmt::format("Write register operation failed : {0}", strerror(errno)), sensorErrorCode::I2C_ERROR);

        end:
        closeBus();
    }

}

