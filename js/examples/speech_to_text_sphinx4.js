#!/usr/bin/env node

// Import the faceDetector JS API Service & Init the RAPPCloud Object
var RAPPCloud = require('RAPPCloud');
RAPPCloud.speechDetectionSphinx4 = require('speechDetectionSphinx4');

var services = new RAPPCloud( );

var audio = //"nao_wav_d05_a1.wav_mono16k.wav";
										 //"yes-no.wav";
										 //"nao_wav_d05_a1.wav";
										 //"nao_ogg_d05_a1.ogg"; 
		     "recording_sentence2.ogg";
		     //"email-robot.wav";
		     //"recording_yes.ogg";
		     //"recording_no.ogg";
		     //"recording_tuesday.ogg";
		     //"recording_triti.ogg";    
var grammar = [];
var words = //["tuesday", "monday"];
					   //["yes", "no"];
					   //["i", "want", "to", "go", "out"];
					   //["email", "robot"];
					   ["check", "my", "emails", "robot"];
					   //["Τρίτη", "Δευτέρα"];
					   
var sentences = //["tuesday", "monday"];
					   //["yes", "no"];
					   //["i", "want", "to", "go", "out"];
					   //["email", "robot"];
					   ["check", "my", "emails", "robot"];
					   //["Τρίτη", "Δευτέρα"];					   

/** 
 * This is the method that will handle the reply by the services.speechDetectionSphinx4
 * Do what you want with it - REMEMBER: The service is Asynchronous!!!
 */
 
function handler ( words_detected )
{
    if ( words_detected.length ) {
		console.log ( 'Found ' + words_detected.length + ' words:');
		for (var i=0; i < words_detected.length; i++) {
			console.log( words_detected[i] );
		}
	}
	else
		console.log ( 'No words found');
}

services.speechDetectionSphinx4( audio,           // audio file
								"en",            // Language
								"rapp",          // user
								//"headset",
								//"nao_wav_1_ch",
								//"nao_wav_4_ch",
								"nao_ogg",       // Audio Source Type
								grammar,         // grammar ? (empty)
								words,           // words to be considered
								sentences,       // sentences to be considered
								handler );