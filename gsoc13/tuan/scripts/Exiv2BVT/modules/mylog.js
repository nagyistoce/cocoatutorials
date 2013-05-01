/**
 * MYLOG
 * This is a small module to write the log to file, auto clean up when the number of lines in log file exceeds the limit
 * Example:
 * 		var mylog = require('./mylog').create(); // initialize
 *		mylog.log("My first log"); 
 *		mylog.log("My first log with the date", true); // include the current time in UTC format
 *
 *
 * Nhu Dinh Tuan (2013)
 * Last update: 29/4/2013
 */

var fs = require('fs'),
	path = require('path');


// the default directory contains the log files
var LOG_DIR = "logs";
// the default name of the current log file.
var CURRENT_LOG_FILE_NAME = "BVTlog";
// the name of the old log file.
var OLD_LOG_FILE_NAME = "old";
// the default extension of the log file
var LOG_EXTENSION = "log";
// the default maximum number of lines in log file
var MAX_LOG_LINE_NO = 10000


// constructor
var Mylog = function(lineLimit, directory) {
	this.directory = LOG_DIR;
	if (typeof directory != "undefined" && directory != null) {
		this.directory = directory;
	}
	this.fileName = CURRENT_LOG_FILE_NAME;
	if (typeof fileName != "undefined" && fileName != null && fileName != "") {
		this.fileName = fileName;
	}
	this.fileExtension = LOG_EXTENSION;
	if (typeof fileExtension != "undefined" && fileExtension != null) {
		this.fileExtension = fileExtension;
	}
	this.lineLimit = MAX_LOG_LINE_NO;
	if (typeof lineLimit != "undefined" && lineLimit != null && lineLimit != "") {
		this.lineLimit = lineLimit;
	}
	
	this.lineNo = 0;
	this.buffer = {};
	
	
	// create the directory if it's necessary
	var logFileStatus = null;
	try {
		logFileStatus = fs.statSync(path.join(this.directory, this.fileName + "." + this.fileExtension));
	} catch (err) {
		// no log file
		if (this.directory != "") {
			//try to create the directory
			try {
				fs.mkdirSync(this.directory);
			} catch (err) {
				// the directory already exists
			}
		}
	}
	
	
	// initialize the writing stream
	try {
		this.stream = fs.createWriteStream(path.join(this.directory, this.fileName + "." + this.fileExtension), {'flags': 'a+', 'encoding': 'utf8'});
	} catch (err) {
		console.log("[mylog.js - 72] ERROR: " + err, true);
		return null;
	}
	
	
	// if the log file exists, need to count the number of lines to initialize the 'lineNo'
	if (logFileStatus != null) {
		var count = 0;
		
		var readStream = fs.createReadStream(path.join(this.directory, this.fileName + "." + this.fileExtension));
		
		var mylog = this;		
		readStream.on('data', function(chunk) {
			for (i=0; i < chunk.length; ++i)
				if (chunk[i] == 10) count++;
			})
		readStream.on('end', function() {
			mylog.lineNo = count;
		});
	}
}


/**
 * Write the 'str' to log file as well as show the 'str' on the console.
 * If needPrintTime = true, the current time (UTC) will be included in the 'str'  
 */
Mylog.prototype.log = function (str, needPrintTime) {
	// while cleaning up the old file, write to buffer
	if (this.stream  == null) {
		return;
	}

	
	if (needPrintTime == true) {
		var now = new Date();
		var dateAndTime = now.toUTCString();
		str = "\n" + dateAndTime + " : " + str;
	}
	
	this.stream.write(str + " \n", 'utf8');
	this.lineNo++;
	console.log(str);
	
	if (this.lineNo > this.lineLimit) {
		this.cleanUp();
	}
}


/**
 * Delete the old log file
 * Change the current log file to become the old log file
 */
Mylog.prototype.cleanUp = function () {	
	if (this.stream == null) {
		return;
	}
	this.stream.end();
	this.stream = null;
	
	
	// delete old log
	try {
		fs.unlinkSync(path.join(this.directory, OLD_LOG_FILE_NAME + "." + this.fileExtension));
	} catch (err) {
		// no old log
	}
	
	try {	
		// change the current log file to the old one
		fs.renameSync(path.join(this.directory, this.fileName + "." + this.fileExtension), path.join(this.directory, OLD_LOG_FILE_NAME + "." + this.fileExtension));
	} catch (err) {
		
	}
	this.lineNo = 0;
	this.stream = fs.createWriteStream(path.join(this.directory, this.fileName + "." + this.fileExtension), {'flags': 'a+', 'encoding': 'utf8'});
}



exports.create = function (lineLimit, directory) {
	return new Mylog(lineLimit, directory);
};