PROTOCOL_ROOT = ../../

.PHONY:pre plugin-manager plugins sample-app

all: .PHONY

pre:
	-mkdir release

resource:
	cd $(PROTOCOL_ROOT)../../resource && $(MAKE)

plugin-manager:
	cd $(PROTOCOL_ROOT)plugin-manager/build/linux && $(MAKE)
	cp -Rdp $(PROTOCOL_ROOT)plugin-manager/build/linux/libppm.a release/
	cp -Rdp $(PROTOCOL_ROOT)plugin-manager/build/linux/libpmimpl.so release/

plugins:
	cd $(PROTOCOL_ROOT)plugins/mqtt-fan/build/linux && $(MAKE)
	cd $(PROTOCOL_ROOT)plugins/mqtt-light/build/linux && $(MAKE)

sample-app:
	cd $(PROTOCOL_ROOT)sample-app/linux/mqtt && $(MAKE)
	cp -Rdp $(PROTOCOL_ROOT)plugin-manager/build/linux/libpmimpl.so $(PROTOCOL_ROOT)sample-app/linux/mqtt/
	cp -Rdp $(PROTOCOL_ROOT)sample-app/linux/mqtt/mqttclient release/
	cp -Rdp $(PROTOCOL_ROOT)sample-app/linux/mqtt/pluginmanager.xml release/

clean:
	cd $(PROTOCOL_ROOT)plugin-manager/build/linux && $(MAKE) clean
	cd $(PROTOCOL_ROOT)plugins/mqtt-fan/build/linux && $(MAKE) clean
	cd $(PROTOCOL_ROOT)plugins/mqtt-light/build/linux && $(MAKE) clean
	cd $(PROTOCOL_ROOT)sample-app/linux/mqtt && $(MAKE) clean
	rm -rf release
