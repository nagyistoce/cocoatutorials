/**
 * EXIV2 BVT
 * Nhu Dinh Tuan (2013)
 * Last update: 29/4/2013
 */
var fs = require('fs'),
    path = require('path');

// global variables
GLOBAL._config = require('./config').get();
GLOBAL._mylog = require('./modules/mylog').create(_config.LogLineNumberLimit, path.join(__dirname, "logs"));
GLOBAL._server = require("./server");
GLOBAL._socketIO = ""; // start with http server
GLOBAL._isBVTRunning = false;
GLOBAL._emailFormat = fs.readFileSync(path.join(__dirname, "mailformat.txt")) //load email format

// get the port from the command line arguments
var port = process.argv[2];
if (typeof port != "undefined" && !isNaN(port)) {
	_config.Simulator.Port = port;
	require('./config').save(_config);
}

// start the server
_server.start();