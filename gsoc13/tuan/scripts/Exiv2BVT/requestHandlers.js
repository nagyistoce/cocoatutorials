/**
 * Nhu Dinh Tuan (2013)
 * This file contains the functions which do the actual work when a request is received.
 * Based on the router, the associated functions will be called.
 *
 * Last update: 29/4/2013
 */

var fs = require('fs'),
	path = require('path'),
	exec = require('child_process').exec,
	spawn = require('child_process').spawn,
	helper = require("./helper.js"),
	HTMLtemplate = require("./HTMLtemplate.js");
	
// Global variable
var log_path = null,
    bvt_log = null,
    command_index = 0,
    commands = new Array();

// list all commands (name, exe, args, isPrintOutput)
    commands[0] = new Array('$ svn revert --quiet -R .', 'svn', ['revert', '--quiet', '-R', '.'], 0);
    commands[1] = new Array('$ svn update .', 'svn', ['update', '.'], 1);
    commands[2] = new Array('$ make distclean', 'make', ['distclean'], 0);
    commands[3] = new Array('$ make config', 'make', ['config'], 1);
    commands[4] = new Array('$ ./configure --disable-nls', './configure', ['--disable-nls'], 0);
    commands[5] = new Array('$ make', 'make', [], 0);
    commands[6] = new Array('$ sudo make install', 'sudo', ['make', 'install'], 1);
    commands[7] = new Array('$ make smaples','make', ['samples'], 1);
    commands[8] = new Array('$ make tests','make', ['tests'], 1);
    
    
    
// support function
function checkBVT(callback) {
	_isBVTRunning = helper.getUNIXTimestamp();
	exec("svn info | grep '^Revision'", function (error, stdout, stderr) {
		var svn_info = stdout;
		var logData = "$ svn info | grep '^Revision'\n";
		if (svn_info == "") {
			logData += error;
			_mylog.log(" Error $ svn info: " + error);
			_isBVTRunning = false;
			return callback(logData, _config.LastBuild);
		} else {
			logData += svn_info + "\n";
			var version_build = svn_info.split(": "); 
			version_build = parseInt(version_build[1]);
			// don't need to run BVT
			if (version_build == _config.LastBuild) {
				_mylog.log(" The BVT has already run. Do nothing");
				logData += "###The BVT has already run for this build. Do nothing\n";
				_isBVTRunning = false;
			} else {
				logData += "###Run BVT in 3 seconds....\n";
			}
			return callback(logData, version_build);
		}
		
	});
}

var commandCallBack = function(code) {
	command_index++;
	if (code == 0 && command_index < commands.length) { // false
		runCommands(commandCallBack);
		return;
	} 

	
	var emailBody = "";
	var bvtStartAt = new Date(_isBVTRunning * 1000);
	var totalTime = parseFloat(parseInt(helper.getUNIXTimestamp() - _isBVTRunning)/60).toFixed(2);
	if (code != 0) {
		_mylog.log(" Failed at " + commands[command_index-1][0]);
		_socketIO.sockets.in('V_' + _config.LastBuild).emit('fail', commands[command_index-1][0]);
		emailBody = "The Exiv2 bvt has been run on the ubuntu machine at " + bvtStartAt;
		emailBody += "\nSVN Version: " + _config.LastBuild;
		emailBody += "\nResult: FAIL AT " + commands[command_index-1][0];
		emailBody += "\nTotal run time: " + totalTime + " minutes";
		emailBody += "\nMore details are available at http://54.251.248.216:8888/archive/" + _config.LastBuild;
	} else {
		_mylog.log(" Completed all.");
		_socketIO.sockets.in('V_' + _config.LastBuild).emit('finish', totalTime);
		emailBody = "The Exiv2 bvt has been run on the ubuntu machine at " + bvtStartAt;
		emailBody += "\nSVN Version: " + _config.LastBuild;
		emailBody += "\nResult: RUN ALL COMMANDS SUCCESSFULLY ";
		emailBody += "\nTotal run time: " + totalTime + " minutes";
		emailBody += "\nIt may not ensure that the build passed all test cases. Please take a look at http://54.251.248.216:8888/archive/" + _config.LastBuild + " for more details";
	}
	
	fs.closeSync(bvt_log);
	_isBVTRunning = false;
	
	
	// email the result
    var mailcontent = _emailFormat + "";
	var emailSpawn = spawn('ssmtp', ['-t']);
    mailcontent = mailcontent.replace(/%subject/g, "EXIV2 UBUNTU BVT RESULT FOR VERSION " + _config.LastBuild);
    mailcontent = mailcontent.replace(/%body/g, emailBody);
    var bufferMailcontent = new Buffer(mailcontent);
    emailSpawn.stdin.write(bufferMailcontent);
    emailSpawn.stdin.end();

    emailSpawn.stderr.on('data', function (data) {
    	console.log(data);
    });
    emailSpawn.on('exit', function (code) {
	    if (code !== 0) {
			_mylog.log(' Unable to email the result with code ' + code);
	    } else {
			_mylog.log(' Email is sent to developers.');
	    }
    });
}	


function runBVT(buildVersion) {
	_mylog.log(" Run BVT ");

	// create the log file
	log_path = path.join(__dirname, "logs", buildVersion +".txt");
	bvt_log = fs.openSync(log_path, 'a');

	// set new build version
	_config.LastBuild = buildVersion;
	_config.LastBVTDate = helper.getUNIXTimestamp();
	require('./config').save(_config);

	// reset
	command_index = 0;
	
	// run commands
	runCommands(commandCallBack);
}


/**
 * COMMANDS IN ORDER
 */
function runCommands(callback) {
	var str = '---------------------------- ' + commands[command_index][0] + ' ----------------------------\n';
	var buffer = new Buffer(str);
	var socketString = str;
	_socketIO.sockets.in('V_' + _config.LastBuild).emit('stdout', socketString);
	_mylog.log(" Run " + commands[command_index][0]);
	fs.writeSync(bvt_log, buffer, 0, buffer.length);

	var commandSpawn = spawn(commands[command_index][1], commands[command_index][2]);
	
	if (commands[command_index][3] == 0) {
		_socketIO.sockets.in('V_' + _config.LastBuild).emit('stdout', '###The ouput of this command is skiped. Please check log file later\n');
	}
	commandSpawn.stdout.on('data', function (data) {
		fs.writeSync(bvt_log, data, 0, data.length);
		if (commands[command_index][3] == 1) {
			socketString = data.toString('utf8');
			_socketIO.sockets.in('V_' + _config.LastBuild).emit('stdout', socketString);
		} else {
			_socketIO.sockets.in('V_' + _config.LastBuild).emit('stdout', '. ');
		}
		
	});

	commandSpawn.stderr.on('data', function (data) {
		fs.writeSync(bvt_log, data, 0, data.length);
		if (commands[command_index][3] == 1) {
		    socketString = data.toString('utf8');
		    _socketIO.sockets.in('V_' + _config.LastBuild).emit('stdout', socketString);
		} else {
		    //
		}
		
	});

	commandSpawn.on('exit', function (code) {
		_mylog.log(" Exit with code " + code);
		str = '\n----------------------------  Exited with code ' + code + '\n\n';
		buffer = new Buffer(str);
		fs.writeSync(bvt_log, buffer, 0, buffer.length);
		_socketIO.sockets.in('V_' + _config.LastBuild).emit('stdout', str);
		return callback(code);
	});
	
	
	commandSpawn.on('error', function (code) {
		_mylog.log(" Error with code " + code);
	});
}



/**
 * Introduction page
 */
function intro(response, requestData) {
	_mylog.log(" Request handler 'intro' was called.");
	
	// this function is called after loading the HTML file
	function callback(data) {
		if (data == null) {
			response.writeHead(400, {"Content-Type": "text/plain"});
			response.write("Error: Unable to load the HTML template");
			_mylog.log(" Error: Unable to load the HTML template intro.html");
		} else {
			_mylog.log(" Load the HTML template intro.html");
			response.writeHead(200, {"Content-Type": "text/html"});
			
			var str = data + "";
			str = str.replace(/%LastBuild/g, _config.LastBuild);
			if (_config.LastBuild != 0) {
				var date = new Date(_config.LastBVTDate * 1000);
				str = str.replace(/%LastBVT/g, "at " + date);
			} else {
				str = str.replace(/%LastBVT/g, "never run");
			}
			response.write(str);
		}
		response.end();
	}
	
	// load the introduction HTML file
	HTMLtemplate.load("intro.html", callback);
}

function bvt(response, requestData) {
	
	_mylog.log(" Request handler 'intro' was called.");

	// this function is called after loading the HTML file
	function callback(data) {
		if (data == null) {
			response.writeHead(400, {"Content-Type": "text/plain"});
			response.write("Error: Unable to load the HTML template");
			_mylog.log(" Error: Unable to load the HTML template lastBVT.html");
			response.end();
		} else {
			_mylog.log(" Load the HTML template lastBVT.html");
			response.writeHead(200, {"Content-Type": "text/html"});
			var str = data + "";
			
			if (_isBVTRunning == false) {
			    _mylog.log(" Check BVT.");
			    checkBVT(function(logData, version){
				    str = str.replace(/%LastBuild/g, version);
				    str = str.replace(/%LogContent/g, logData);
				    response.write(str);
				    response.end();
				    
				    if (version !=  _config.LastBuild) setTimeout(function(){runBVT(version);}, 3000);
			    });
			} else {
			    _mylog.log(" BVT is running.");
			    var date = new Date(_isBVTRunning * 1000);
			    str = str.replace(/%LastBuild/g, _config.LastBuild);
			    str = str.replace(/%LogContent/g, "###BVT is running. It started at " + date + ". \n###The latest output is shown below:\n");
			    response.write(str);
			    response.end();
			}
		}
	}
	
	// load the lastBVT HTML file
	HTMLtemplate.load("runBVT.html", callback);
	
	
}

function archive(response, requestData) {
    _mylog.log(" Request handler 'archive' was called.");
    var buildVersion = "", logData = "";
    if (typeof requestData.parts[2] != "undefined") buildVersion = requestData.parts[2];

    try {
	    var filePath = path.join(__dirname, "logs", buildVersion +".txt");
	    logData = fs.readFileSync(filePath);
    } catch(err) {
	    _mylog.log(" No log file for the build " + buildVersion);
	    logData = "No log file for the build " + buildVersion;
    }

    response.writeHead(200, {"Content-Type": "text/plain"});
    response.write(logData);
    response.end();
}

function reset(response, requestData) {
	_mylog.log(" Request handler 'reset' was called.");
	response.writeHead(200, {"Content-Type": "text/plain"});

	if (_isBVTRunning != false) {
		_mylog.log(" Can not reset the server because the BVT is running.");
		response.write("Can not reset the server because the BVT is running.");
		response.end();
		return;
	}


	// remove all files under the directory
	/*
	rmContentDir = function(dirPath) {
	    try { var files = fs.readdirSync(dirPath); }
	    catch(e) { return; }
	    if (files.length > 0)
		for (var i = 0; i < files.length; i++) {
		    var filePath = dirPath + '/' + files[i];
		    if (fs.statSync(filePath).isFile())
			fs.unlinkSync(filePath);
		    else
			rmDir(filePath);
		}
	};*/


	// remove log files
	//logsDir = path.join(__dirname, "logs");
	//rmContentDir(logsDir);
	var BVTlogFile = path.join(__dirname, "logs", _config.LastBuild + ".txt");
	try {
	    if (fs.statSync(BVTlogFile).isFile())
	    fs.unlinkSync(BVTlogFile);
	} catch (err) {	
	    _mylog.log(" Error remove the log " + _config.LastBuild + ".txt: " + err);
	}

	// set new build version
	_config.LastBuild = 0;
	_config.LastBVTDate = 0;
	require('./config').save(_config);

	response.write("The server is reset successfully.");
	response.end();
}


exports.intro = intro;
exports.bvt = bvt;
exports.archive = archive;
exports.reset = reset;