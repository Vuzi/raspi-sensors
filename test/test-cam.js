// Load raspi-sensors plugin
var RaspiSensors = require('../build/raspi-sensors');

console.log('raspi-sensors test v0.1');

// Create some sensors
var cam = new RaspiSensors.Sensor({
	type    : "TSL2561",
	address : 0X39
}, "light_sensor");

cam.camTest();

console.log('Test done !');
