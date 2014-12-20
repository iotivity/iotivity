#******************************************************************
#
# Copyright 2014 Samsung Electronics All Rights Reserved.
#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

PROJECT_NAME		=	controllee

CONFIG			?= 	release

BUILD_DIR		?= 	../../../build

#configure linker program
LINKER 			?=	$(CXX)

#configure Source Root Directory
SRC_DIR 		=	../../../controllee

#configure Source Build Directory
override BUILD_DIR := $(addsuffix /${PROJECT_NAME},$(BUILD_DIR))
override OUTPUT_DIR := $(addsuffix /${PROJECT_NAME},$(OUTPUT_DIR))

#configure CPP Files, include paths, cpp compiler option
override CXXFLAGS += -std=c++0x -DCMAPI_EXPORTS=1 -DSHPPAL_API_EXPORTS=1 -fpermissive
override CPP_INC_DIR = 	-I../../../controllee/include/ \
			-I../../../controllee/include/api \
			-I../../../controllee/include/core \
			-I../../../controllee/include/device \
			-I../../../controllee/include/Notification \
			-I../../../controllee/include/Serialization \
			-I../../../controllee/include/Server \
			-I../../../controllee/include/SubscriptionStore \
			-I../../../controllee/include/utils \
			-I../../../controllee/include/xsd \
			-I../../../opensource/boost/include \
			-I../../../opensource/jsoncpp/include \
			-I../../../opensource/sqlite3/include \
			-I../../../../../../${OUTPUT_OC_DIR}/include \
			-I../../../../../../${OUTPUT_OC_DIR}/dependencies/cereal/include \
			-I../../../../../../${OUTPUT_OC_DIR}/csdk/stack/include \
			-I../../../../../../${OUTPUT_OC_DIR}/csdk/occoap/include \
			-I../../../../../../${OUTPUT_OC_DIR}/csdk/ocrandom/include \
			-I../../../../../../${OUTPUT_OC_DIR}/csdk/ocsocket/include \
			-I../../../../../../${OUTPUT_OC_DIR}/oc_logger/include

override SRC_CPP_FILES = 	api/CDError.cpp\
				api/Configuration.cpp\
				api/Device.cpp\
				api/ControlResponse.cpp\
				api/HeadersList.cpp\
				core/CDContext.cpp\
				core/CDCoreResourceHandlerFactory.cpp\
				core/CDSerializableDataFactory.cpp\
				core/CDServer.cpp\
				device/ControlDevice.cpp\
				device/ControlDeviceFactory.cpp\
				device/ControlleeManager.cpp \
				device/MyDevice.cpp\
				Notification/NotificationResponse.cpp\
				Notification/SubscriptionManager.cpp\
				Notification/SubscriptionResourceHandler.cpp\
				Notification/SubscriptionResourceHandlerFactory.cpp\
				Notification/SubscriptionSerializationFactory.cpp\
				Notification/SubscriptionsResourceHandler.cpp\
				Serialization/CDDeserializer.cpp\
				Serialization/CDErrorMessage.cpp\
				Serialization/CDSerializationFactory.cpp\
				Serialization/CDSerializer.cpp\
				Serialization/IDeserializer.cpp\
				Serialization/ISerializable.cpp\
				Serialization/JsonSerializer.cpp\
				Serialization/JsonDeserializer.cpp\
				Serialization/XSDClassesImpl.cpp\
				Server/AccessPointResourceHandler.cpp\
				Server/AccessPointsResourceHandler.cpp\
				Server/AlarmResourceHandler.cpp\
				Server/AlarmsResourceHandler.cpp\
				Server/AsyncResourceHandler.cpp\
				Server/CapabilityResourceHandler.cpp\
				Server/CDResourceHandlerFactory.cpp\
				Server/ConfigurationResourceHandler.cpp\
				Server/DetectionResourceHandler.cpp\
				Server/DeviceResourceHandler.cpp\
				Server/DevicesResourceHandler.cpp\
				Server/DoorResourceHandler.cpp\
				Server/DoorsResourceHandler.cpp\
				Server/DRLCResourceHandler.cpp\
				Server/EnergyConsumptionResourceHandler.cpp\
				Server/EnergyResourceHandler.cpp\
				Server/HumidityResourceHandler.cpp\
				Server/InformationResourceHandler.cpp\
				Server/LevelResourceHandler.cpp\
				Server/LightResourceHandler.cpp\
				Server/LongPollingNotificationResourceHandler.cpp\
				Server/MessageResourceHandler.cpp\
				Server/MessagesResourceHandler.cpp\
				Server/ModeResourceHandler.cpp\
				Server/MovementResourceHandler.cpp\
				Server/NetworkResourceHandler.cpp\
				Server/NetworksResourceHandler.cpp\
				Server/OperationResourceHandler.cpp\
				Server/RecordResourceHandler.cpp\
				Server/RecordsResourceHandler.cpp\
				Server/RemoteResourceHandler.cpp\
				Server/ResourceHandlerFactory.cpp \
				Server/ResourceTypeEnum.cpp \
				Server/SyncResourceHandler.cpp\
				Server/TemperatureResourceHandler.cpp\
				Server/TemperaturesResourceHandler.cpp\
				Server/ThermostatResourceHandler.cpp\
				Server/TimeResourceHandler.cpp\
				Server/UsageLimitResourceHandler.cpp\
				Server/UsageLimitsResourceHandler.cpp\
				Server/UsagesResourceHandler.cpp\
				Server/VersionResourceHandler.cpp\
				Server/VersionsResourceHandler.cpp\
				Server/WiFiResourceHandler.cpp\
				SubscriptionStore/SubscriptionRecord.cpp\
				SubscriptionStore/SubscriptionSqliteDatabase.cpp\
				utils/IPUtils.cpp\
				utils/Utils.cpp\
				xsd/AccessPoint.cpp\
				xsd/AccessPoints.cpp\
				xsd/AccessPointsLink.cpp\
				xsd/ActionsLink.cpp\
				xsd/Actuators.cpp\
				xsd/Alarm.cpp\
				xsd/Alarms.cpp\
				xsd/ArrayOfAnyURI.cpp\
				xsd/ArrayOfConnectivityType.cpp\
				xsd/ArrayOfDayType.cpp\
				xsd/ArrayOfResourceType.cpp\
				xsd/ArrayOfString.cpp\
				xsd/ArrayOfTimeType.cpp\
				xsd/ArrayOfUuid.cpp\
				xsd/Capability.cpp\
				xsd/Color.cpp\
				xsd/Configuration.cpp\
				xsd/ConfigurationLink.cpp\
				xsd/Detection.cpp\
				xsd/Device.cpp\
				xsd/Devices.cpp\
				xsd/DevicesLink.cpp\
				xsd/Door.cpp\
				xsd/Doors.cpp\
				xsd/DRLC.cpp\
				xsd/Energy.cpp\
				xsd/EnergyConsumption.cpp\
				xsd/EnergyLink.cpp\
				xsd/Events.cpp\
				xsd/FoodManagerLink.cpp\
				xsd/GroupsLink.cpp\
				xsd/Humidity.cpp\
				xsd/IdentifiedObject.cpp\
				xsd/Image.cpp\
				xsd/Information.cpp\
				xsd/InformationLink.cpp\
				xsd/Level.cpp\
				xsd/Light.cpp\
				xsd/Link.cpp\
				xsd/List.cpp\
				xsd/LongPollingNotification.cpp\
				xsd/LongPollingNotificationLink.cpp\
				xsd/Message.cpp\
				xsd/Messages.cpp\
				xsd/MessagesLink.cpp\
				xsd/Mode.cpp\
				xsd/Movement.cpp\
				xsd/Network.cpp\
				xsd/Networks.cpp\
				xsd/Notification.cpp\
				xsd/NotificationsLink.cpp\
				xsd/Operation.cpp\
				xsd/PatternsLink.cpp\
				xsd/Record.cpp\
				xsd/Records.cpp\
				xsd/RecordsLink.cpp\
				xsd/Remote.cpp\
				xsd/Resource.cpp\
				xsd/Specification.cpp\
				xsd/Subscription.cpp\
				xsd/Subscriptions.cpp\
				xsd/SubscriptionsLink.cpp\
				xsd/Temperature.cpp\
				xsd/Temperatures.cpp\
				xsd/Thermostat.cpp\
				xsd/Time.cpp\
				xsd/Usage.cpp\
				xsd/UsageLimit.cpp\
				xsd/UsageLimits.cpp\
				xsd/Usages.cpp\
				xsd/Version.cpp\
				xsd/Versions.cpp\
				xsd/WiFi.cpp\

#configure C Files, include paths, c compiler option
override CFLAGS +=
override C_INC_DIR =
override SRC_C_FILES =

#configure archiver flags
override ARFLAGS = rcs

#configure linker flags
#override LDFLAGS += -L../../../${OUTPUT_OPENSOURCE_DIR}/boost/static/ -L../../../${OUTPUT_OPENSOURCE_DIR}/sqlite3/static -L../../../${OUTPUT_OPENSOURCE_DIR}/jsoncpp/static
override LDFLAGS += -L/usr/lib/ -L../../../${OUTPUT_OPENSOURCE_DIR}/sqlite3/static -L../../../${OUTPUT_OPENSOURCE_DIR}/jsoncpp/static
override LDFLAGS += -L../../../../../../${OUTPUT_OC_DIR}/${CONFIG}/obj -L../../../../../../${OUTPUT_OC_DIR}/csdk/linux/${CONFIG} -L../../../../../../${OUTPUT_OC_DIR}/oc_logger/lib
override LDLIBS += -l${BOOST_SYSTEM_LIB} -l${BOOST_THREAD_LIB} -l${BOOST_REGEX_LIB} -lsqlite3 -ldl -ljsoncpp
override LDLIBS += -l${OC_LIB} -l${OCTBSTACK_LIB} -l${OCLOGGER_LIB}

include ../../platformconfigs/$(TARGET_OS)_config.mk

override LDLIBS += ${RTLIB} ${PTLIB}

#configure target names
STATIC_LIB_TARGET	=	$(OUTPUT_DIR)/static/${STATIC_LIB_PREFIX}${PROJECT_NAME}.${STATIC_LIB_EXTENSION}
SHARED_LIB_TARGET	=	$(OUTPUT_DIR)/shared/${SHARED_LIB_PREFIX}${PROJECT_NAME}.${SHARED_LIB_EXTENSION}
SHARED_IMPORT_LIB_TARGET = $(OUTPUT_DIR)/shared/${SHARED_LIB_PREFIX}${PROJECT_NAME}.${SHARED_LIB_EXTENSION}.a

include ../../BuildTargets.mk
