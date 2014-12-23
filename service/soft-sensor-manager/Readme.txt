/******************************************************************
*
* Copyright 2014 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/


===============================================================================
==                How to 	Build SoftSensorManager                          ==
===============================================================================

Once the source code is downloaded in your local specific folder, you may follow 
the steps to build and execute Soft Sensor Manager and its applications. 
In this context, we assume that the code was downloaded into 'oic' folder.


=======================================
1. Download source code
=======================================

Once you download the codes, please make sure that the downloaded code structure is as follows; 

oic/resource 
oic/service
oic/utilities
 
 
The path for Soft Sensor Manager is as following;

oic/service/soft-sensor-manager
 
The SoftSensorManager directory includes following sub directories;

Directories	Description
oic/service/soft-sensor-manager/build            : There are makefiles for different platform; Linux, Tizen, and Arduino.  
oic/service/soft-sensor-manager/doc              : SSM developer's guide and Getting started documents
oic/service/soft-sensor-manager/SampleApp        : There are two types of sample applications; application for UI, and application for physical sensors.
                                                   For UI application, there are SSMTesterApp in /linux, and /Tizen.
                                                   For physical sensors, 
                                                        1) Temperature and Humidity sensors, THSensorApp, in \linux and \arduino.
                                                           In the two directories, in \linux and \arduino , there are two TemperaterHumiditySensor applications, 
                                                           THSensorApp and THSensorApp1, and they are for DiscomfortSoftSensor which aggregates 
                                                           two TemperaterHumiditySensors to calculate current discomfort index in the given room.
                                                        2) Trackee_Thing for IndoorTrajectorySensor in \linux and \arduino
 
oic/service/soft-sensor-manager/SDK              : The SDK APIs for applications is located.
oic/service/soft-sensor-manager/SSMCore          : The SSM service codes 
oic/service/soft-sensor-manager/SoftSensorPlugin : The source codes for soft sensors can be located in this folder. 
                                                       Examples of soft sensors are DiscomfortIndexSensor and IndoorTrajectorySensor.


=======================================
2. Refer readme files in each build 
   directory for each module.
=======================================
There are readme files in the build directories for each module (e.g. \SDK, \ SSMCore,  \SampleApp). 
Please refer the files for specific setup.


=======================================
3. Run make
=======================================

3.1 Run make for SoftSensorManager & App in Ubuntu. 

3.1.1 Before running make for SoftSensorManager & App in Ubuntu, resource should be built in advance. 
   Please refer to ¡®Build the IoTivity project for Linux¡¯ in previous section

3.1.2 If you type "make" at ¡°soft-sensor-manager/build/linux¡±, all packages will be pushed to "/soft-sensor-manager/build/linux/release".  
   You can also found other packages in the folder

+--------------------------------------------------------------+
| ~/oic/service/soft-sensor-manager/build/linux$ make          |
+--------------------------------------------------------------+

3.2 Run make for App in Arduino

3.2.1 If you want to build for Arduino, download Arduino IDE (Arduino 1.0.6) from following url. 
   Extract ¡®arduino-1.0.6-linux32.tgz¡¯ and change folder name from ¡®arduino-1.0.6¡¯ to ¡®arduino¡¯ and then move to "/usr/share/".
   url: http://arduino.cc/en/Main/Software

+--------------------------------------------------------------------+
| $ mv arduino-1.0.6 arduino                                         |
| $ sudo cp -Rdp ./arduino /usr/share/                               |
+--------------------------------------------------------------------+
  
3.2.2 Download Time library (Time.zip, Click ¡°The download¡±) from following url. 
   Unzip Time.zip and move them to /usr/share/arduino/libraries.  
   url: http://playground.arduino.cc/Code/Time

+--------------------------------------------------------------------+
| $ sudo cp -Rdp ./Time /usr/share/arduino/libraries/                |
+--------------------------------------------------------------------+

3.2.3 Create file named ¡®local.properties¡¯ in  ¡°/oic/resource/csdk/¡± with the following definitions.

+------------------------------------------------------------------------------------+
| < local.properties >                                                               |
| ARDUINO_DIR = /usr/share/arduino                                                   |
| ARDUINO_TOOLS_DIR = $(ARDUINO_DIR)/hardware/tools/avr/bin                          |
+------------------------------------------------------------------------------------+
   If you have a problem with compiling ¡®resource¡¯ when you compile arduino application, 
   you may need to check 'local.properties' which is written in the readme file, '/oic/resource/csdk/README'.

3.2.4 Before running make for application, you need to build resource with arduino platform first. 
   Please refer to below example.

+------------------------------------------------------------------------------------------+
| ~/oic/resource/csdk$ make PLATFORM=arduinomega ARDUINOWIFI=1                             |
+------------------------------------------------------------------------------------------+
   PLATFORM :  arduinomega or arduinodue.
   ARDUINOWIFI : 0 (Ethernet), 1(Wifi)

3.2.5 Now, you are ready to build sample arduino application. 
   To build all sample  applications for arduino, just do as below.

+------------------------------------------------------------------------------------------------+
|~/oic/service/soft-sensor-manager/ build/Arduino$ make PLATFORM=arduinomega ARDUINOWIFI=1       |                           
+------------------------------------------------------------------------------------------------+ 

   If you want to build each sample application separately, go to build directory for sample application. 
   Belowing is example for THSensor App. 
   
+-------------------------------------------------------------------------------------------------------------------+
|~/oic/service/soft-sensor-manager/SampleApp/arduino/THSensorApp/build$ make PLATFORM=arduinomega ARDUINOWIFI=1     |                           
+-------------------------------------------------------------------------------------------------------------------+ 

3.2.6. To build and deploy the binary into the target hardware board, Aruino in this case, you need 'install' option. 
   Please refer to below example for THSensorApp;

+--------------------------------------------------------------------------------------------------------------------------+
|~/oic/service/soft-sensor-manager/SampleApp/arduino/THSensorApp/build$ make install PLATFORM=arduinomega ARDUINOWIFI=1    |                           
+--------------------------------------------------------------------------------------------------------------------------+ 
   Before ¡®make install¡¯, you need to check the file located at "/oic/service/soft-sensor-manager/SampleApp/arduino/THSensorApp/build/Makefile". 
   Line 26, ARDUINO_PORT is the serial port path, which has to be aligned on your system.  


=======================================
4. Execute THSensorApp and SSMTesterApp
=======================================

If you want to check how soft-sensor-manager is working, you can run simple applications - THSensorApp and SSMTesterApp

4.1 To initiate THSensorApp, please enter as following; 

+------------------------------------------------------------------------+
|~/oic/service/soft-sensor-manager/build/linux/release$ ./THSensorApp    |           
|~/oic/service/soft-sensor-manager/build/linux/release$ ./THSensorApp1   |                 
+------------------------------------------------------------------------+
 
4.2 To initiate SSMTesterApp, please enter as following;

+--------------------------------------------------------------------------+
|~/oic/service/soft-sensor-manager/build/linux/release$ ./SSMTesterApp     |
+--------------------------------------------------------------------------+

Note that the sequence of process initiations should be followed due to the process dependencies.


=======================================
5. Build with SCons
=======================================

If you want to use Scons to build soft-sensor-manager, build the source code and execute sample applications as following steps.

5.1 After downloading the source codes, type "scons" at project root directory "oic/"

+---------------------------------------+
|~/oic$ scons                           |
+---------------------------------------+

5.2 To execute Soft Sensor Manager and its applications, some shared libraries must be exported.

5.2.1 Go to "oic/out/<target_os>/<target_arch>/release", and check if there are following .so files
 liboc.so
 liboctbstack.so
 liboc_logger.so
 
5.2.2 To export the shared libraries, please enter as following;

+---------------------------------------------------------------------------------------------------------------------------+
|~/oic/out/<target_os>/<target_arch>/release$ export LD_LIBRARY_PATH=~/oic/out/<target_os>/<target_arch>/release            |
+---------------------------------------------------------------------------------------------------------------------------+

5.3 Generated outputs from building soft-sensor-manager will be in "oic/out/<target_os>/<target_arch>/release/service/soft-sensor-manager".

5.4 To initiate THSensorApp, please enter as following;

+------------------------------------------------------------------------------------------+
|~/oic/out/<target_os>/<target_arch>/release/service/soft-sensor-manager$ ./THSensorApp    |           
|~/oic/out/<target_os>/<target_arch>/release/service/soft-sensor-manager$ ./THSensorApp1   |                 
+------------------------------------------------------------------------------------------+

5.5 To initiate SSMTesterApp, please enter as following;

+--------------------------------------------------------------------------------------------+
|~/oic/out/<target_os>/<target_arch>/release/service/soft-sensor-manager$ ./SSMTesterApp     |
+--------------------------------------------------------------------------------------------+