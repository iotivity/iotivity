******************************************************************

 Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

===============================================================================
==                    NOTICE - Transition to SCONS                           ==
===============================================================================

The IoTivity build system is transitioning to SCONS. Although the 
makefiles are still available (until v1.0) and some developers are 
still using them, they are currently no longer supported. To learn more 
about building using SCONS see Readme.scons.txt in the repository root 
directory. The build steps used in continuous integration can be found
in auto_build.sh which is also in the the repository root directory.

===============================================================================
==                      Rich Stack & Lite Stack                              ==
===============================================================================

The OIC-RESOURCE repository (referred as "<oic-resource>") contains two SDKs
with underlying code (i.e. "stack"). The two SDKs are referred to as "Rich SDK"
(or "C++ SDK") and "Lite SDK" (or "C SDK"). The associated stacks are referred
to as "Rich Stack" (or "C++ Stack") and "Lite Stack" (or "C Stack"),
respectively. The Rich Stack-SDK pair requires that the Lite Stack-SDK pair is
built under it.

For a list of artifact locations for all possible build processes in the
OIC-RESOURCE repository, please refer to:
<oic-resource>/artifact_output_locations.txt.

===============================================================================

The Rich Stack is intended ONLY for Linux Ubuntu 12.04 operating system.
To build Rich Stack, please follow these requirements:
- GCC compiler version is 4.6
- Pre-install the "gnu-libc" libraries package.
- You have pre-built the Lite Stack for Linux Ubuntu 12.04.
- You have installed and pre-built Boost 1.54.
- To compile Rich Unit Tests, you must have Google Test 1.7.0 and must also
  create/append "local.properties" file at <oic-resource>/csdk/ with the
  following specified:

  GTEST_DIR=<gtest-1.7.0>

The Lite Stack is intended ONLY for Linux Ubuntu 12.04, Arduino ATMega 2560 with
Arduino Framework 1.0.5, and Arduino Due with Arduino Framework 1.5.7.
Tip: Use Cutecom in Ubuntu 12.04 to view logs from Arduino ATMega 2560 and
Arduino Due.

To build Lite Stack, please follow these requirements for Linux Ubuntu 12.04:
- GCC compiler version is 4.6
- Pre-install the "gnu-libc" libraries package.

To build Lite Stack, please follow these requirements for Arduino ATMega 2560:
- Arduino Framework 1.0.5 is installed. (Newer versions of the Arduino
  Framework for Arduino ATMega 2560 have not been tested.)
- AVR-GCC compiler version is 4.5.3 (Distributed with Arduino 1.0.5, no further
  action necessary if framework was obtained from
  http://arduino.cc/en/Main/OldSoftwareReleases. Note: Frameworks obtained from
  apt-get have had different compiler versions.)
- Unzip the Time Library from here to your Arduino Directory Structure:
  http://playground.arduino.cc/code/time
- Apply patch "arduino-1.0.5_linux.patch" at
  <oic-resource>/tools/arduino/patches to your Arduino Directory Structure at
  location <Arduino 1.0.5>.
- Create/Append "local.properties" file at location
  <oic-resource>/resource/csdk/ with the following specified:

  ARDUINO_DIR := <arduino-1.0.5>
  ARDUINO_TOOLS_DIR := $(ARDUINO_DIR)/hardware/tools/avr/bin

To build Lite Stack, please follow these requirements for Arduino Due:
- Arduino Framework 1.5.7 is installed. (Newer versions of the Arduino
  Framework for Arduino Due have not been tested.)
- AVR-GCC compiler version is 4.8.3 (Distributed with Arduino 1.5.7, no further
  action necessary if framework was obtained from
  http://arduino.cc/en/Main/OldSoftwareReleases. Note: Frameworks obtained from
  apt-get have had different compiler versions.)
- Unzip the Time Library from here to your Arduino Directory Structure:
  http://www.pjrc.com/teensy/td_libs_Time.html
- Apply patch "arduino-1.5.7_linux.patch" at
  <oic-resource>/tools/arduino/patches to your Arduino Directory Structure at
  location <Arduino 1.5.7>.
- Create/Append "local.properties" file at location
  <oic-resource>/resource/csdk/ with the following specified:

  ARDUINO_DIR := <arduino-1.5.7>
  ARDUINO_TOOLS_DIR := $(ARDUINO_DIR)/hardware/tools/avr/bin

Note: If you are building Unit Tests for Linux, building for Arduino Mega, and
building for Arduino Due you may find it useful to set the "local.properties"
file to look like this:

GTEST_DIR=<gtest-1.7.0>

ifeq ($(PLATFORM), arduinomega)
	#Location of arduino sdk for Arduino Mega
	ARDUINO_DIR := <arduino-1.0.5>
	ARDUINO_TOOLS_DIR := $(ARDUINO_DIR)/hardware/tools/avr/bin
else
	#Location of arduino sdk for Arduino Due
	ARDUINO_DIR := <arduino-1.5.7>
	ARDUINO_TOOLS_DIR := $(ARDUINO_DIR)/hardware/tools/gcc-arm-none-eabi-4.8.3-2014q1/bin
endif

===============================================================================

Instructions for Common Build Processes:

NOTE: 'GNU Make' is required to utilize the build script at location:
<oic-resource>/buildScript.mk.

=======================================
== All Targets - Linux, ATMega & Due ==
=======================================

- All Modules, including OCICUC, in Release & Debug (Lite Stack, Lite Unit
  Tests, Lite Examples, Rich Stack, Rich Unit Tests, Rich Examples, Rich
  OCICUC) for linux, Arduino ATMega 2560 with Ethernet Shield, Arduino ATMega
  ATMega 2560 with WiFi Shield, Arduino Due with Ethernet Shield and Arduino
  Due with WiFi Shield:
  (This will be what is used on the Build Server(s) to limit regression.)

	make -f buildScript.mk all_dev

=======================================
== All Targets - Linux & ATMega 2560 ==
=======================================

- All Modules in Release (Lite Stack, Lite Unit Tests, Lite Examples, Rich
  Stack, Rich Unit Tests, Rich Examples) for Linux & Arduino ATMega 2560 with
  Ethernet Shield:

	make -f buildScript.mk all

- All Modules in Debug (Lite Stack, Lite Unit Tests, Lite Examples, Rich Stack,
  Rich Examples) for Linux & Arduino ATMega 2560 with Ethernet Shield:

	make -f buildScript.mk all_debug

=======================================
==     All Targets - Linux & Due     ==
=======================================

- All Modules in Release (Lite Stack, Lite Unit Tests, Lite Examples, Rich
  Stack, Rich Unit Tests, Rich Examples) for Linux & Arduino Due with Ethernet
  Shield:

	make -f buildScript.mk complement

- All Modules in Debug (Lite Stack, Lite Unit Tests, Lite Examples, Rich Stack,
  Rich Unit Tests, Rich Examples) for Linux & Arduino Due with Ethernet Shield:

	make -f buildScript.mk complement_debug

=======================================
==     All Targets - Linux Only      ==
=======================================

- All Modules, without OCICUC, in Release (Lite Stack, Lite Unit Tests, Lite
  Examples, Rich Stack, Rich Unit Tests, Rich Examples) for linux:

	make -f buildScript.mk linux

- All Modules, without OCICUC, in Debug (Lite Stack, Lite Unit Tests, Lite
  Examples, Rich Stack, Rich Unit Tests, Rich Examples) for linux:

	make -f buildScript.mk linux_debug

- All Rich Stack Modules in Release, without OCICUC, (Lite Stack, Rich Stack,
  Rich Unit Tests, Rich Examples) for Linux:

	make -f buildScript.mk linux_ub

- All Rich Stack Modules in Debug, without OCICUC, (Lite Stack, Rich Stack,
  Rich Unit Tests, Rich Examples) for Linux:

	make -f buildScript.mk linux_ub_debug

- All Rich Stack Modules, including OCICUC, in Release & Debug (Lite Stack,
  Rich Stack, Rich Unit Tests, Rich Examples, Rich OCICUC) for linux:

	make -f buildScript.mk linux_ub_dev

- All Lite Stack Modules in Release (Lite Stack, Lite Unit Tests, Lite
  Examples) for Linux:

	make -f buildScript.mk linux_tb

- All Lite Stack Modules in Debug (Lite Stack, Lite Unit Tests, Lite Examples)
  for Linux:

	make -f buildScript.mk linux_tb_debug

=======================================
==      All Targets - Mega Only      ==
=======================================

- All Lite Stack Modules in Release (Lite Stack, Lite Unit Tests, Lite
  Examples) for Arduino ATMega 2560 with Ethernet Shield:

	make -f buildScript.mk arduinomega

- All Lite Stack Modules in Debug (Lite Stack, Lite Unit Tests, Lite Examples)
  for Arduino ATMega 2560 with Ethernet Shield:

	make -f buildScript.mk arduinomega_debug

- All Lite Stack Modules in Release (Lite Stack, Lite Unit Tests, Lite
  Examples) for Arduino ATMega 2560 with WiFi Shield:

	make -f buildScript.mk arduinomega_wifi

- All Lite Stack Modules in Debug (Lite Stack, Lite Unit Tests, Lite Examples)
  for Arduino ATMega 2560 with WiFi Shield:

	make -f buildScript.mk arduinomega_wifi_debug

=======================================
==      All Targets - Due Only      ==
=======================================

- All Lite Stack Modules in Release (Lite Stack, Lite Unit Tests, Lite
  Examples) for Arduino Due with Ethernet Shield:

	make -f buildScript.mk arduinodue_all

- All Lite Stack Modules in Debug (Lite Stack, Lite Unit Tests, Lite Examples)
  Examples) for Arduino Due with Ethernet Shield:

	make -f buildScript.mk arduinodue_all_debug

- All Lite Stack Modules in Release (Lite Stack, Lite Unit Tests, Lite
  Examples) for Arduino Due with WiFi Shield:

	make -f buildScript.mk arduinodue_wifi

- All Lite Stack Modules in Debug (Lite Stack, Lite Unit Tests, Lite Examples)
  for Arduino Due with WiFi Shield:

	make -f buildScript.mk arduinodue_wifi_debug

=======================================
==         Clean All Targets         ==
=======================================

- Clean All:

	make -f buildScript.mk clean

