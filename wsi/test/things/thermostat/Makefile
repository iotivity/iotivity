IOTIVITY_DIR=${HOME}/source/active/iotivity
all:
	g++ -g thermostat.cpp `pkg-config --cflags --libs elementary` -std=c++11 -pthread -Wall -fpermissive \
		-I$(IOTIVITY_DIR)/resource/include \
		-I$(IOTIVITY_DIR)/resource/csdk/stack/include \
		-I$(IOTIVITY_DIR)/resource/csdk/ocrandom/include \
		-I$(IOTIVITY_DIR)/resource/csdk/logger/include \
		-I$(IOTIVITY_DIR)/resource/c_common \
		-I$(IOTIVITY_DIR)/resource/oc_logger/include \
		-I$(IOTIVITY_DIR)/wsi/bin/inc/ \
		-I$(IOTIVITY_DIR)/intf/inc \
		-L$(IOTIVITY_DIR)/out/linux/x86_64/release/ \
		-loc -loctbstack -loc_logger -lconnectivity_abstraction -lcoap -lpthread -o thermostat

clean:
	rm *.o bulb
