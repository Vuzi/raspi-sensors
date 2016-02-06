# raspi-sensor install script, it will ask the user for information and compile the project

echo "Interactive builder for raspi-sensor. For more information feel free to visit https://github.com/Vuzi/raspi-sensor"

# if gpio should be installed
while true; do
    read -p "Do you want to use GPIO sensors with this plugin ? (Y/n) " gpio
    case $gpio in
        ''|[Yy]* ) gpio=true; break;;
        [Nn]* ) gpio=false; break;;
        * ) echo "Please answer yes or no";;
    esac
done

if $gpio ; then
	while true; do
	    read -p "GPIO requires the WiringPi library, where is located libwiringPi.a ? (/usr/local/lib/libwiringPi.a) " wiringPi_lib
	    case $wiringPi_lib in
	        '' ) wiringPi_lib='/usr/local/lib/libwiringPi.a'; break;;
	        * ) break;;
	    esac
	done
fi

while true; do
    read -p "i2c needs to be functioning, where is located the i2c bus to use ? (/dev/i2c-1) " i2c_file
    case $i2c_file in
        '' ) i2c_file='/usr/local/lib/libwiringPi.a'; break;;
        * ) break;;
    esac
done

while true; do
    read -p "Compile in debug or release mode ? (release|debug) " target
    case $target in
        '' ) target='release'; break;;
        release|debug ) break;;
        * ) echo "Please answer release or debug";;
    esac
done

echo "Now running node-gyp clean..."

node-gyp clean
if [ $? -ne 0 ]; then
	exit $?;
fi

echo "Now running node-gyp configure..."

node-gyp configure
if [ $? -ne 0 ]; then
	exit $?;
fi

echo "Now running node-gyp build..."

if $gpio ; then
	node-gyp build raspi-sensors-gpio --wiringPi-static-lib="$wiringPi_lib" --i2c-bus-file="$i2c_file" --$target
else
	node-gyp build raspi-sensors --i2c-bus-file="$i2c_file" --$target
fi

if [ $? -ne 0 ]; then
    exit $?;
fi

cp build/*/*.node build/raspi-sensors.node
