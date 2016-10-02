{
    # Shared configuration
    "target_defaults": {
        "default_configuration": "Release",

        "cflags": [ "-std=c++0x", "-W", "-Wall", "-pipe" ],
        "cflags!": [ "-fno-exceptions" ],
        "cflags_cc!": [ "-fno-exceptions" ],

        "include_dirs": [ "include", "/usr/local/include" ],

        "libraries": [ "-L/usr/local/lib" ],

        "configurations": {
            "Debug" : {
                "defines": [ "DEBUG" ],
                "cflags": [ "-g" ]
            },
            "Release" : {
                "defines": [ "RELEASE" ],
                "cflags": [ "-O2" ]
            }
        }
    },

    "variables" : {
        "gpio" : "true",
        "wiringPi-lib" : "-lwiringPi", # wiringPi static lib, ignore if not used
        "i2c-bus-file" : "/dev/i2c-1"  # i2c file to use for i2c sensors
    },

    "targets": [
        {
            "target_name": "raspi-sensors",
            "sources": [
                "src/format.cc",
                "src/wrapper_sensor.cc",
                "src/plugin.cc",
                "src/sensor.cc",
                "src/sensor_result.cc",
                "src/i2c_sensor.cc",
                "src/TSL2561.cc",
                "src/BMP180.cc"
            ],
            "defines": [ "I2C_BUS_FILE=\"<(i2c-bus-file)\"" ],
            'conditions': [
                [   # raspi-sensor with GPIO activated (note that GPIO requires wiringPi)
                    'gpio == "true"', {
                        "sources": [
                            "src/gpio_sensor.cc",
                            "src/DHT22.cc",
                            "src/DHT11.cc",
                            "src/PIR.cc"
                        ],
                        "defines": [ "USE_GPIO" ],
                        "libraries": [ "<(wiringPi-lib)" ]
                    }
                ]
            ]
        }
    ]
}

