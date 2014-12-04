Setting Wi-Fi SSID and Password for Arduino Wi-FI Shield
=====================================
PreCondition: In connectivity\src\wifi_adapter\arduino\cawifiadapterutils.cpp	
Update the SSID and Password credentials before testing with Wi-Fi Shield
char ssid[] = "";     //  your network SSID (name)
char pass[] = "";  // your network password

Compiling Interface APIs for ARDUINOMEGA:
========================================
PREREQ: Arduino MUST be installed in the machine where we compile
1) Open the local.properties file and change the path for ARDUINO_DIR to your own arduino installation directory.
2) Similarly change the path for ARDUINO_TOOLS_DIR to your arduino tools directory.
NOTE: We have changed local.properties so that, this could be a generic location (/usr/share/arduino).
So, this assumes arduino files to be installed in this location. If there's a change, we need to change accordingly in local.properties.

3) Go to "connectivity\build\arduino" directory, Open Makefile and change ARDUINO_PORT according to connected PORT
4) Go to "connectivity\build" directory. To build a specific TRANSPORT [ETHERNET,BLE,WIFI], use the following command :
	sudo make PLATFORM=arduinomega TRANSPORT=<ETHERNET/BLE/WIFI>    

Note: Prior to building for BLE Transport, following additional steps are required:
-	Copy the directory "connectivity\src\bt_le_adapter\arduino\external\RBL_nRF8001" to "arduino-<x.x.x>/library" folder
-	Download BLE library from internet and place it in "arduino-<x.x.x>/library" folder
	
	
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