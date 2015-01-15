Setting Wi-Fi SSID and Password for Arduino Wi-FI Shield
=====================================
PreCondition: In connectivity\src\wifi_adapter\arduino\cawifiserver.cpp	
Update the SSID and Password credentials before testing with Wi-Fi Shield
char ssid[] = "";     //  your network SSID (name)
char pass[] = "";  // your network password

BUILDING for ARDUINOMEGA:
========================================
PREREQ: Arduino MUST be installed in the machine where we compile. Refer to Arduino_Setup_README file for help.

1) Go to "connectivity\build\arduino" directory, Open Makefile and change ARDUINO_PORT according to the PORT selected in ARDUINO IDE.
2) BUILD : Go to "connectivity\build" directory. To build for a specific TRANSPORT [ETHERNET,BLE,WIFI], use the following command :
	sudo make PLATFORM=arduinomega TRANSPORT=<ETHERNET/BLE/WIFI> ARDUINO_DIR = <PATH_TO_ARDUINO_INSTALL_DIR>/arduino-1.5.x
	P.S : For PATH_TO_ARDUINO_INSTALL_DIR value refer Arduino_Setup_README.txt file.

Note: Prior to building for BLE Transport, following additional steps are required:
1) Download RedBearLab Arduino RBL_nRF8001 Library committed (2014/7/30 18:17:53) from link(https://github.com/RedBearLab/nRF8001)
2) Extract zip file and apply RBL patch(connectivity/lib/arduino/RBL_nRF8001.patch) to "RBL_nRF8001" folder by following below steps,
	i)  Go to downloaded RBL_Library Folder ("nRF8001-master/Arduino/libraries/RBL_nRF8001")
	ii) Apply dos2unix for RBL_Library Folder and RBL Patch File (RBL_nRF8001.patch), if we have downloaded Library in Non UNIX Platform.
	iii) Apply RBL Patch by using the below command
		patch -p1 < RBL Patch File Location(RBL_nRF8001.patch)
3) Copy the patched RBL_nRF8001 Library and place it in "arduino-<x.x.x>/libraries" folder
4) Download Nordic Arduino BLE library from the link (https://github.com/NordicSemiconductor/ble-sdk-arduino).
   Extract the zip file and copy the "BLE" folder available under "libraries" folder and place it in "arduino-<x.x.x>/libraries" folder

Flashing Interface APIs for ARDUINOMEGA:
========================================
Precondition: Connect Arduino Mega Board to Linux PC. Open Arduino IDE and select corresponding connected PORT
1) To Install, connect the ArduinoMega Board with corresponding Shields to Linux PC.
2) Perform sudo make install PLATFORM=arduinomega TRANSPORT=BLE
   It will flash BLE Transport Adaptor in Arduino Board. Similarly, TRANSPORT can be changed to ETHERNET, BLE, WIFI etc..

To Run Adapter code in ARDUINOMEGA Board:
========================================	
PreCondition: Flash the executable in ARDUINO MEGA BOARD. Follow above Step(Flashing Interface APIs for ARDUINOMEGA)
1) Open Serial Monitor in Arduino IDE. It will execute the flashed binary and logs will be available in serial Monitor under BaudRate(115200)