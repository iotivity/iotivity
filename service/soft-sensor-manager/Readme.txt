Before run make,                                                                   
                                                                                   
you have to modify Soft Sensor repository path                                     
                                                                                   
that Soft Sensor Manager can find assets while it starts.                          
                                                                                   
                                                                                   
If you want to build THSensorApp for Arduino, let's install follow.
$ sudo apt-get install arduino

Download Time library(Time.zip) from http://playground.arduino.cc/Code/Time 
Unzip Time.zip to /usr/share/arduino/libraries/Time.

$ sudo cp -Rdp THSensorApp/Time /usr/share/arduino/libraries/

Open the file located at "THSensorApp/build/Makefile"

Line 9, ARDUINO_PORT is the serial port path,

which has to be aligned on your system.


If you type "make" at SoftSensorManager folder.

All packages will be pushed to "Outputs" folder.

You can also found other packages from their.
