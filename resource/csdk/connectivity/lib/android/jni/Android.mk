#/******************************************************************
#*
#* Copyright 2015 Samsung Electronics All Rights Reserved.
#*
#*
#*
#* Licensed under the Apache License, Version 2.0 (the "License");
#* you may not use this file except in compliance with the License.
#* You may obtain a copy of the License at
#*
#*      http://www.apache.org/licenses/LICENSE-2.0
#*
#* Unless required by applicable law or agreed to in writing, software
#* distributed under the License is distributed on an "AS IS" BASIS,
#* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#* See the License for the specific language governing permissions and
#* limitations under the License.
#*
#******************************************************************/
#define build type
BUILD = debug

#you can set the path to ../../../../../extlibs/glib
GLIB_SRC_PATH	?= $(YOUR_GLIB_PATH)/glib-2.40.2

DEFINE_FLAG = -D__ANDROID__


BUILD_FLAG.debug	= $(DEFINE_FLAG)
BUILD_FLAG = $(BUILD_FLAG.$(BUILD))

LOCAL_PATH = ../

#Build glib
include $(CLEAR_VARS)
include $(GLIB_SRC_PATH)/Android.mk
