// Load raspi-sensors plugin
var RaspiSensors = require('../build/raspi-sensors');

console.log('raspi-sensors test v0.2');

// Create some sensors
var DHT22 = new RaspiSensors.Sensor({
	type : "DHT22",
	pin  : 0X7
}); 

var TSL2561 = new RaspiSensors.Sensor({
	type    : "TSL2561",
	address : 0X39
}, "light_sensor");

var BMP180 = new RaspiSensors.Sensor({
	type    : "BMP180",
	address : 0x77
}, "temp_sensor");

// Define a callback
var dataLog = function(err, data) {
	if(err) {
		console.error("An error occured!");
		console.error(err.cause);
		return;
	}

	// Only log for now
	console.log(data);
}

// Try to fetch right now a value
DHT22.fetch(dataLog);

// Fetch some value at a certain interval
TSL2561.fetchInterval(dataLog, 4);
BMP180.fetchInterval(dataLog, 5);

// After 20s of logging, stop everything
setTimeout(function() {
	console.log("Time to stop the logging of values!");

	TSL2561.fetchClear();
	BMP180.fetchClear();
}, 20000)

console.log('Control send back to the main thread');
