<h2>FAQ</h2>

Hello!  I have been receiving more email requests for assistance than I can handle, so I have created this FAQ to assist you.
I apologize for not being able to reply promptly to all of your email requests, but I am doing this free of charge, in my spare time,
while also running my own business and looking after my family.  I hope that you can understand and forgive my inability to 
personally handle the large number of requests that are coming in.

Please have a look at the questions and answers below to see if they solve your problem:

---

 **Q:** How do I resolve an "HTTP POST" error, or another error?
 
 **A:** First of all, ensure that your Twilio account is paid for.  (Note: I do not receive any money from this.  Twilio is a third 
 party service for sending text messages.  It should only cost you about $1 per month.)  The "trial" Twilio accounts will only work for
 a single phone number.

 If you are indeed using a paid Twilio account and you're still having trouble sending text messages, first you should ensure that your
 Twilio credentials are correct.  Go to this URL:

```
https://www.hurl.it/
```

 And select "POST" in the "Destination" drop down list box.  Then replace the "yourapihere.com" text with the following URL:

```
api.twilio.com/2010-04-01/Accounts/{AccountSID}/SMS/Messages.json
```

 But replace the "{AccountSID}" text with whatever your AccountSID is listed as on your Twilio account page.  Next, click the
 "Add another parameter" link 3 times, and enter the following parameters:

```
PARAMETER 1 NAME: Body
PARAMETER 1 VALUE: Test

PARAMETER 2 NAME: From
PARAMETER 2 VALUE: {YOUR FULL TWILIO ACCOUNT PHONE NUMBER, STARTING WITH PLUS SIGN, FOR INTERNATIONAL DIALING}

PARAMETER 3 NAME: To
PARAMETER 3 VALUE: {THE PHONE NUMBER YOU WISH TO SEND THE TEST TEXT MESSAGE TOO, AGAIN IN INTERNATIONAL DIALING FORMAT}
```

 Then click the "I'm not a robot" button, followed by the "Launch Request" button.

 If this works correctly, you should see a green circle with the text "201 CREATED" shown on that website, and you should also
 receive a text message via the "To" phone number you specified.

 If this does NOT work, you may have entered your AccountSID incorrectly, or perhaps you are not using the correct phone number
 format for your Twilio account or your "To" phone number.  Try changing things up until you get it to work.  Once you have the correct
 data, then go back and change your Pebble Seizure Detect source code to include the right values.

 Please note: In the source code, the text "%2B" gets translated into a "+" symbol.  So if your phone number does NOT begin with +1 (as phone numbers do in Canada in the USA), you must change this line:

```
messageBody = "Body=" + messageBody + "+at+location:+http://maps.apple.com/?q=" + lastLatitude + "," + lastLongitude + "&From=%22%2B1[YOUR TWILIO PHONE NUMBER]%22&To=%22%2B1" + phoneNumber + "%22";
```
  
 To replace the "%2B1" portion (which the computer understands as "+1") such that your Twilio and other phone numbers will be correct.

--- 
 
 **Q:** How can I make the seizure detection more sensitive, or less sensitive?
 
 **A:** In the "main.c" file, look for this line:

```
#define SEIZURE_DETECTION_VALUE 125 
```
  Change the "125" number to a smaller number to make the app MORE sensitive, and change it to a larger number to make it LESS sensitive.  Then recompile and reinstall the app.  You will need to experiment to find a balance between detecting all seizures, but reducing false positives.
  
---
  
 **Q:** I have an Android device.  Isn't it possible to send text messages on the Android platform without Twilio?
 
 **A:** Yes!  That is true.  But I do not have an Android device myself so I am not able to code and test that.  However, after I created Pebble Seizure Detect I became aware of this project:

* [http://www.openseizuredetector.org.uk/](http://www.openseizuredetector.org.uk/)
  
It can use used on Android devices and sends text messages without Twilio.

---

 
If this FAQ has **not** helped you, please post an issue here and I will do my best to assist you when I am able:

* [https://github.com/PebbleSeizureDetect/PebbleSeizureDetect/issues](https://github.com/PebbleSeizureDetect/PebbleSeizureDetect/issues)

Thank you!
