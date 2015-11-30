<h2>PEBBLE SEIZURE DETECT</h2>

"Pebble Seizure Detect" is an open-source piece of software designed to aid people who experience tonic-clonic seizures.  It attempts to detect rhythmic motions of the Pebble watch in the frequency range typically seen during tonic-clonic seizures.  If a potential seizure is detected, the app will automatically send text messages to the phone numbers you specified during setup.  The text messages include a link to a Google Map showing the wearer's last known GPS location.  For a full list of features, read on, below.

My name is Ryan Clark, and I am an independent game developer.  My wife has epilepsy and an unexpected tonic-clonic seizure in 2014 prompted me to create "Pebble Seizure Detect".  She has been using the app successfully for a year now, so we've decided to release it as open source software in the hopes that it will help you as much as it has helped our family.


<h4>FEATURES</h4>

- Detection of tonic-clonic motions using a Butterworth bandpass filter
- If a potential seizure is detected, text messages are sent to any number of phone numbers that you specify.  Messages are sent via an inexpensive third party web service called Twilio.
- Text messages include the most recent GPS coordinates of the wearer (updated every minute) in the form of a Google Maps link
- There is a 15 second delay during which a detected seizure can be canceled, in case the app detected a seizure when there was none.
- There is an option to temporarily disable seizure detection, if the wearer intends to engage in a behaviour that may generate motions similar to those seen during a tonic-clonic seizure (brushing teeth, for example)
- A "panic button" feature permits immediate transmission of text messages to any number of phone numbers.  (This panic message also includes the Google Maps GPS location link.)  Some people who have epilepsy experience an "aura" prior to a tonic-clonic seizure, and they may be able to use this panic button to request help if they begin to experience an aura.
- There is an option to transmit the wearer's location to a web server every minute, so that their whereabouts can be tracked by others, even without the detection of a seizure


<h4>REQUIREMENTS</h4>

- Any Pebble watch (the original version will do, and it can generally be purchased for under $100)
- An Apple iPhone with enabled Bluetooth and location services.  ("Pebble Seizure Detect" should also work on an Android device, but it has not yet been tested on one.)
- A Twilio.com account for sending text messages (see the instructions, below)
- A webserver, if you wish to track the wearer's position even if a seizure has not been detected


<h4>POTENTIAL ISSUES</h4>

This application will definitely generate "false positives" -- that is, it will often think that it has detected a seizure when in fact no seizure has occurred.  Strong motions that are similar to the motions of a tonic-clonic seizure may trigger a false positive.  Brushing teeth will often generate a false positive, for example.  To reduce the likelihood of this occurring, we suggest wearing your Pebble on your non-dominant hand.  Also, if you know that you are about to do something that may trigger a false positive, you can make use of the "TEMP DISABLE" feature.  Or, if you like, you can alter the app's source code to reduce the sensitivity level, though be aware that doing so will also decrease the likelihood that the app will detect a real seizure, should one occur.  If a false positive does occur, the wearer will still have 15 seconds to realize that the watch is buzzing and showing a "SEIZURE DETECTED" message.  Simply press the watch's middle button 3 times to cancel the alert, thereby preventing the text messages from being sent.

If the wearer does indeed experience a tonic-clonic seizure, it is possible that the app will not detect it for a variety of reasons.  The most common would be that the wearer's hand and watch are pinned underneath their body.  This would prevent the watch from moving, which would prevent the app from detecting the seizure.  The app may also fail to detect a seizure if the seizure is extremely brief, or if the limbs do not move strongly enough, or if they move at an unusually low or high frequency.

The app will warn the user if its Bluetooth connection with the phone has been lost.  If a seizure occurs while the connection is lost, text messages cannot be sent.  However, as long as the wearer has the phone on their person, this should not occur.  But if the watch and phone are separated by a significant distance, the Bluetooth connection may indeed be lost.

The app requires an internet connection to the Twilio webservers in order to send its text messages.  If the phone's internet connection is lost or disabled, text messages cannot be sent.  The app does not currently warn the user if the internet connection has been lost.  Be sure that you have cellular data enabled (or are in wifi range) and that you have good reception at all times.

If the Twilio service is offline or fails in some way, the text messages may not be sent.

If you are underground, or in a building, or the GPS fails for some other reason, the text messages may send a link to a Google Map that contains an inaccurate location.

This app will drain your phone's battery a bit faster than when it is not running.  Obviously if the phone or watch run out of batteries, seizures will not be detected.

The option to transmit the wearer's location to a webserver even without the detection of a seizure requires that your webserver be functioning correctly.  Also, this location information is not sent in a secure fashion, so be aware that hackers could potentially intercept your position data.  You'll also want to ensure that you protect the folder in which you install the app on your webserver (lock it with an .hataccess password, for example), otherwise anyone who learns the URL could use it to track your location.

And lastly, there could be flaws in the design of the app, or bugs in the code itself.

Despite this list of potential issues, we have been using the app for over a year and it has performed very well for us.  We hope that it does the same for you.


<h4>DISCLAIMER</h4>

The short version: I am not some big corporation, and I am releasing this software for free.  Please don't sue me if it doesn't work :)  I've done my best to make it work well, but there certainly will be flaws.

The legalese version: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


<h4>INSTRUCTIONS</h4>

So how do you get it working?  If you have read the list of potential flaws and the disclaimer above, and you possess all of the listed requirements, you can get "Pebble Seizure Detect" working on your Pebble by following the steps below.  And don't worry, no knowledge of programming or code is needed to do this!  Just follow these steps carefully:

1.  Get a Twilio account:

	Twilio is a web service that allows you to send text messages using only an internet connection.  We need to use this service because Apple does not permit iOS apps to natively send text messages.  I believe that Android apps *are* permitted to send text messages, so it may be possible for you to alter the code of "Pebble Seizure Detect" to just send text messages on your Android device without using Twilio.  I do not have an Android device so I unfortunately cannot do this for you.

	To get a Twilio account, go here: https://www.twilio.com/try-twilio

	Note that this account will cost money, after the free trial period.  It currently costs me about $1/month, so it's not really too bad!

2.  Get a CloudPebble account:

	CloudPebble is the tool that is used to compile and install apps on your Pebble and your phone.  Sign up for an account here: https://auth.getpebble.com/users/sign_up

3.  Set up your project on CloudPebble:

	- Log in to CloudPebble with your new account and then go to this URL: https://cloudpebble.net/ide/
	- Click the "Create" button to create a new project.
	- Name the project "Seizure Detect", leave the project type as "Pebble C SDK" but change the SDK Version to "SDK 2".  Leave the template as "Empty Project" and click "Create"
	- Next to "Source Files" on the left, click "Add New"
	- Add a new "C file" and call it "main.c".  "Target" should be "App/Watchface" and leave the "Create Header" box unchecked.  Then, click "Create"
	- Delete any text contents of the "main.c" file
	- Copy the source code from this URL and paste it into "main.c": https://raw.githubusercontent.com/PebbleSeizureDetect/PebbleSeizureDetect/master/main.c
	- Click the "Save" icon on the right hand side
	- Next to "Source Files" on the left, again click "Add New"
	- Change the "File Type" to "JavaScript file" and name it "phone.js"
	- Copy the source code from this URL and paste it into "phone.js": https://raw.githubusercontent.com/PebbleSeizureDetect/PebbleSeizureDetect/master/phone.js
	- Click the "Save" icon on the right hand side
	- Keep this project window open as we will need it again later

4. Edit the source code to make it work properly for your Twilio account's new phone number:

	- To find the new phone number that comes with your Twilio account, go to this page: https://www.twilio.com/user/account/phone-numbers/incoming
	- Take that phone number and remove everything but the numbers.  Also, do not include the "1".  You should end up with something like this if you are in the USA or Canada: 6045551234
	- In the "phone.js" source code file, find the text "[YOUR TWILIO PHONE NUMBER]" and replace it with your Twilio number.  For example, find this line:

    <pre>messageBody = "Body=" + messageBody + "+at+location:+http://maps.apple.com/?q=" + lastLatitude + "," + lastLongitude + "&From=%22%2B1[YOUR TWILIO PHONE NUMBER]%22&To=%22%2B1" + phoneNumber + "%22";</pre>
		
	And modify it with your phone number to become something like:
  
		<pre>messageBody = "Body=" + messageBody + "+at+location:+http://maps.apple.com/?q=" + lastLatitude + "," + lastLongitude + "&From=%22%2B16045551234%22&To=%22%2B1" + phoneNumber + "%22";</pre>

	- NOTE: If you are not in the USA or Canada, ignore the previous two steps and follow these instead:
	  - Take the phone number from your Twilio account page and remove everything but the numbers, but KEEP your country code.  For example +44 7700 900031 would become 447700900031
	  - In the "phone.js" source code file, find the text "1[YOUR TWILIO PHONE NUMBER]" (note that we are including the "1" here!) and replace it with your Twilio number.  For example, find this line:

			<pre>messageBody = "Body=" + messageBody + "+at+location:+http://maps.apple.com/?q=" + lastLatitude + "," + lastLongitude + "&From=%22%2B1[YOUR TWILIO PHONE NUMBER]%22&To=%22%2B1" + phoneNumber + "%22";</pre>
		
		And modify it with your phone number to become something like:

			<pre>messageBody = "Body=" + messageBody + "+at+location:+http://maps.apple.com/?q=" + lastLatitude + "," + lastLongitude + "&From=%22%2B447700900031%22&To=%22%2B1" + phoneNumber + "%22";</pre>

5. Edit the source code to make it work properly with your Twilio account's ID and auth token:
	
	- Log in to your Twilio account and go to the Account details page to find your AccountSID and AuthToken's Primary Token.  (To view the "Primary Token" you may have to click on the little lock icon below it.)
	- In the "phone.js" source code file, find the text "[YOUR_AccountSID]" and replace it with whatever your AccountSID is
	- Next, go to this URL: https://www.base64encode.org/ and in the top box, enter your AccountSID, then a colon, then your AuthToken's Primary Token.  It should look something like this:
	
		AC008a1d58ca2491c8b1da25ef5d33337a:bafee59f98f1bf63d3fafed24df17ba5
	
	- Click "Encode" to encode it as UTF-8.  The result in the bottom box should look something like this:
	
		QUMwMDhhMWQ1OGNhMjQ5MWM4YjFkYTI1ZWY1ZDMzMzM3YTpiYWZlZTU5Zjk4ZjFiZjYzZDNmYWZlZDI0ZGYxN2JhNQ==
	
	- Copy that value, and in the "phone.js" source code file, find the text "[AccountSID:PrimaryAuthToken, Base64 encoded]", and paste it to replace.
	
6. Edit the source code to include the wearer's name, and relevant phone numbers.
	
	- In the "phone.js" source code file, find the text "NAME" (in all capital letters).  Replace "NAME" with the first name (no spaces) of the wearer.  "NAME" appears multiple times in the source code, so be sure to replace all instances of it, but only replace "NAME" if it is in all capital letters.  Ignore lower case "name".
	- In the "phone.js" source code file, find the two instances of the text "6045551234" and replace both instances with the phone number you would like to have notified if a seizure is detected or if the panic button is pressed.
	- If you would like more than one number to be notified, copy the entire line of code containing the phone number and paste it again in the next line, and edit the phone number in the newly pasted line.  Be sure to do this for both the "Panic message" section and the "Seizure message" section.  If you do decide to add two phone numbers, that section of the source code should look something like this:
	
	 <pre>// Panic message?
	if (e.payload["0"] == 1)
	{
		send_text_message("Ryan+pushed+the+panic+button", "6045551234");
		send_text_message("Ryan+pushed+the+panic+button", "7785556789");
	}
	// Seizure message?
	else if (e.payload["0"] == 2)
	{
		send_text_message("Ryan+may+have+had+a+seizure", "6045551234");
		send_text_message("Ryan+may+have+had+a+seizure", "7785556789");
	}</pre>
	
	The above source code would send two messages on a detected seizure or panic.  One message would be sent to the phone number 6045551234 and one message would be sent to the phone number 7785556789.
	
7. Install the Pebble app on your iPhone:

	- On your iPhone, visit this page and install the Pebble app: https://itunes.apple.com/ca/app/pebble-smartwatch/id592012721?mt=8
	- Follow the instructions here to pair your Pebble watch with your iPhone: https://help.getpebble.com/customer/portal/articles/1427478-getting-started-2-0
    - Follow the instructions here to enable the Pebble developer connection: https://developer.getpebble.com/guides/publishing-tools/developer-connection/

8. Compile, install, and run "Pebble Seizure Detect":

	- Go back to your CloudPebble project and click the "COMPILATION" link on the left-hand side
	- Click the "Run Build" button, and hopefully you'll eventually see the word "SUCCEEDED" in the status column.
	- Click the "Phone" tab at the top of the screen
	- With your phone on and the Pebble app running (and the developer connection enabled, as described in step 7 above), click the "INSTALL AND RUN" button on CloudPebble.  The app should be installed on your phone and it should eventually start up on your watch!

9. Test it!:

	- To ensure that the app is working correctly, use the up and down buttons on your Pebble to select the "PANIC" menu item and then push the middle button.  Then push the middle button 3 times in quick succession to send a panic message.  Check to make sure that the phone numbers you have added to the source code did indeed receive a text message!  And ensure that the message includes a clickable link to a Google Map that accurately represents your location.
	- You may also wish to simulate a seizure by making a strong "toothbrushing" type of motion with the watch, for 30 to 60 seconds.  It should indicate that a seizure has been detected.  You should practice disabling a detected seizure (so that you know how to disable it during a false positive) and you should also allow it to actually send the seizure detection message and ensure that the relevant phone numbers do indeed receive a text message with your Google Maps location link.
	

<h4>OPTIONAL INSTRUCTIONS FOR PERSISTENT LOCATION TRACKING</h4>

If you would like to enable the ability to transmit the wearer's location to a webserver even without the detection of a seizure, follow the instructions below.  Please note that you will need to be relatively web savvy and already have a running website in order for this to work:

<ul>
	<li>Create a new text file called "postdir.php" and paste the contents of this URL into it: https://raw.githubusercontent.com/PebbleSeizureDetect/PebbleSeizureDetect/master/postdir.php</li>
	<li>You may wish to edit the default timezone and the string that says "Pacific Time"</li>
	<li>Upload this "postdir.php" file somewhere on your webserver</li>
	<li>Open your CloudPebble project again, and in the "phone.js" source code file:
		<ul><li>Find the text "/*var" and delete the "/*" portion</li>
		<li>Find the text "req.send();*/" and delete the "*/" portion</li>
		<li>Find the text "http://yourwebserverdomainname.com/yourfolder/postdir.php" and replace it with the actual path to the "postdir.php" file on your webserver</li></li></ul>
	<li>Click the "COMPILATION" link on the left-hand side</li>
	<li>Click the "Run Build" button, and hopefully you'll eventually see the word "SUCCEEDED" in the status column.</li>
	<li>Click the "Phone" tab at the top of the screen</li>
	<li>With your phone on and the Pebble app running (and the developer connection enabled, as described during initial setup), click the "INSTALL AND RUN" button on CloudPebble.  The app should be installed on your phone and it should eventually start up on your watch.</li>
	<li>To test if the location is now being transmitted properly, navigate to the following URL: "http://yourwebserverdomainname.com/yourfolder/location/" but replace the "http://yourwebserverdomainname.com/yourfolder/" portion with the path to the directory in which  you uploaded the "postdir.php" file.  Browsing to the "location" subfolder will bring up a page that links you to the Google Maps location of the most recent GPS update.  Click the link to ensure that it is accurate.  If the Google Map is showing you the GPS coordinates 0,0 just wait one minute to give the app time to get a proper GPS reading from the phone and submit it.  Then visit the URL again and the correct coordinates should be shown.</li>
</ul>

<br /><h4>SUMMARY</h4>

That's about it!  If you have any troubles, or if you have feedback, you can contact me here: pebbleseizuredetect@braceyourselfgames.com

Alternatively, you can message me on Twitter: https://twitter.com/braceyourselfok

I hope that "Pebble Seizure Detect" is helpful for you.  If you know of other people who may benefit from it, please spread the word.  Thank you!
