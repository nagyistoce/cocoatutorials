/**
 * Nhu Dinh Tuan (2013)
 * This code is to load and save the configuration settings
 *
 * Last update: 29/4/2013
 */
 
var fs = require('fs'),
	path = require('path');

// the default file name
var CONFIG_FILE_NAME = "Exiv2BVT";
// the default extension
var CONFIG_EXTENSION = "conf";

function get() {
	var cfgFile = CONFIG_FILE_NAME + "." + CONFIG_EXTENSION;
	var filePath = path.join(__dirname, cfgFile);
	
	var configData = {};
	try {
		// read the file
		var data = fs.readFileSync(filePath, 'utf8');
		configData = JSON.parse(data);
	} catch(err) {
		// the file doesn't exist
		// create a new file with the default settings
		configData.Port = 8888;
		configData.LogLineNumberLimit = 2000;
		configData.LastBVTDate = 0;
		configData.LastBuild = 0;
		configData.Schedule = 12;
		save(configData);
	}
	return configData;
}


/**
 * Save the configuration to the file.
 **/
function save(configData) {
	var cfgFile = CONFIG_FILE_NAME + "." + CONFIG_EXTENSION;
	
	// update the file
	var str = JSON.stringify(configData);
	fs.writeFile(path.join(__dirname, cfgFile), str, function() {
		//
	});
}

exports.get = get;
exports.save = save;