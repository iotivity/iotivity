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
# Note:  Refer to READMEs at "<oic-resource>/resource/" &
#        "<oic-resource>/resource/csdk/" for more information.
#
# Linux:
#    OS: Ubuntu 12.04
#    GCC Version: 4.6.1
#
# Arduino ATMega 2560:
#    Framework Version: Arduino 1.0.5
#    AVR-GCC Version: 4.5.3

# Header Description:
# ====NAME/TITLE - BUILD TYPE - OUT DIRECTORY====

###############################################################################
## TB Stack, TB Examples, UB Stack, & UB Examples - Linux Only               ##
###############################################################################

linux: linux_release

linux_release: linux_tb_release linux_ub_release

linux_debug: linux_tb_debug linux_ub_debug

###############################################################################
####      TB Stack & TB Examples - Linux Only                              ####
###############################################################################

linux_tb: linux_tb_release

linux_tb_release: linux_tb_stack_release linux_tb_examples_release

linux_tb_debug: linux_tb_stack_debug linux_tb_examples_debug

###############################################################################
####      UB Stack, TB Stack, & UB Examples - Linux Only                   ####
###############################################################################

linux_ub: linux_ub_release

linux_ub_release: linux_tb_stack_release linux_ub_stack_release linux_ub_examples_release

linux_ub_debug: linux_tb_stack_debug linux_ub_stack_debug linux_ub_examples_debug

###############################################################################
####      TB Stack - Linux Only                                            ####
###############################################################################

linux_tb_stack: linux_tb_stack_release

linux_tb_stack_debug: clean
	@echo "=====BUILD LIBCOAP FOR LINUX - DEBUG - <oic-resource>/resource/csdk/libcoap/linux/debug/====="
	@cd ./resource/csdk/libcoap && $(MAKE) "BUILD=debug" "PLATFORM=linux"
	@echo "=====BUILD TB STACK FOR LINUX - DEBUG - <oic-resource>/resource/csdk/linux/debug/====="
	@cd ./resource/csdk && $(MAKE) "all" "BUILD=debug" "PLATFORM=linux"

linux_tb_stack_release: clean
	@echo "=====BUILD LIBCOAP FOR LINUX - RELEASE - <oic-resource>/resource/csdk/libcoap/linux/release/====="
	@cd ./resource/csdk/libcoap && $(MAKE) "BUILD=release" "PLATFORM=linux"
	@echo "=====BUILD TB STACK FOR LINUX - RELEASE - <oic-resource>/resource/csdk/linux/release/====="
	@cd resource/csdk && $(MAKE) "all" "BUILD=release" "PLATFORM=linux"

###############################################################################
####      TB Examples - Linux Only                                         ####
###############################################################################

linux_tb_examples: linux_tb_stack_release linux_tb_examples_release

linux_tb_examples_release: linux_tb_stack_release
	@echo "=====BUILD TB SAMPLE APPS FOR LINUX - RELEASE - <oic-resource>/resource/csdk/stack/samples/linux/SimpleClientServer/release/====="
	@cd ./resource/csdk/stack/samples/linux/SimpleClientServer && $(MAKE) "PLATFORM=linux" "BUILD=release"

linux_tb_examples_debug: linux_tb_stack_debug
	@echo "=====BUILD TB SAMPLE APPS FOR LINUX - DEBUG - <oic-resource>/resource/csdk/stack/samples/linux/SimpleClientServer/debug/====="
	@cd ./resource/csdk/stack/samples/linux/SimpleClientServer && $(MAKE) "PLATFORM=linux" "BUILD=debug"

###############################################################################
####      UB Stack (& TB Stack as prerequisite) - Linux Only               ####
###############################################################################

linux_ub_stack: linux_ub_stack_release

linux_ub_stack_release: linux_tb_stack_release
	@echo "=====BUILD UB STACK FOR LINUX - RELEASE - <oic-resource>/resource/release/====="
	@cd ./resource/ && $(MAKE) "all" "BUILD=release"

linux_ub_stack_debug: linux_tb_stack_debug
	@echo "=====BUILD UB STACK FOR LINUX - DEBUG - <oic-resource>/resource/debug/====="
	@cd ./resource/ && $(MAKE) "all" "BUILD=debug"

###############################################################################
####      UB Examples (& UB Stack as prerequisite) - Linux Only            ####
###############################################################################

linux_ub_examples: linux_ub_examples_release

linux_ub_examples_release: linux_ub_stack_release
	@echo "=====BUILD UB SAMPLE APPS FOR LINUX - RELEASE - <oic-resource>/resource/examples/release/====="
	@cd ./resource/examples/ && $(MAKE) "apps" "BUILD=release"

linux_ub_examples_debug: linux_ub_stack_debug
	@echo "=====BUILD UB SAMPLE APPS FOR LINUX - DEBUG - <oic-resource>/resource/examples/debug/====="
	@cd ./resource/examples/  && $(MAKE) "apps" "BUILD=debug"

###############################################################################
####      TB Stack, TB Examples, UB Stack, UB Examples,                    ####
####      & OCICUC - Linux Only                                            ####
###############################################################################

linux_ub_dev: linux_ub_dev_release

linux_ub_dev_release: linux_ub_stack_release linux_ub_examples_release linux_tb_examples_release
	@echo "=====BUILD UB OCICUC - <oic-resource>/resource/examples/ocicuc/====="
	@cd ./resource/examples/ocicuc/ && $(MAKE) "BUILD=release"

linux_ub_dev_debug: linux_ub_stack_debug linux_ub_examples_debug linux_tb_examples_debug
	@echo "=====BUILD UB OCICUC - <oic-resource>/resource/examples/ocicuc/====="
	@cd ./resource/examples/ocicuc/ && $(MAKE) "BUILD=debug"



###############################################################################
##      TB Stack & TB Examples - Arduino Mega Only - Ethernet                ##
###############################################################################

arduinomega: arduinomega_release

arduinomega_all: arduinomega_release arduinomega_debug

arduinomega_all_dev: arduinomega_release_dev arduinomega_debug_dev

arduinomega_release: arduinomega_ethernet_release

arduinomega_debug: arduinomega_ethernet_debug

arduinomega_release_dev: arduinomega_ethernet_release

arduinomega_debug_dev: arduinomega_ethernet_debug

###############################################################################
## TB Stack & TB Examples - Arduino Mega Only - Ethernet Only                ##
###############################################################################

arduinomega_ethernet: arduinomega_ethernet_release

arduinomega_ethernet_all: arduinomega_ethernet_release arduinomega_ethernet_debug

arduinomega_ethernet_release: arduinomega_stack_ethernet_release arduinomega_samples_ethernet_release

arduinomega_ethernet_debug: arduinomega_stack_ethernet_debug arduinomega_samples_ethernet_debug

###############################################################################
####             TB Stack - Arduino Mega Only - Ethernet Only              ####
###############################################################################

arduinomega_stack_ethernet: arduinomega_stack_ethernet_release

arduinomega_stack_ethernet_all: arduinomega_stack_ethernet_release arduinomega_stack_ethernet_debug

arduinomega_stack_ethernet_release: clean
	@echo "=====BUILD C STACK FOR ARDUINO ATMEGA 2560 - RELEASE - ETHERNET SHIELD======"
	@echo "=====BUILD LIBCOAP - RELEASE - <oic-resource>/resource/csdk/libcoap/arduinomega/ethernet_shield/release/====="
	@cd ./resource/csdk/libcoap/ && $(MAKE) "BUILD=release" "PLATFORM=arduinomega" "ARDUINOWIFI=0"
	@echo "=====BUILD TB STACK - RELEASE - <oic-resource>/resource/csdk/arduinomega/ethernet_shield/release/====="
	@cd ./resource/csdk && $(MAKE) "all" "BUILD=release" "PLATFORM=arduinomega" "ARDUINOWIFI=0"

arduinomega_stack_ethernet_debug: clean
	@echo "=====BUILD C STACK FOR ARDUINO ATMEGA 2560 - DEBUG - ETHERNET SHIELD======"
	@echo "=====BUILD LIBCOAP - DEBUG - <oic-resource>/resource/csdk/libcoap/arduinomega/ethernet_shield/debug/====="
	@cd ./resource/csdk/libcoap/ && $(MAKE) "BUILD=debug" "PLATFORM=arduinomega" "ARDUINOWIFI=0"
	@echo "=====BUILD TB STACK - DEBUG - <oic-resource>/resource/csdk/arduinomega/ethernet_shield/debug/====="
	@cd ./resource/csdk && $(MAKE) "all" "BUILD=debug" "PLATFORM=arduinomega" "ARDUINOWIFI=0"

###############################################################################
####          TB Examples - Arduino Mega Only - Ethernet Only              ####
###############################################################################

arduinomega_samples_ethernet: arduinomega_samples_ethernet_release

arduinomega_samples_ethernet_all: arduinomega_samples_ethernet_release arduinomega_samples_ethernet_debug

arduinomega_samples_ethernet_release: arduinomega_stack_ethernet_release
	@echo "=====BUILD TB SAMPLE APPS FOR ARDUINO ATMEGA 2560 - RELEASE - ETHERNET SHIELD====="
	@echo "=====BUILD OCSERVER - RELEASE - <oic-resource>/resource/csdk/stack/samples/arduino/SimpleClientServer/ocserver/arduinomega/ethernet_shield/release/====="
	@cd ./resource/csdk/stack/samples/arduino/SimpleClientServer/ocserver/ && $(MAKE) "BUILD=release" "PLATFORM=arduinomega" "ARDUINOWIFI=0"

arduinomega_samples_ethernet_debug: arduinomega_stack_ethernet_debug
	@echo "=====BUILD TB SAMPLE APPS FOR ARDUINO ATMEGA 2560 - DEBUG - ETHERNET SHIELD====="
	@echo "=====BUILD OCSERVER - DEBUG - <oic-resource>/resource/csdk/stack/samples/arduino/SimpleClientServer/ocserver/arduinomega/ethernet_shield/debug/====="
	@cd ./resource/csdk/stack/samples/arduino/SimpleClientServer/ocserver/ && $(MAKE) "BUILD=debug" "PLATFORM=arduinomega" "ARDUINOWIFI=0"


###############################################################################
##       TB Stack & TB Examples - Arduino Due Only - Ethernet                ##
###############################################################################

arduinodue: arduinodue_release

arduinodue_all: arduinodue_release arduinodue_debug

arduinodue_all_dev: arduinodue_release_dev arduinodue_debug_dev

arduinodue_release: arduinodue_ethernet_release

arduinodue_debug: arduinodue_ethernet_debug

arduinodue_release_dev: arduinodue_ethernet_release

arduinodue_debug_dev: arduinodue_ethernet_debug

###############################################################################
##        TB Stack& TB Examples - Arduino Due Only - Ethernet Only           ##
###############################################################################

arduinodue_ethernet: arduinodue_ethernet_release

arduinodue_ethernet_all: arduinodue_ethernet_release arduinodue_ethernet_debug

arduinodue_ethernet_release: arduinodue_stack_ethernet_release arduinodue_samples_ethernet_release

arduinodue_ethernet_debug: arduinodue_stack_ethernet_debug arduinodue_samples_ethernet_debug

###############################################################################
####             TB Stack - Arduino Due Only - Ethernet Only               ####
###############################################################################

arduinodue_stack_ethernet: arduinodue_stack_ethernet_release

arduinodue_stack_ethernet_all: arduinodue_stack_ethernet_release arduinodue_stack_ethernet_debug

arduinodue_stack_ethernet_release: clean
	@echo "=====BUILD C STACK FOR ARDUINO DUE - RELEASE - ETHERNET SHIELD======"
	@echo "=====BUILD LIBCOAP - RELEASE - <oic-resource>/resource/csdk/libcoap/arduinodue/ethernet_shield/release/====="
	@cd ./resource/csdk/libcoap/ && $(MAKE) "BUILD=release" "PLATFORM=arduinodue" "ARDUINOWIFI=0"
	@echo "=====BUILD TB STACK - RELEASE - <oic-resource>/resource/csdk/arduinodue/ethernet_shield/release/====="
	@cd ./resource/csdk/ && $(MAKE) "all" "BUILD=release" "PLATFORM=arduinodue" "ARDUINOWIFI=0"

arduinodue_stack_ethernet_debug: clean
	@echo "=====BUILD C STACK FOR ARDUINO DUE - DEBUG - ETHERNET SHIELD======"
	@echo "=====BUILD LIBCOAP - DEBUG - <oic-resource>/resource/csdk/libcoap/arduinodue/ethernet_shield/debug/====="
	@cd ./resource/csdk/libcoap/ && $(MAKE) "BUILD=debug" "PLATFORM=arduinodue" "ARDUINOWIFI=0"
	@echo "=====BUILD TB STACK - DEBUG - <oic-resource>/resource/csdk/arduinodue/ethernet_shield/debug/====="
	@cd ./resource/csdk/ && $(MAKE) "all" "BUILD=debug" "PLATFORM=arduinodue" "ARDUINOWIFI=0"

###############################################################################
####          TB Examples - Arduino Due Only - Ethernet Only               ####
###############################################################################

arduinodue_samples_ethernet: arduinodue_samples_ethernet_release

arduinodue_samples_ethernet_all: arduinodue_samples_ethernet_release arduinodue_samples_ethernet_debug

arduinodue_samples_ethernet_release: arduinodue_stack_ethernet_release
	@echo "=====BUILD TB SAMPLE APPS FOR ARDUINO DUE - RELEASE - ETHERNET SHIELD====="
	@echo "=====BUILD OCSERVER - RELEASE - <oic-resource>/resource/csdk/stack/samples/arduino/SimpleClientServer/ocserver/arduinodue/ethernet_shield/release/====="
	@cd ./resource/csdk/stack/samples/arduino/SimpleClientServer/ocserver/ && $(MAKE) -f makefiledue "BUILD=release" "PLATFORM=arduinodue" "ARDUINOWIFI=0"

arduinodue_samples_ethernet_debug: arduinodue_stack_ethernet_debug
	@echo "=====BUILD TB SAMPLE APPS FOR ARDUINO DUE - DEBUG - ETHERNET SHIELD====="
	@echo "=====BUILD OCSERVER - DEBUG - <oic-resource>/resource/csdk/stack/samples/arduino/SimpleClientServer/ocserver/arduinodue/ethernet_shield/debug/====="
	@cd ./resource/csdk/stack/samples/arduino/SimpleClientServer/ocserver/ && $(MAKE) -f makefiledue "BUILD=debug" "PLATFORM=arduinodue" "ARDUINOWIFI=0"


###############################################################################
####  Clean All - Linux, Arduino Mega & Arduino Due - Ethernet             ####
###############################################################################
clean:
	@cd ./resource/csdk/ && $(MAKE) "clean"
	@cd ./resource/csdk/ && $(MAKE) "deepclean"
	@cd ./resource/csdk/stack/samples/linux/SimpleClientServer/ && $(MAKE) "clean"
	@cd ./resource/csdk/stack/samples/arduino/SimpleClientServer/ocserver/ && $(MAKE) "clean"
	@cd ./resource/csdk/stack/samples/arduino/SimpleClientServer/ocserver/ && $(MAKE) "f=arduinodue" "clean"
	@cd ./resource/ && $(MAKE) "clean"
	@cd ./resource/examples $(MAKE) "clean"

###############################################################################
# Building the 'all' target is not appropriate for M1 release. The only
# appropriate builds for M1 are "Linux Rich modules and/or Linux Lite modules
# only, Arduino Mega Lite modules only, or Arduino Due Lite modules only.
###############################################################################
all:
	$(error "Building the 'all' target is not appropriate for M1 release.\
	The only appropriate builds for M1 are "Linux Rich modules and/or Linux\
	Lite modules only, Arduino Mega Lite modules only, or Arduino Due Lite\
	modules only. For more information, please read the README at the root\
	of the repository.")

###############################################################################
###   Building the 'arduino' target is not appropriate for M1 release.      ###
###############################################################################
arduino:
	$(error "Building the 'arduino' target is not appropriate for M1\
	release. Please choose either "arduinomega" or "arduinodue" as\
	your target.")
