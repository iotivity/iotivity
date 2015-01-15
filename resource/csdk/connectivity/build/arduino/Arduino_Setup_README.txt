STEPS to setup Arduino on linux machine for building CA code:

1) Download the Arduino package as per your system specification from the given link : 
	http://arduino.cc/en/main/software
	
2) Unzip the downloaded package to some location say ARDUINO_DIR(This is the arduino installation directory).
	ARDUINO_DIR = <PATH_TO_ARDUINO_INSTALL_DIR>/arduino-1.5.x

3) Change permissions for executable in "ARDUINO_DIR"
		chmod -R +x ./*
		
4) ONE TIME ONLY - ADDITIONAL LIBRARIES
	Arduino builds are dependent on latest Time library. Download it from here:
		I. http://playground.arduino.cc/Code/Time
	Place this library in the "ARDUINO_DIR/libraries/Time" folder where remaining libraries are present.
	[P.S - The Time library files path should be as follows "ARDUINO_DIR/libraries/Time/Time/{***files***}"]
	
5) ONE TIME ONLY - PATCH
	Complete patch has been provided at : "connectivity\lib\arduino\arduino_libraries.patch"
		Copy this patch file to "ARDUINO_DIR/libraries/"
		
6) To apply patch give the command : 
		patch -p1 < arduino_libraries.patch
	To undo the patch give the command :
		patch -R -p1 < arduino_libraries.patch
		
7) Launch Arduino IDE (required for logs and port and board selection.)
	Goto to "ARDUINO_DIR" directory.
	Launch command := sudo ./arduino
	
8) Select your board and port under the tools option in Arduino IDE.