#/******************************************************************
# *
# * Copyright 2014 Samsung Electronics All Rights Reserved.
# *
# *
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *      http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
# *
#******************************************************************/

##
# override with `make BUILD=debug`
# override with `make PLATFORM=arduinomega` or `make PLATFORM=arduinodue`
# default to release build
# default to build for linux
BUILD	 := debug
PLATFORM := linux
TRANSPORT := ALL
ARDUINO_DIR := /usr/share/arduino
APPNAME := casample
OBJ_DIR := ./bin
VERSION_MAJOR := 0
VERSION_MINOR := 1
VERSION_PATCH := 0

ifeq ($(PLATFORM),tizen)
$(info *******************tizen Build !!******************)
all:
	sh tizen/gbsbuild.sh $(TRANSPORT)

clean:
	sh tizen/gbsbuild.sh clean

else ifeq ($(PLATFORM),arduinomega)
$(info *********ArduinoMega Build!!**********)
all:
	sh arduino/build.sh clean $(PLATFORM) $(TRANSPORT) $(ARDUINO_DIR) $(APPNAME)
	sh arduino/build.sh make $(PLATFORM) $(TRANSPORT) $(ARDUINO_DIR) $(APPNAME)

install:
	sh arduino/build.sh install $(PLATFORM) $(TRANSPORT) $(ARDUINO_DIR) $(APPNAME)

clean:
	sh arduino/build.sh clean

else
   $(info *********error**********)
   $(error Wrong value for PLATFORM !!)
endif



