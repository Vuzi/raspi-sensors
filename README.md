# raspi-sensor
Nodejs C++ plugin, allowing to easily read data from raspberryPi's sensors.

## Supported sensors
For now, those sensors are supported :
- DHT22 (GPIO)
- PIR (GPIO)
- BMP180 (i2c)
- TLS2561 (i2c)

## Requirement
Nodejs, npm and node-gyp are required. This actual version uses the *0.12.6* version of Nodejs, and may or may not be compatible with newest or oldest versions.

### i2c sensors
To use **i2c** sensors, the i2c driver should be loading, usually using [raspi-config](https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c). The file (usually `/dev/i2c-1`) used to communicate with the bus will be asked during the installation.

### GPIO sensors
If you wish to use **GPIO** sensors, an existing installation of [wiringPi](http://wiringpi.com/pins/) is required. The shared library of **wiringPi** should be generated, and present in the default location, i.e. `/usr/local/lib`.

### Compiling
Finally, **node-gyp**, **g++/gcc 4.8.2** and **Make** are also needed to compile and generate the plugin. You can install **node-gyp** with npm, and **g++/gcc** and **Make** with your favorite package manager.

You should also look at the tests files (located in the test sub directory) and modify the pin and addresses according to your own configuration, using **WiringPi** own [notation](http://wiringpi.com/pins/). If any sensor isn't connected, feel free to comment its initialization.

## Building the plugin
Once everything is installed, simply run :
````
# To use i2c sensors
node-gyp configure --gpio=false
node-gyp build --release

# To use i2c and gpio sensors
node-gyp configure
node-gyp build--release 
````
You should now be able to run the sensor test 'test.js' :
````
sudo node test/test_i2c.js  # Test some i2c sensors
sudo node test/test_gpio.js # Test a GPIO sensor
sudo node test/test_all.js  # Test both GPIO and i2c sensors
````
If your configuration is correct, you'll see some data from your sensors.

You can also build the plugin using the provided shell script :
````
./install.sh
````

## Note
Note that, for now, root credentials are needed for any GPIO sensors because **wiringPi** needs it to access the GPIO bus, and because we need to access the i2c located in */dev*.

## Disclaimer
Every library used is the property of their respective owners and/or collaborators.
- NodeJs : https://raw.githubusercontent.com/nodejs/node/master/LICENSE
- C++ Format : https://raw.githubusercontent.com/cppformat/cppformat/master/LICENSE.rst
- WiringPi : http://wiringpi.com/

