var express = require("express");
var path = require("path");
var morgan = require("morgan");
var http = require("http");
var fs = require("fs");
var spawn = require("child_process").spawn;
var mongoose = require("mongoose");
var Sensor = require("./sensor_schema");

var app = express();

var cmd = "./generateJSON.o";
var generateJSON;

var sensorPath = path.resolve(__dirname, "sensors");
var sensor1JSON;
var sensor2JSON;

mongoose.connect("mongodb://localhost:27017/test");

app.use(express.static(path.resolve(__dirname, "public")));

app.use(morgan("combined"));

app.set("views", path.resolve(__dirname, "views"));
app.set("view engine", "ejs");

app.get("/", function(request, response) {
	response.render("index");
});

app.get("/sensor_1", function(request, response) {
	
	if (typeof sensor1JSON != "undefined") {
		response.status(202).json(sensor1JSON);
	} 
	else {
		response.status(404).send("No File Found");
	}
	
});

app.get("/sensor_2", function(request, response) {

	if (typeof sensor2JSON != "undefined") {
		response.status(202).json(sensor2JSON);
	} 
	else {
		response.status(404).send("No File Found");
	}
})

app.get("/get_all", function(request, response) {

	var DB_count;
	console.log("Get all");

	Sensor.find( { Sensor_ID: 1 }, function(err, count) {
		if (err) {
			throw err;
		}
		res.send(count);
	})
	response.status(404).send(DB_count);
})

app.use(function(request, response) {
	response.status(404).render("404");
})

app.listen(3000, function() {
	console.log("App started on port 3000");
	generateJSON = spawn(cmd);

	setInterval(function() {
		fs.readFile(sensorPath + "/sensor_1.json", "utf8", function (err, data) {
			if (err) {
				throw err;
			}
			sensor1JSON = JSON.parse(data);
			// if (typeof sensor1JSON != "undefined") {
			// 	console.log("Creating Entry");
			// 	Sensor.create({ Sensor_ID: sensor1JSON.Sensor_ID,
			//  				    Current: sensor1JSON.Current,
			//  				    Date: new Date(sensor1JSON.Date) });
			// }
		});
	}, 1000);

	setInterval(function() {
		fs.readFile(sensorPath + "/sensor_2.json", "utf8", function (err, data) {
			if (err) {
				throw err;
			}
			sensor2JSON = JSON.parse(data);
		});
	}, 1000);
});

