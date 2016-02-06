{
    # Shared configuration
    "target_defaults": {
        "default_configuration": "Release",

        "cflags": [ "-std=c++0x", "-W", "-Wall", "-pipe" ],
        "cflags!": [ "-fno-exceptions" ],
        "cflags_cc!": [ "-fno-exceptions" ],

        "include_dirs": [ "include" ],

        "sources": [
            "src/format.cc",
            "src/wrapper_sensor.cc",
            "src/plugin.cc",
            "src/sensor.cc",
            "src/sensor_result.cc"
        ],

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
        "wiringPi-static-lib" : "/usr/local/lib/libwiringPi.a", # wiringPi static lib, ignore if not used
        "i2c-bus-file" : "/dev/i2c-1"                           # i2c file to use for i2c sensors
    },

    "targets": [
        # MeteoNode with i2c and GPIO activated (note GPIO requires wiringPi)
        {
            "target_name": "meteonode-gpio",
            "sources": [
                "src/i2c_sensor.cc",
                "src/TSL2561.cc",
                "src/BMP180.cc",
                "src/gpio_sensor.cc",
                "src/DHT22.cc",
                "src/PIR.cc",
            ],
            "defines": [ "USE_GPIO", "I2C_BUS_FILE=\"<(i2c-bus-file)\"" ],
            "libraries": [ "<(wiringPi-static-lib)", "-Wl,-rpath,<!(pwd)/build/Debug/" ]
        },
        # MeteoNode only with i2c devices
        {
            "target_name": "meteonode",
            "sources": [
                "src/i2c_sensor.cc",
                "src/TSL2561.cc",
                "src/BMP180.cc"
            ],
            "defines": [ "I2C_BUS_FILE=\"<(i2c-bus-file)\"" ],
        }
    ]
}

