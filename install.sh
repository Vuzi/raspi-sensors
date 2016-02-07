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

while true; do
    read -p "i2c needs to be functioning, where is located the i2c bus to use ? (/dev/i2c-1) " i2c_file
    case $i2c_file in
        '' ) i2c_file='/usr/local/lib/libwiringPi.a'; break;;
        * ) break;;
    esac
done

while true; do
    read -p "Compile in debug or release mode ? (Release|Debug) " target
    case $target in
        '' ) target='Release'; break;;
        Release|Debug ) break;;
        * ) echo "Please answer Release or Debug";;
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

node-gyp build raspi-sensors --i2c-bus-file="$i2c_file" --gpio="$gpio" --$target

if [ $? -ne 0 ]; then
    exit $?;
fi

cp "build/$target/raspi-sensors.node" "build/raspi-sensors.node"
