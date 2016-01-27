#!/usr/bin/env node

// Import the faceDetector JS API Service & Init the RAPPCloud Object
var RAPPCloud = require('../includes/cloud/RAPPCloud');
RAPPCloud.textToSpeech = require('../includes/cloud/textToSpeech');

var services = new RAPPCloud( );

var text = "Hello human";

/** 
 * This is the method that will handle the reply by the services.speechDetectionSphinx4
 * Do what you want with it - REMEMBER: The service is Asynchronous!!!
 */
 
function handler ( audio_data, encoding, basename )
{
	var fs = require('fs');
	var path = "audio/" + basename;
	fs.writeFile(path, audio_data, encoding, function(err) {
	    if(err) {
	        return console.log(err);
	    }
	    console.log("The file was saved under audio/" + basename );
	}); 
}

services.textToSpeech( text, "en", handler );