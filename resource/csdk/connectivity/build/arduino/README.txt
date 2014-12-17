Setting Wi-Fi SSID and Password for Arduino Wi-FI Shield
=====================================
PreCondition: In connectivity\src\wifi_adapter\arduino\cawifiserver.cpp	
Update the SSID and Password credentials before testing with Wi-Fi Shield
char ssid[] = "";     //  your network SSID (name)
char pass[] = "";  // your network password

Compiling Interface APIs for ARDUINOMEGA:
========================================
PREREQ: Arduino MUST be installed in the machine where we compile. Suppose ARDUINO_DIR = <PATH_TO_ARDUINO_INSTALL_DIR>/arduino-1.5.x
1) Patch the Wiznet Ethernet library with the patch available at :
        I. connectivity\lib\extlibs\arduino_linux_ethernet_cpp.patch
		II.connectivity\lib\extlibs\arduino_linux_ethernet_socket_cpp.patch
	Place both these libraries in the ARDUINO_DIR/libraries/ folder where remaining libraries are present.
2) Arduino builds are dependent on latest Time and TimedAction libraries. Download it from here:
		I. http://playground.arduino.cc/Code/Time
		II.http://playground.arduino.cc/Code/TimedAction
3) For TimedAction library. Apply the following patch.
    (Also the updated library has been pushed for reference at : connectivity\lib\extlibs\TimedAction)
	TimedAction.h Line 33 : Replace "WProgram.h" with "Arduino.h"
4) Go to "connectivity\build\arduino" directory, Open Makefile and change ARDUINO_PORT according to connected PORT
5) BUILD :  Go to "connectivity\build" directory. To build a specific TRANSPORT [ETHERNET,BLE,WIFI], use the following command :
	sudo make PLATFORM=arduinomega TRANSPORT=<ETHERNET/BLE/WIFI> ARDUINO_DIR = <PATH_TO_ARDUINO_INSTALL_DIR>/arduino-1.5.x

Note: Prior to building for BLE Transport, following additional steps are required:
1) Copy the directory "connectivity\src\bt_le_adapter\arduino\external\RBL_nRF8001" to "arduino-<x.x.x>/library" folder
2) Download BLE library zip file from the link(https://github.com/NordicSemiconductor/ble-sdk-arduino) 
   Extract zip file and copy the "BLE" folder available under "libraries" folder and place it in "arduino-<x.x.x>/library" folder
3) In caleadapter.cpp, modify BLE_ADDRESS macro to Target Arduino BLE Shield Address.
   This file is present in "connectivity/src/bt_le_adapter/arduino" folder

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