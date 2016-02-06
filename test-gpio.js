// Load MeteoNode plugin
var MeteoNode = require('./build/Debug/meteonode-gpio');

console.log('MeteoNode test v0.2');

// Create some sensors
var DHT22 = new MeteoNode.Sensor({
	type : "DHT22",
	pin  : 0X7
}); 

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
DHT22.fetchInterval(dataLog, 4);

// After 20s of login, stop everything
setTimeout(function() {
	console.log("Time to stop the loging of values!");

	DHT22.fetchClear();
}, 20000)

console.log('Control send back to the main thread');
