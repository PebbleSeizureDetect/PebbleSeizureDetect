//    This file is part of Pebble Seizure Detect.
//
//    Pebble Seizure Detect is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    Pebble Seizure Detect is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    A copy of the GNU General Public License can be found at http://www.gnu.org/licenses/

var locationOptions = { "timeout": 30000, "maximumAge": 60000, "enableHighAccuracy": true }; 
var lastLatitude = 0;
var lastLongitude = 0;

function locationSuccess(pos)
{
	var coordinates = pos.coords;
	console.log("Coordinates!:");
	console.log(coordinates.latitude);
	console.log(coordinates.longitude);
	
	lastLatitude = coordinates.latitude;
	lastLongitude = coordinates.longitude;
}

function locationError(err)
{
	console.warn('Location error (' + err.code + '): ' + err.message);
	Pebble.showSimpleNotificationOnPebble("SEIZURE DETECT", "GPS ERROR: (" + err.code + "): " + err.message);
}

function send_text_message(messageBody, phoneNumber)
{
	messageBody = "Body=" + messageBody + "+at+location:+http://maps.apple.com/?q=" + lastLatitude + "," + lastLongitude + "&From=%22%2B1[YOUR TWILIO PHONE NUMBER]%22&To=%22%2B1" + phoneNumber + "%22";
	
	var req = new XMLHttpRequest();
	req.open("POST", "https://api.twilio.com/2010-04-01/Accounts/[YOUR_AccountSID]/SMS/Messages.json", true);
	req.setRequestHeader("Authorization", "Basic [AccountSID:PrimaryAuthToken, Base64 encoded]");
	req.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
	req.setRequestHeader("Content-length", messageBody.length);
	req.setRequestHeader("Connection", "close");
	req.onload = function(e)
	{
		if ((req.readyState == 4) && ((req.status == 200) || (req.status == 201)))
		{
			console.log(req.responseText);
		}
		else
		{
			console.log("HTTP post error!  Body: " + messageBody + " Number: " + phoneNumber + " req: " + JSON.stringify(req));
			Pebble.showSimpleNotificationOnPebble("SEIZURE DETECT", "HTTP POST ERROR. See console log.");
		}
	};
	req.send(messageBody);
}

function send_location()
{
	/*var req = new XMLHttpRequest();
	req.open("GET", "http://yourwebserverdomainname.com/yourfolder/postdir.php?q=" + lastLatitude + "," + lastLongitude, true);
	req.onload = function(e)
	{
		if ((req.readyState == 4) && ((req.status == 200) || (req.status == 201)))
		{
			console.log(req.responseText);
		}
		else
		{
			console.log("HTTP post error!  Could not send location.  req: " + JSON.stringify(req));
			Pebble.showSimpleNotificationOnPebble("SEIZURE DETECT", "Failed to send location ping. See console log.");
		}
	};
	req.send();*/
}

// This is a hack needed to wake up GPS on the phone, otherwise you will get old position data
// But we don't want to leave watchPosition on all the time or it eats up battery
function force_location_update()
{
	var options = { enableHighAccuracy: true, maximumAge: 100, timeout: 60000 };
	var watchID = navigator.geolocation.watchPosition( locationSuccess, locationError, options );
	setTimeout( function() { navigator.geolocation.clearWatch( watchID ); }, 1000 );
}

Pebble.addEventListener("ready", function(e)
{
	force_location_update();
	navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
	send_location();
});

Pebble.addEventListener("appmessage", function(e)
{
	console.log('Phone received message: ' + e.payload["0"]);
	force_location_update();
	navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
	
	// Panic message?
	if (e.payload["0"] == 1)
	{
		send_text_message("NAME+pushed+the+panic+button", "6045551234");
		//send_text_message("NAME+pushed+the+panic+button", "7785556789");
	}
	// Seizure message?
	else if (e.payload["0"] == 2)
	{
		send_text_message("NAME+may+have+had+a+seizure", "6045551234");
		//send_text_message("NAME+may+have+had+a+seizure", "7785556789");
	}
	// Location ping?
	else if (e.payload["0"] == 3)
	{
		send_location();
	}
});
