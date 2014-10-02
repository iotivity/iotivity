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

# Header Description:
# ====NAME/TITLE - BUILD TYPE - OUT DIRECTORY====

all: linux arduinomega

all_dev: linux_ub_dev arduinomega

linux: linux_tb linux_ub

linux_tb: linux_tb_stack linux_tb_unittests linux_tb_examples

linux_ub: linux_tb linux_ub_stack linux_ub_examples

linux_tb_stack:
	@echo "=====BUILD FOR LINUX - UBUNTU 12.04===="
	@echo "=====BUILD LIBCOAP - DEBUG - <oic-resource>/csdk/libcoap/linux/debug/====="
	$(MAKE) -C csdk/libcoap "BUILD=debug" "PLATFORM=linux"
	@echo "=====BUILD LIBCOAP - RELEASE - <oic-resource>/csdk/libcoap/linux/release/====="
	$(MAKE) -C csdk/libcoap "BUILD=release" "PLATFORM=linux"
	@echo "=====BUILD TB STACK - DEBUG - <oic-resource>/csdk/linux/debug/====="
	$(MAKE) -C csdk/ "buildScript_all" "BUILD=debug" "PLATFORM=linux"
	@echo "=====BUILD TB STACK - RELEASE - <oic-resource>/csdk/linux/release/====="
	$(MAKE) -C csdk/ "buildScript_all" "BUILD=release" "PLATFORM=linux"

linux_tb_unittests: linux_tb_stack
	@echo "=====BUILD TB UNITTESTS - DEBUG - <oic-resource>/csdk/stack/test/linux/debug/====="
	$(MAKE) -C csdk/stack/test/linux/ "BUILD=debug"
	@echo "=====BUILD TB UNITTESTS - RELEASE - <oic-resource>/csdk/stack/test/linux/release/====="
	$(MAKE) -C csdk/stack/test/linux/ "BUILD=release"

linux_tb_examples: linux_tb_stack
	@echo "=====BUILD TB SAMPLE APPS - DEBUG - <oic-resource>/csdk/stack/samples/linux/SimpleClientServer/debug/====="
	$(MAKE) -C csdk/stack/samples/linux/SimpleClientServer "PLATFORM=linux" "BUILD=debug"
	@echo "=====BUILD TB SAMPLE APPS - RELEASE - <oic-resource>/csdk/stack/samples/linux/SimpleClientServer/release/====="
	$(MAKE) -C csdk/stack/samples/linux/SimpleClientServer "PLATFORM=linux" "BUILD=release"

linux_ub_stack: linux_tb_stack
	@echo "=====BUILD UB STACK AND SAMPLE APPS AND OCICUC - DEBUG - <oic-resource>/debug/====="
	$(MAKE) -C ./ "buildScript_all" "BUILD=debug"
	@echo "=====BUILD UB STACK AND SAMPLE APPS AND OCICUC - RELEASE - <oic-resource>/release/====="
	$(MAKE) -C ./ "buildScript_all" "BUILD=release"

linux_ub_examples: linux_ub_stack
	@echo "=====BUILD UB SAMPLE APPS - DEBUG - <oic-resource>/examples/debug====="
	$(MAKE) -C examples/ "buildScript_all" "BUILD=debug"
	@echo "=====BUILD UB SAMPLE APPS - RELEASE - <oic-resource>/examples/release====="
	$(MAKE) -C examples/ "buildScript_all" "BUILD=release"

linux_ub_dev: linux
	@echo "=====BUILD UB OCICUC - <oic-resource>/examples/ocicuc====="
	$(MAKE) -C examples/ocicuc/

arduinomega: arduinomega_ethernet

arduinomega_ethernet: arduinomega_stack_ethernet arduinomega_samples_ethernet

arduinomega_stack_ethernet:
	@echo "=====BUILD C STACK FOR ARDUINO ATMEGA 2560 - ETHERNET SHIELD======"
	@echo "=====BUILD LIBCOAP - DEBUG - <oic-resource>/csdk/libcoap/arduinomega/ethernet_shield/debug/====="
	$(MAKE) -C csdk/libcoap "BUILD=debug" "PLATFORM=arduinomega" "ARDUINOWIFI=0"
	@echo "=====BUILD LIBCOAP - RELEASE - <oic-resource>/csdk/libcoap/arduinomega/ethernet_shield/release/====="
	$(MAKE) -C csdk/libcoap "BUILD=release" "PLATFORM=arduinomega" "ARDUINOWIFI=0"
	@echo "=====BUILD TB STACK - DEBUG - <oic-resource>/csdk/arduinomega/ethernet_shield/debug/====="
	$(MAKE) -C csdk "buildScript_all" "BUILD=debug" "PLATFORM=arduinomega" "ARDUINOWIFI=0"
	@echo "=====BUILD TB STACK - RELEASE - <oic-resource>/csdk/arduinomega/ethernet_shield/release/====="
	$(MAKE) -C csdk "buildScript_all" "BUILD=release" "PLATFORM=arduinomega" "ARDUINOWIFI=0"

arduinomega_samples_ethernet:
	@echo "=====BUILD TB SAMPLE APPS FOR ARDUINO ATMEGA 2560 - ETHERNET SHIELD====="
	@echo "=====BUILD OCSERVER - DEBUG - <oic-resource>/csdk/stack/samples/arduino/SimpleClientServer/ocserver/arduinomega/ethernet_shield/debug/====="
	$(MAKE) -C csdk/stack/samples/arduino/SimpleClientServer/ocserver "BUILD=debug" "PLATFORM=arduinomega" "ARDUINOWIFI=0"
	@echo "=====BUILD OCSERVER - RELEASE - <oic-resource>/csdk/stack/samples/arduino/SimpleClientServer/ocserver/arduinomega/ethernet_shield/release/====="
	$(MAKE) -C csdk/stack/samples/arduino/SimpleClientServer/ocserver "BUILD=release" "PLATFORM=arduinomega" "ARDUINOWIFI=0"

linux_clean:
	-rm -rf -C csdk/libcoap/linux
	-rm -rf -C csdk/linux
	$(MAKE) -C csdk/stack/test/linux "clean"
	$(MAKE) -C csdk/stack/samples/linux/SimpleClientServer "clean"
	$(MAKE) -C . "clean"

arduinomega_clean:
	-rm -rf csdk/libcoap/arduinomega
	-rm -rf csdk/arduinomega
	-rm -rf csdk/stack/test/arduino/arduinomega
	-rm -rf csdk/stack/samples/arduino/SimpleClientServer/ocserver/arduinomega

clean:
	$(MAKE) -C csdk "clean" "deepclean"
	$(MAKE) -C csdk/stack/test/linux "clean"
	$(MAKE) -C csdk/stack/samples/linux/SimpleClientServer "clean"
	$(MAKE) -C csdk/stack/samples/arduino/SimpleClientServer/ocserver "clean"
	$(MAKE) -C . "clean"
	$(MAKE) -C oc_logger/ "clean"
