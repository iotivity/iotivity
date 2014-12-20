#########################################################################################
#@Author : Bhupathi Ranjith Kumar
#@Description: Makefile template
#
#
#########################################################################################

PROJECT_NAME		=	controlmanager

CONFIG			?= 	release

BUILD_DIR		?= 	../../../build

#configure linker program
LINKER 			?=	$(CXX)

#configure Source Root Directory
SRC_DIR 		=	../../../controlmanager

#configure Source Build Directory
override BUILD_DIR := $(addsuffix /${PROJECT_NAME},$(BUILD_DIR))
override OUTPUT_DIR := $(addsuffix /${PROJECT_NAME},$(OUTPUT_DIR))

#configure CPP Files, include paths, cpp compiler option
override CXXFLAGS += -DCMAPI_EXPORTS=1 -DSHPPAL_API_EXPORTS=1 -std=c++0x

ifeq ($(CONFIG),debug)
	override CXXFLAGS += -DCM_LOGGING_ERROR=1 -DCM_LOGGING_WARNING=1 -DCM_LOGGING_INFO=1 -DCM_LOGGING_DEBUG=1
endif

override CPP_INC_DIR = 	-I../../../controlmanager/platform/include \
			-I../../../controlmanager/api/include \
			-I../../../controlmanager/api/include/Client \
			-I../../../controlmanager/api/include/Server \
			-I../../../controlmanager/api/include/xsd \
			-I../../../controlmanager/core/src \
			-I../../../controlmanager/core/src/DeviceFinder \
			-I../../../controlmanager/api/src \
			-I../../../opensource/sqlite3/include \
			-I../../../opensource/jsoncpp/include \
			-I../../../controlmanager/core/src/subscriptionstore/include \
			-I../../../controlmanager/core/src/subscriptionstore/sqlite/src \
			-I../../../controlmanager/core/src/subscriptionmanager/src \
			-I../../../controlmanager/core/src/Connector/Client \
			-I../../../../../../${OUTPUT_OC_DIR}/include \
			-I../../../../../../${OUTPUT_OC_DIR}/dependencies/cereal/include \
			-I../../../../../../${OUTPUT_OC_DIR}/csdk/stack/include \
			-I../../../../../../${OUTPUT_OC_DIR}/csdk/occoap/include \
			-I../../../../../../${OUTPUT_OC_DIR}/csdk/ocrandom/include \
			-I../../../../../../${OUTPUT_OC_DIR}/csdk/ocsocket/include \
			-I../../../opensource/boost/include \
			-I../../../../../../${OUTPUT_OC_DIR}/oc_logger/include
ifeq ($(TARGET_OS),android)
	override CPP_INC_DIR += -I../../../oic-utilities/android/boost/include
else 
	override CPP_INC_DIR += -I/usr/local/boost/include
endif

override SRC_CPP_FILES =	api/src/Connector/HeadersList.cpp\
				api/src/Client/VersionResource.cpp\
				api/src/Client/TemperaturesResource.cpp\
				api/src/Client/DevicesResource.cpp\
				api/src/Client/NotificationsResource.cpp\
				api/src/Client/DetectionResource.cpp\
				api/src/Client/ThermostatResource.cpp\
				api/src/Client/HumidityResource.cpp\
				api/src/Client/NotificationResource.cpp\
				api/src/Client/TimeResource.cpp\
				api/src/Client/ModeResource.cpp\
				api/src/Client/AccessPointResource.cpp\
				api/src/Client/AccessPointsResource.cpp\
				api/src/Client/UsageLimitsResource.cpp\
				api/src/Client/NetworkResource.cpp\
				api/src/Client/SubscriptionsResource.cpp\
				api/src/Client/AlarmsResource.cpp\
				api/src/Client/TemperatureResource.cpp\
				api/src/Client/UsagesResource.cpp\
				api/src/Client/AlarmResource.cpp\
				api/src/Client/CapabilityResource.cpp\
				api/src/Client/UsageLimitResource.cpp\
				api/src/Client/InformationResource.cpp\
				api/src/Client/LightResource.cpp\
				api/src/Client/SubscriptionResource.cpp\
				api/src/Client/VersionsResource.cpp\
				api/src/Client/DeviceResource.cpp\
				api/src/Client/NetworksResource.cpp\
				api/src/Client/WiFiResource.cpp\
				api/src/Client/MovementResource.cpp\
				api/src/Client/ConfigurationResource.cpp\
				api/src/Client/OperationResource.cpp\
				api/src/Client/DoorResource.cpp\
				api/src/Client/EnergyResource.cpp\
				api/src/Client/DoorsResource.cpp\
				api/src/Client/EnergyConsumptionResource.cpp\
				api/src/Client/LongPollingNotificationResource.cpp\
				api/src/Client/Resource/Resource.cpp\
				api/src/Client/LevelResource.cpp\
				api/src/xsd/Records.cpp\
				api/src/xsd/Time.cpp\
				api/src/xsd/List.cpp\
				api/src/xsd/PatternsLink.cpp\
				api/src/xsd/NotificationsLink.cpp\
				api/src/xsd/SubscriptionsLink.cpp\
				api/src/xsd/DevicesLink.cpp\
				api/src/xsd/Message.cpp\
				api/src/xsd/ArrayOfString.cpp\
				api/src/xsd/Link.cpp\
				api/src/xsd/Temperatures.cpp\
				api/src/xsd/Resource.cpp\
				api/src/xsd/Door.cpp\
				api/src/xsd/UsageLimit.cpp\
				api/src/xsd/Events.cpp\
				api/src/xsd/Image.cpp\
				api/src/xsd/Movement.cpp\
				api/src/xsd/Remote.cpp\
				api/src/xsd/Usages.cpp\
				api/src/xsd/LongPollingNotification.cpp\
				api/src/xsd/MessagesLink.cpp\
				api/src/xsd/Networks.cpp\
				api/src/xsd/Doors.cpp\
				api/src/xsd/WiFi.cpp\
				api/src/xsd/Device.cpp\
				api/src/xsd/GroupsLink.cpp\
				api/src/xsd/ActionsLink.cpp\
				api/src/xsd/AccessPointsLink.cpp\
				api/src/xsd/IdentifiedObject.cpp\
				api/src/xsd/Messages.cpp\
				api/src/xsd/Light.cpp\
				api/src/xsd/Level.cpp\
				api/src/xsd/Specification.cpp\
				api/src/xsd/Temperature.cpp\
				api/src/xsd/LongPollingNotificationLink.cpp\
				api/src/xsd/Subscription.cpp\
				api/src/xsd/Actuators.cpp\
				api/src/xsd/Alarms.cpp\
				api/src/xsd/ArrayOfDayType.cpp\
				api/src/xsd/Mode.cpp\
				api/src/xsd/ArrayOfUuid.cpp\
				api/src/xsd/ArrayOfConnectivityType.cpp\
				api/src/xsd/AccessPoints.cpp\
				api/src/xsd/ConfigurationLink.cpp\
				api/src/xsd/Humidity.cpp\
				api/src/xsd/Thermostat.cpp\
				api/src/xsd/Subscriptions.cpp\
				api/src/xsd/Energy.cpp\
				api/src/xsd/ArrayOfTimeType.cpp\
				api/src/xsd/Versions.cpp\
				api/src/xsd/Operation.cpp\
				api/src/xsd/RecordsLink.cpp\
				api/src/xsd/EnergyLink.cpp\
				api/src/xsd/Record.cpp\
				api/src/xsd/Capability.cpp\
				api/src/xsd/UsageLimits.cpp\
				api/src/xsd/Information.cpp\
				api/src/xsd/Usage.cpp\
				api/src/xsd/EnergyConsumption.cpp\
				api/src/xsd/Network.cpp\
				api/src/xsd/Version.cpp\
				api/src/xsd/Configuration.cpp\
				api/src/xsd/Detection.cpp\
				api/src/xsd/Color.cpp\
				api/src/xsd/ArrayOfAnyURI.cpp\
				api/src/xsd/ArrayOfResourceType.cpp\
				api/src/xsd/AccessPoint.cpp\
				api/src/xsd/FoodManagerLink.cpp\
				api/src/xsd/Alarm.cpp\
				api/src/xsd/Devices.cpp\
				api/src/xsd/DRLC.cpp\
				api/src/xsd/Notification.cpp\
				api/src/xsd/InformationLink.cpp\
				api/src/Serialization/xsd/CMErrorMessage.cpp\
				api/src/Serialization/xsd/XSDClassesImpl.cpp\
				api/src/Serialization/CMSerializationFactory.cpp\
				api/src/Serialization/IDeserializer.cpp\
				api/src/Serialization/ISerializable.cpp\
				api/src/CMError.cpp\
				api/src/Device.cpp\
				api/src/Notification/NotificationResponse.cpp\
				api/src/ControlManager.cpp\
				api/src/Configuration.cpp\
				api/src/MyDevice.cpp\
				platform/windows/src/Net/IPUtils.cpp\
				platform/windows/src/Thread/Thread.cpp\
				platform/windows/src/Thread/Mutex.cpp\
				platform/windows/src/Thread/ThreadImpl.cpp\
				platform/windows/src/Thread/RecursiveMutex.cpp\
				platform/windows/src/StringConversion.cpp\
				platform/windows/src/Log.cpp\
				core/src/Connector/Session.cpp\
				core/src/Connector/Client/ClientSessionData.cpp\
				core/src/Connector/Client/ClientSession.cpp\
				core/src/Connector/Server/ServerSession.cpp\
				core/src/Connector/Server/ServerSessionData.cpp\
				core/src/Connector/ControlResponse.cpp\
				core/src/Connector/ResourceIdentifier.cpp\
				core/src/Connector/MethodIdentifier.cpp\
				core/src/Connector/SessionStore.cpp\
				core/src/Connector/ControlRequest.cpp\
				core/src/Server/InformationResourceHandler.cpp\
				core/src/Server/CapabilityResourceHandler.cpp\
				core/src/Server/DeviceResourceHandler.cpp\
				core/src/Server/CMResourceHandlerFactory.cpp\
				core/src/Server/DevicesResourceHandler.cpp\
				core/src/Server/ResourceHandler/AsyncResourceHandler.cpp\
				core/src/Server/ResourceHandler/SyncResourceHandler.cpp\
				core/src/Server/ResourceHandler/ResourceHandlerFactory.cpp\
				core/src/subscriptionmanager/src/ResourceHandlers/SubscriptionResourceHandlerFactory.cpp\
				core/src/subscriptionmanager/src/ResourceHandlers/SubscriptionsResourceHandler.cpp\
				core/src/subscriptionmanager/src/ResourceHandlers/NotificationResourceHandler.cpp\
				core/src/subscriptionmanager/src/ResourceHandlers/NotificationsResourceHandler.cpp\
				core/src/subscriptionmanager/src/ResourceHandlers/SubscriptionResourceHandler.cpp\
				core/src/subscriptionmanager/src/Serialization/SubscriptionSerializationFactory.cpp\
				core/src/subscriptionmanager/src/SubscriptionManager.cpp\
				core/src/Client/OCClient.cpp\
				core/src/Server/OCServer.cpp\
				core/src/Serialization/Json/JsonSerializer.cpp\
				core/src/Serialization/Json/JsonDeserializer.cpp\
				core/src/subscriptionstore/src/SubscriptionRecord.cpp\
				core/src/subscriptionstore/sqlite/src/SubscriptionSqliteDatabase.cpp\
				core/src/DeviceFinder/Service/DeviceFinderServiceDevice.cpp\
				core/src/DeviceFinder/OC/OCDevice.cpp\
				core/src/DeviceFinder/OC/OCDeviceManager.cpp\
				core/src/DeviceFinder/OC/OCDeviceFinder.cpp\
				core/src/DeviceFinder/DeviceFinderImpl.cpp\
				core/src/DeviceFinder/ControlDeviceFactory.cpp\
				core/src/Engine/CMCoreSerializableDataFactory.cpp\
				core/src/Engine/CMCoreResourceHandlerFactory.cpp\
				core/src/Engine/CMEngine.cpp\
				core/src/Utils.cpp\
				core/src/CMContext.cpp\

#configure C Files, include paths, c compiler option
override CFLAGS += 
override C_INC_DIR =
override SRC_C_FILES =

#configure archiver flags
override ARFLAGS = rcs

#configure linker flags
#override LDFLAGS += -L../../../${OUTPUT_OPENSOURCE_DIR}/boost/static/ -L../../../${OUTPUT_OPENSOURCE_DIR}/sqlite3/static -L../../../${OUTPUT_OPENSOURCE_DIR}/jsoncpp/static
ifeq ($(TARGET_OS),android)
	override LDFLAGS += -L../../../../../../${OUTPUT_OC_UTILITY_DIR}/android/boost/libs/armeabi
	override LDFLAGS += -L../../../../../../${OUTPUT_OC_DIR}/csdk/android/release
else 
	override LDFLAGS += -L${BOOST_PATH}
	override LDFLAGS += -L../../../../../../${OUTPUT_OC_DIR}/csdk/linux/release
endif

override LDFLAGS += -L../../../${OUTPUT_OPENSOURCE_DIR}/sqlite3/static -L../../../${OUTPUT_OPENSOURCE_DIR}/jsoncpp/static
override LDFLAGS += -L../../../../../../${OUTPUT_OC_DIR}/${CONFIG}/obj -L../../../../../../${OUTPUT_OC_DIR}/csdk/linux/${CONFIG} -L../../../../../../${OUTPUT_OC_DIR}/oc_logger/lib
override LDLIBS += -l${BOOST_SYSTEM_LIB} -l${BOOST_THREAD_LIB} -l${BOOST_REGEX_LIB} -lsqlite3 -ldl -ljsoncpp
override LDLIBS += -l${OC_LIB} -l${OCTBSTACK_LIB} -l${OCLOGGER_LIB}

include platformconfigs/$(TARGET_OS)_config.mk

override LDLIBS += ${RTLIB} ${PTLIB}

#configure target names
STATIC_LIB_TARGET	=	$(OUTPUT_DIR)/static/${STATIC_LIB_PREFIX}${PROJECT_NAME}.${STATIC_LIB_EXTENSION}
SHARED_LIB_TARGET	=	$(OUTPUT_DIR)/shared/${SHARED_LIB_PREFIX}${PROJECT_NAME}.${SHARED_LIB_EXTENSION}
SHARED_IMPORT_LIB_TARGET = $(OUTPUT_DIR)/shared/${SHARED_LIB_PREFIX}${PROJECT_NAME}.${SHARED_LIB_EXTENSION}.a

include ../../BuildTargets.mk
