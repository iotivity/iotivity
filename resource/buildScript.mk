# //******************************************************************
# //
# // Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
# //
# //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# //
# // Licensed under the Apache License, Version 2.0 (the "License");
# // you may not use this file except in compliance with the License.
# // You may obtain a copy of the License at
# //
# //      http://www.apache.org/licenses/LICENSE-2.0
# //
# // Unless required by applicable law or agreed to in writing, software
# // distributed under the License is distributed on an "AS IS" BASIS,
# // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# // See the License for the specific language governing permissions and
# // limitations under the License.
# //
# //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#

# This makefile build-script requires the following for each platform:
# Note:  Refer to READMEs at "<oic-resource>/" & "<oic-resource>/csdk/" for
#        more information.
#
# Linux:
#    OS: Ubuntu 12.04
#    GCC Version: 4.6.1
#
# Arduino ATMega 2560:
#    Framework Version: Arduino 1.0.5
#    AVR-GCC Version: 4.5.3
#
# Arduino Due:
#    Framework Version: Arduino 1.5.7
#    AVR-GCC Version: 4.8.3

# Header Description:
# ====NAME/TITLE - BUILD TYPE - OUT DIRECTORY====

###############################################################################
####       TB Stack, TB Unit Tests, TB Examples, UB Stack,                 ####
####       & UB Examples - Linux & Arduino Mega                            ####
#### Note: Default "all" target builds Release only "all" of Linux &       ####
####       Release only "all" of Arduino Mega with Ethernet Shield.        ####
###############################################################################

all: all_release

all_all: all_release all_debug

dev: linux_ub_dev arduinomega_all_dev

all_release: linux_release arduinomega_release

all_debug: linux_debug arduinomega_debug

###############################################################################
####       TB Stack, TB Unit Tests, TB Examples, UB Stack,                 ####
####       & UB Examples - Linux & Arduino Due                             ####
#### Note: Default "all" target builds Release only "all" of Linux &       ####
####       Release only "all" of Arduino Due with Ethernet Shield.         ####
###############################################################################

complement: complement_release

complement_all: complement_release complement_debug

complement_dev: linux_ub_dev arduinodue_all_dev

complement_release: linux_release arduinodue_release

complement_debug: linux_debug arduinodue_debug

###############################################################################
####       TB Stack, TB Unit Tests, TB Examples, UB Stack, UB Examples,    ####
####       & OCICUC - Linux, Arduino Mega and Arduino Due                  ####
#### Note: The "all_dev" target is intended for use on the build           ####
####       server(s). This includes "all" targets for Release & Debug for  ####
####       Linux, "all" Release & Debug for Arduino Mega with Ethernet     ####
####       Shield & WiFi Shield, and "all" Release & Debug for Arduino     ####
####       Due with Ethernet Shield & Wifi Shield.                         ####
###############################################################################

all_dev: linux_ub_dev arduinomega_all_dev arduinodue_all_dev

###############################################################################
## TB Stack, TB Unit Tests, TB Examples, UB Stack, & UB Examples - Linux Only #
###############################################################################

linux: linux_tb_release

linux_all: linux_release linux_debug

linux_release: linux_tb_release linux_ub_release

linux_debug: linux_tb_debug linux_ub_debug

###############################################################################
####      TB Stack, TB Unit Tests, & TB Examples - Linux Only              ####
###############################################################################

linux_tb: linux_tb_release

linux_tb_all: linux_tb_release linux_tb_debug

linux_tb_release: linux_tb_stack_release linux_tb_unittests_release linux_tb_examples_release

linux_tb_debug: linux_tb_stack_debug linux_tb_unittests_debug linux_tb_examples_debug

###############################################################################
####      UB Stack, TB Stack, & UB Examples - Linux Only                   ####
###############################################################################

linux_ub: linux_ub_release

linux_ub_all: linux_ub_release linux_ub_debug

linux_ub_release: linux_tb_stack_release linux_ub_unittests_release linux_ub_stack_release linux_ub_examples_release

linux_ub_debug: linux_tb_stack_debug linux_ub_unittests_debug linux_ub_stack_debug linux_ub_examples_debug

###############################################################################
####      TB Stack - Linux Only                                            ####
###############################################################################

linux_tb_stack: linux_tb_stack_release

linux_tb_stack_all: linux_tb_stack_release linux_tb_stack_debug

linux_tb_stack_debug:
	@echo "=====BUILD LIBCOAP FOR LINUX - DEBUG - <oic-resource>/csdk/libcoap/linux/debug/====="
	$(MAKE) -C csdk/libcoap "BUILD=debug" "PLATFORM=linux"
	@echo "=====BUILD TB STACK FOR LINUX - DEBUG - <oic-resource>/csdk/linux/debug/====="
	$(MAKE) -C csdk/ "buildScript_all" "BUILD=debug" "PLATFORM=linux"

linux_tb_stack_release:
	@echo "=====BUILD LIBCOAP FOR LINUX - RELEASE - <oic-resource>/csdk/libcoap/linux/release/====="
	$(MAKE) -C csdk/libcoap "BUILD=release" "PLATFORM=linux"
	@echo "=====BUILD TB STACK FOR LINUX - RELEASE - <oic-resource>/csdk/linux/release/====="
	$(MAKE) -C csdk/ "buildScript_all" "BUILD=release" "PLATFORM=linux"

###############################################################################
####      TB Unit Tests - Linux Only                                       ####
###############################################################################

linux_tb_unittests: linux_tb_unittests_release

linux_tb_unittests_all: linux_tb_unittests_release linux_tb_unittests_debug

linux_tb_unittests_release: linux_tb_stack_release
	@echo "=====BUILD TB UNITTESTS FOR LINUX - RELEASE - <oic-resource>/csdk/stack/test/linux/release/====="
	$(MAKE) -C csdk/stack/test/linux/ "BUILD=release"

linux_tb_unittests_debug: linux_tb_stack_debug
	@echo "=====BUILD TB UNITTESTS FOR LINUX - DEBUG - <oic-resource>/csdk/stack/test/linux/debug/====="
	$(MAKE) -C csdk/stack/test/linux/ "BUILD=debug"

###############################################################################
####      TB Examples - Linux Only                                         ####
###############################################################################

linux_tb_examples: linux_tb_stack_release linux_tb_examples_release

linux_tb_examples_all: linux_tb_stack_release linux_tb_examples_debug

linux_tb_examples_release: linux_tb_stack_release
	@echo "=====BUILD TB SAMPLE APPS FOR LINUX - RELEASE - <oic-resource>/csdk/stack/samples/linux/SimpleClientServer/release/====="
	$(MAKE) -C csdk/stack/samples/linux/SimpleClientServer "PLATFORM=linux" "BUILD=release"
	$(MAKE) -C csdk/stack/samples/linux/secure "PLATFORM=linux" "BUILD=release"

linux_tb_examples_debug: linux_tb_stack_debug
	@echo "=====BUILD TB SAMPLE APPS FOR LINUX - DEBUG - <oic-resource>/csdk/stack/samples/linux/SimpleClientServer/debug/====="
	$(MAKE) -C csdk/stack/samples/linux/SimpleClientServer "PLATFORM=linux" "BUILD=debug"
	$(MAKE) -C csdk/stack/samples/linux/secure "PLATFORM=linux" "BUILD=debug"

###############################################################################
####      UB Stack (& TB Stack as prerequisite) - Linux Only               ####
###############################################################################

linux_ub_stack: linux_ub_stack_release

linux_ub_stack_all: linux_ub_stack_release linux_ub_stack_debug

linux_ub_stack_release: linux_tb_stack_release
	@echo "=====BUILD UB STACK FOR LINUX - RELEASE - <oic-resource>/release/====="
	$(MAKE) -C ./ "buildScript_all" "BUILD=release"

linux_ub_stack_debug: linux_tb_stack_debug
	@echo "=====BUILD UB STACK FOR LINUX - DEBUG - <oic-resource>/debug/====="
	$(MAKE) -C ./ "buildScript_all" "BUILD=debug"

###############################################################################
####      UB Unit Tests (& TB Stack as prequisite) - Linux Only            ####
###############################################################################

linux_ub_unittests: linux_ub_unittests_release

linux_ub_unittests_all: linux_ub_unittests_release linux_ub_unittests_debug

linux_ub_unittests_release: linux_ub_stack_release
	@echo "=====BUILD UB UNIT TESTS FOR LINUX - RELEASE - <oic-resource>/unittests/release====="
	$(MAKE) -C unittests/ "BUILD=release"

linux_ub_unittests_debug: linux_ub_stack_debug
	@echo "=====BUILD UB UNIT TESTS FOR LINUX - DEBUG - <oic-resource>/unittests/debug====="
	$(MAKE) -C unittests/ "BUILD=debug"

###############################################################################
####      UB Examples (& UB Stack as prerequisite) - Linux Only            ####
###############################################################################

linux_ub_examples: linux_ub_examples_release

linux_ub_examples_all: linux_ub_examples_release linux_ub_examples_debug

linux_ub_examples_release: linux_ub_stack_release
	@echo "=====BUILD UB SAMPLE APPS FOR LINUX - RELEASE - <oic-resource>/examples/release/====="
	$(MAKE) -C examples/ "buildScript_all" "BUILD=release"

linux_ub_examples_debug: linux_ub_stack_debug
	@echo "=====BUILD UB SAMPLE APPS FOR LINUX - DEBUG - <oic-resource>/examples/debug/====="
	$(MAKE) -C examples/ "buildScript_all" "BUILD=debug"

###############################################################################
####      TB Stack, TB Unit Tests, TB Examples, UB Stack, UB Examples,     ####
####      UB Unit Tests, & OCICUC - Linux Only                             ####
###############################################################################

linux_ub_dev: linux_ub_stack_all linux_ub_unittests_all linux_ub_examples_all linux_tb_examples_all
	@echo "=====BUILD UB OCICUC - <oic-resource>/examples/ocicuc/====="
	$(MAKE) -C examples/ocicuc/

###############################################################################
#TB Stack, TB Unit Tests, & TB Examples - Arduino Mega Only - Ethernet & WiFi #
###############################################################################

arduinomega: arduinomega_release

arduinomega_all: arduinomega_release arduinomega_debug

arduinomega_all_dev: arduinomega_release_dev arduinomega_debug_dev

arduinomega_release: arduinomega_ethernet_release

arduinomega_debug: arduinomega_ethernet_debug

arduinomega_release_dev: arduinomega_ethernet_release arduinomega_wifi_release

arduinomega_debug_dev: arduinomega_ethernet_debug arduinomega_wifi_debug

###############################################################################
## TB Stack, TB Unit Tests, & TB Examples - Arduino Mega Only - Ethernet Only #
###############################################################################

arduinomega_ethernet: arduinomega_ethernet_release

arduinomega_ethernet_all: arduinomega_ethernet_release arduinomega_ethernet_debug

arduinomega_ethernet_release: arduinomega_stack_ethernet_release arduinomega_samples_ethernet_release

arduinomega_ethernet_debug: arduinomega_stack_ethernet_debug arduinomega_samples_ethernet_debug

###############################################################################
## TB Stack, TB Unit Tests, & TB Examples - Arduino Mega Only - Ethernet Only #
###############################################################################

arduinomega_wifi: arduinomega_wifi_release

arduinomega_wifi_all: arduinomega_wifi_release arduinomega_wifi_debug

arduinomega_wifi_release: arduinomega_stack_wifi_release arduinomega_samples_wifi_release

arduinomega_wifi_debug: arduinomega_stack_wifi_debug arduinomega_samples_wifi_debug

###############################################################################
####             TB Stack - Arduino Mega Only - Ethernet Only              ####
###############################################################################

arduinomega_stack_ethernet: arduinomega_stack_ethernet_release

arduinomega_stack_ethernet_all: arduinomega_stack_ethernet_release arduinomega_stack_ethernet_debug

arduinomega_stack_ethernet_release:
	@echo "=====BUILD C STACK FOR ARDUINO ATMEGA 2560 - RELEASE - ETHERNET SHIELD======"
	@echo "=====BUILD LIBCOAP - RELEASE - <oic-resource>/csdk/libcoap/arduinomega/ethernet_shield/release/====="
	$(MAKE) -C csdk/libcoap "BUILD=release" "PLATFORM=arduinomega" "ARDUINOWIFI=0"
	@echo "=====BUILD TB STACK - RELEASE - <oic-resource>/csdk/arduinomega/ethernet_shield/release/====="
	$(MAKE) -C csdk "buildScript_all" "BUILD=release" "PLATFORM=arduinomega" "ARDUINOWIFI=0"

arduinomega_stack_ethernet_debug:
	@echo "=====BUILD C STACK FOR ARDUINO ATMEGA 2560 - DEBUG - ETHERNET SHIELD======"
	@echo "=====BUILD LIBCOAP - DEBUG - <oic-resource>/csdk/libcoap/arduinomega/ethernet_shield/debug/====="
	$(MAKE) -C csdk/libcoap "BUILD=debug" "PLATFORM=arduinomega" "ARDUINOWIFI=0"
	@echo "=====BUILD TB STACK - DEBUG - <oic-resource>/csdk/arduinomega/ethernet_shield/debug/====="
	$(MAKE) -C csdk "buildScript_all" "BUILD=debug" "PLATFORM=arduinomega" "ARDUINOWIFI=0"

###############################################################################
####             TB Stack - Arduino Mega Only - WiFi Only                  ####
###############################################################################

arduinomega_stack_wifi: arduinomega_stack_wifi_release

arduinomega_stack_wifi_all: arduinomega_stack_wifi_release arduinomega_stack_wifi_debug

arduinomega_stack_wifi_release:
	@echo "=====BUILD C STACK FOR ARDUINO ATMEGA 2560 - RELEASE - WiFi SHIELD======"
	@echo "=====BUILD LIBCOAP - RELEASE - <oic-resource>/csdk/libcoap/arduinomega/wifi_shield/release/====="
	$(MAKE) -C csdk/libcoap "BUILD=release" "PLATFORM=arduinomega" "ARDUINOWIFI=1"
	@echo "=====BUILD TB STACK - RELEASE - <oic-resource>/csdk/arduinomega/wifi_shield/release/====="
	$(MAKE) -C csdk "buildScript_all" "BUILD=release" "PLATFORM=arduinomega" "ARDUINOWIFI=1"

arduinomega_stack_wifi_debug:
	@echo "=====BUILD C STACK FOR ARDUINO ATMEGA 2560 - DEBUG - WiFi SHIELD======"
	@echo "=====BUILD LIBCOAP - DEBUG - <oic-resource>/csdk/libcoap/arduinomega/wifi_shield/debug/====="
	$(MAKE) -C csdk/libcoap "BUILD=debug" "PLATFORM=arduinomega" "ARDUINOWIFI=1"
	@echo "=====BUILD TB STACK - DEBUG - <oic-resource>/csdk/arduinomega/wifi_shield/debug/====="
	$(MAKE) -C csdk "buildScript_all" "BUILD=debug" "PLATFORM=arduinomega" "ARDUINOWIFI=1"

###############################################################################
####          TB Examples - Arduino Mega Only - Ethernet Only              ####
###############################################################################

arduinomega_samples_ethernet: arduinomega_samples_ethernet_release

arduinomega_samples_ethernet_all: arduinomega_samples_ethernet_release arduinomega_samples_ethernet_debug

arduinomega_samples_ethernet_release: arduinomega_stack_ethernet_release
	@echo "=====BUILD TB SAMPLE APPS FOR ARDUINO ATMEGA 2560 - RELEASE - ETHERNET SHIELD====="
	@echo "=====BUILD OCSERVER - RELEASE - <oic-resource>/csdk/stack/samples/arduino/SimpleClientServer/ocserver/arduinomega/ethernet_shield/release/====="
	$(MAKE) -C csdk/stack/samples/arduino/SimpleClientServer/ocserver "BUILD=release" "PLATFORM=arduinomega" "ARDUINOWIFI=0"

arduinomega_samples_ethernet_debug: arduinomega_stack_ethernet_debug
	@echo "=====BUILD TB SAMPLE APPS FOR ARDUINO ATMEGA 2560 - DEBUG - ETHERNET SHIELD====="
	@echo "=====BUILD OCSERVER - DEBUG - <oic-resource>/csdk/stack/samples/arduino/SimpleClientServer/ocserver/arduinomega/ethernet_shield/debug/====="
	$(MAKE) -C csdk/stack/samples/arduino/SimpleClientServer/ocserver "BUILD=debug" "PLATFORM=arduinomega" "ARDUINOWIFI=0"

###############################################################################
####             TB Examples - Arduino Mega Only - WiFi Only               ####
###############################################################################

arduinomega_samples_wifi: arduinomega_samples_wifi_release

arduinomega_samples_wifi_all: arduinomega_samples_wifi_release arduinomega_samples_wifi_debug

arduinomega_samples_wifi_release: arduinomega_stack_wifi_release
	@echo "=====BUILD TB SAMPLE APPS FOR ARDUINO ATMEGA 2560 - RELEASE - WIFI SHIELD====="
	@echo "=====BUILD OCSERVER - RELEASE - <oic-resource>/csdk/stack/samples/arduino/SimpleClientServer/ocserver/arduinomega/wifi_shield/release/====="
	$(MAKE) -C csdk/stack/samples/arduino/SimpleClientServer/ocserver "BUILD=release" "PLATFORM=arduinomega" "ARDUINOWIFI=1"

arduinomega_samples_wifi_debug: arduinomega_stack_wifi_debug
	@echo "=====BUILD TB SAMPLE APPS FOR ARDUINO ATMEGA 2560 - DEBUG - WIFI SHIELD====="
	@echo "=====BUILD OCSERVER - DEBUG - <oic-resource>/csdk/stack/samples/arduino/SimpleClientServer/ocserver/arduinomega/wifi_shield/debug/====="
	$(MAKE) -C csdk/stack/samples/arduino/SimpleClientServer/ocserver "BUILD=debug" "PLATFORM=arduinomega" "ARDUINOWIFI=1"


###############################################################################
#TB Stack, TB Unit Tests, & TB Examples - Arduino Due Only - Ethernet & WiFi ##
###############################################################################

arduinodue: arduinodue_release

arduinodue_all: arduinodue_release arduinodue_debug

arduinodue_all_dev: arduinodue_release_dev arduinodue_debug_dev

arduinodue_release: arduinodue_ethernet_release

arduinodue_debug: arduinodue_ethernet_debug

arduinodue_release_dev: arduinodue_ethernet_release arduinodue_wifi_release

arduinodue_debug_dev: arduinodue_ethernet_debug arduinodue_wifi_debug

###############################################################################
## TB Stack, TB Unit Tests, & TB Examples - Arduino Due Only - Ethernet Only ##
###############################################################################

arduinodue_ethernet: arduinodue_ethernet_release

arduinodue_ethernet_all: arduinodue_ethernet_release arduinodue_ethernet_debug

arduinodue_ethernet_release: arduinodue_stack_ethernet_release arduinodue_samples_ethernet_release

arduinodue_ethernet_debug: arduinodue_stack_ethernet_debug arduinodue_samples_ethernet_debug

###############################################################################
## TB Stack, TB Unit Tests, & TB Examples - Arduino Due Only - Ethernet Only ##
###############################################################################

arduinodue_wifi: arduinodue_wifi_release

arduinodue_wifi_all: arduinodue_wifi_release arduinodue_wifi_debug

arduinodue_wifi_release: arduinodue_stack_wifi_release arduinodue_samples_wifi_release

arduinodue_wifi_debug: arduinodue_stack_wifi_debug arduinodue_samples_wifi_debug

###############################################################################
####             TB Stack - Arduino Due Only - Ethernet Only               ####
###############################################################################

arduinodue_stack_ethernet: arduinodue_stack_ethernet_release

arduinodue_stack_ethernet_all: arduinodue_stack_ethernet_release arduinodue_stack_ethernet_debug

arduinodue_stack_ethernet_release:
	@echo "=====BUILD C STACK FOR ARDUINO DUE - RELEASE - ETHERNET SHIELD======"
	@echo "=====BUILD LIBCOAP - RELEASE - <oic-resource>/csdk/libcoap/arduinodue/ethernet_shield/release/====="
	$(MAKE) -C csdk/libcoap "BUILD=release" "PLATFORM=arduinodue" "ARDUINOWIFI=0"
	@echo "=====BUILD TB STACK - RELEASE - <oic-resource>/csdk/arduinodue/ethernet_shield/release/====="
	$(MAKE) -C csdk "buildScript_all" "BUILD=release" "PLATFORM=arduinodue" "ARDUINOWIFI=0"

arduinodue_stack_ethernet_debug:
	@echo "=====BUILD C STACK FOR ARDUINO DUE - DEBUG - ETHERNET SHIELD======"
	@echo "=====BUILD LIBCOAP - DEBUG - <oic-resource>/csdk/libcoap/arduinodue/ethernet_shield/debug/====="
	$(MAKE) -C csdk/libcoap "BUILD=debug" "PLATFORM=arduinodue" "ARDUINOWIFI=0"
	@echo "=====BUILD TB STACK - DEBUG - <oic-resource>/csdk/arduinodue/ethernet_shield/debug/====="
	$(MAKE) -C csdk "buildScript_all" "BUILD=debug" "PLATFORM=arduinodue" "ARDUINOWIFI=0"

###############################################################################
####             TB Stack - Arduino Due Only - WiFi Only                   ####
###############################################################################

arduinodue_stack_wifi: arduinodue_stack_wifi_release

arduinodue_stack_wifi_all: arduinodue_stack_wifi_release arduinodue_stack_wifi_debug

arduinodue_stack_wifi_release:
	@echo "=====BUILD C STACK FOR ARDUINO DUE - RELEASE - WiFi SHIELD======"
	@echo "=====BUILD LIBCOAP - RELEASE - <oic-resource>/csdk/libcoap/arduinodue/wifi_shield/release/====="
	$(MAKE) -C csdk/libcoap "BUILD=release" "PLATFORM=arduinodue" "ARDUINOWIFI=1"
	@echo "=====BUILD TB STACK - RELEASE - <oic-resource>/csdk/arduinodue/wifi_shield/release/====="
	$(MAKE) -C csdk "buildScript_all" "BUILD=release" "PLATFORM=arduinodue" "ARDUINOWIFI=1"

arduinodue_stack_wifi_debug:
	@echo "=====BUILD C STACK FOR ARDUINO DUE - DEBUG - WiFi SHIELD======"
	@echo "=====BUILD LIBCOAP - DEBUG - <oic-resource>/csdk/libcoap/arduinodue/wifi_shield/debug/====="
	$(MAKE) -C csdk/libcoap "BUILD=debug" "PLATFORM=arduinodue" "ARDUINOWIFI=1"
	@echo "=====BUILD TB STACK - DEBUG - <oic-resource>/csdk/arduinodue/wifi_shield/debug/====="
	$(MAKE) -C csdk "buildScript_all" "BUILD=debug" "PLATFORM=arduinodue" "ARDUINOWIFI=1"

###############################################################################
####          TB Examples - Arduino Due Only - Ethernet Only               ####
###############################################################################

arduinodue_samples_ethernet: arduinodue_samples_ethernet_release

arduinodue_samples_ethernet_all: arduinodue_samples_ethernet_release arduinodue_samples_ethernet_debug

arduinodue_samples_ethernet_release: arduinodue_stack_ethernet_release
	@echo "=====BUILD TB SAMPLE APPS FOR ARDUINO DUE - RELEASE - ETHERNET SHIELD====="
	@echo "=====BUILD OCSERVER - RELEASE - <oic-resource>/csdk/stack/samples/arduino/SimpleClientServer/ocserver/arduinodue/ethernet_shield/release/====="
	$(MAKE) -C csdk/stack/samples/arduino/SimpleClientServer/ocserver "BUILD=release" "PLATFORM=arduinodue" "ARDUINOWIFI=0"

arduinodue_samples_ethernet_debug: arduinodue_stack_ethernet_debug
	@echo "=====BUILD TB SAMPLE APPS FOR ARDUINO DUE - DEBUG - ETHERNET SHIELD====="
	@echo "=====BUILD OCSERVER - DEBUG - <oic-resource>/csdk/stack/samples/arduino/SimpleClientServer/ocserver/arduinodue/ethernet_shield/debug/====="
	$(MAKE) -C csdk/stack/samples/arduino/SimpleClientServer/ocserver "BUILD=debug" "PLATFORM=arduinodue" "ARDUINOWIFI=0"

###############################################################################
####             TB Examples - Arduino Due Only - WiFi Only                ####
###############################################################################

arduinodue_samples_wifi: arduinodue_samples_wifi_release

arduinodue_samples_wifi_all: arduinodue_samples_wifi_release arduinodue_samples_wifi_debug

arduinodue_samples_wifi_release: arduinodue_stack_wifi_release
	@echo "=====BUILD TB SAMPLE APPS FOR ARDUINO DUE - RELEASE - WIFI SHIELD====="
	@echo "=====BUILD OCSERVER - RELEASE - <oic-resource>/csdk/stack/samples/arduino/SimpleClientServer/ocserver/arduinodue/wifi_shield/release/====="
	$(MAKE) -C csdk/stack/samples/arduino/SimpleClientServer/ocserver "BUILD=release" "PLATFORM=arduinodue" "ARDUINOWIFI=1"

arduinodue_samples_wifi_debug: arduinodue_stack_wifi_debug
	@echo "=====BUILD TB SAMPLE APPS FOR ARDUINO DUE - DEBUG - WIFI SHIELD====="
	@echo "=====BUILD OCSERVER - DEBUG - <oic-resource>/csdk/stack/samples/arduino/SimpleClientServer/ocserver/arduinodue/wifi_shield/debug/====="
	$(MAKE) -C csdk/stack/samples/arduino/SimpleClientServer/ocserver "BUILD=debug" "PLATFORM=arduinodue" "ARDUINOWIFI=1"

###############################################################################
####                Clean All - Linux Only - Ethernet Only                 ####
###############################################################################

linux_clean:
	-rm -rf -C csdk/libcoap/linux
	-rm -rf -C csdk/linux
	$(MAKE) -C csdk/stack/test/linux "clean"
	$(MAKE) -C csdk/stack/samples/linux/SimpleClientServer "clean"
	$(MAKE) -C csdk/stack/samples/linux/secure "clean"
	$(MAKE) -C . "clean"

###############################################################################
####            Clean All - Arduino Mega Only - Ethernet & WiFi            ####
###############################################################################

arduinomega_clean:
	-rm -rf csdk/libcoap/arduinomega
	-rm -rf csdk/arduinomega
	-rm -rf csdk/stack/test/arduino/arduinomega
	-rm -rf csdk/stack/samples/arduino/SimpleClientServer/ocserver/arduinomega

###############################################################################
####            Clean All - Arduino Due Only - Ethernet & WiFi             ####
###############################################################################

arduinodue_clean:
	-rm -rf csdk/libcoap/arduinodue
	-rm -rf csdk/arduinodue
	-rm -rf csdk/stack/test/arduino/arduinodue
	-rm -rf csdk/stack/samples/arduino/SimpleClientServer/ocserver/arduinodue


###############################################################################
####  Clean All - Linux, Arduino Mega & Arduino Due - Ethernet & WiFi      ####
###############################################################################
clean:
	$(MAKE) -C csdk "clean" "deepclean"
	$(MAKE) -C csdk/stack/test/linux "clean"
	$(MAKE) -C csdk/stack/samples/linux/SimpleClientServer "clean"
	$(MAKE) -C csdk/stack/samples/linux/secure "clean"
	$(MAKE) -C csdk/stack/samples/arduino/SimpleClientServer/ocserver "clean"
	$(MAKE) -C . "clean"
	$(MAKE) -C unittests/ "clean"
	$(MAKE) -C examples/ "clean"
	$(MAKE) -C oc_logger/ "clean"

###############################################################################
### You must specify arduinomega or arduinodue when using an arduino target.###
###############################################################################
arduino:
	$(error "You must specify "arduinomega" or "arduinodue" when trying to\
	build arduino targets.")
