NFC Sample Code
===============

NOTE: If you use this sample application for your own purposes, follow the
      licensing agreement specified in Software_Use_Agreement.rtf in the
      home directory of the installed Software Development Kit (SDK).

Overview
--------

This sample project demonstrates how the NFC interface can be used
to emulate an NFC Type 2 Tag with a NDEF message that initially
contains the URI http://www.onsemi.com.

1.  Function NFC_Initialize configures the NFC interface and 
	prepares responses for Layer 3 response.

2.  Function NFC_Handler checks for the end of communication status 
	(set by the NFC interrupt) and handles incoming requests.

    
Hardware Requirements
---------------------
External NFC antenna is connected to the EVB connector U5.

Importing a Project
-------------------
To import the sample code into your IDE workspace, refer to the 
*Integrated Development Environment (IDE) User's Guide* guide for more 
information.
  
Verification
------------
To verify that the application is working correctly, load the sample code on 
the device and approach it with an *NFC Reader*. The *NFC Reader* should be able to
detect the card UUID and connect to the card. The very first time this is attempted, 
the *NFC Reader* should be able to read the URL: http://www.onsemi.com. 
Subsequently, the *NFC Reader* should be able to read what it last wrote to the tag

To write to the NFC Tag, approach a phone with an *NFC Reader* to write the desired data.
The device should print the data (english language text payloads are supported) 
as a log message.
When the tag is read after, the *NFC Reader* should be able to read 
the data that was written to the tag.

**NOTE**: The *NFC Reader* is usually an NFC App - 
such as NFC Tools on Android/iOS phones. 
Alternately, a card-reader device can also be used.

Notes
-----
Sometimes the firmware cannot be successfully re-flashed, due to the
application going into Power Mode or resetting continuously (either by design 
or due to programming error). To circumvent this scenario, a software recovery
mode using RECOVERY_GPIO can be implemented with the following steps:

1.  Connect RECOVERY_GPIO to ground.
2.  Press the RESET button (this restarts the application, which pauses at the
    start of its initialization routine).
3.  Re-flash the board. After successful re-flashing, disconnect RECOVERY_GPIO from
    ground, and press the RESET button so that the application can work
    properly.

***
Copyright (c) 2022 Semiconductor Components Industries, LLC
(d/b/a onsemi). 