OCPATH = ../../../../../oic-resource/build/linux
NOTIFICATIONMGRPATH = ../../../NotificationManager/build/linux

.PHONY : oic-resource NotificationManager

all: .PHONY

oic-resource:
	cd $(OCPATH) && $(MAKE)

NotificationManager:
	cd $(NOTIFICATIONMGRPATH) && $(MAKE) && cp ./NotificationManager ../../../build/linux/release/ && cp ./SampleConsumer ../../../build/linux/release/ && cp ./SampleProvider ../../../build/linux/release/

clean:
	cd ../release && rm *
	cd $(OCPATH) && $(MAKE) clean
	cd $(NOTIFICATIONMGRPATH) && $(MAKE) clean
