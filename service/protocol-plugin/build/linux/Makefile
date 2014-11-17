PROTOCOL_ROOT = ../../

.PHONY:pre resource plugin-manager plugins sample-app

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
	cd $(PROTOCOL_ROOT)sample-app/linux/fan-control && $(MAKE)
	cp -Rdp $(PROTOCOL_ROOT)sample-app/linux/fan-control/fanclient release/
	

clean:
	cd $(PROTOCOL_ROOT)plugin-manager/build/linux && $(MAKE) clean
	cd $(PROTOCOL_ROOT)plugins/mqtt-fan/build/linux && $(MAKE) clean
	cd $(PROTOCOL_ROOT)plugins/mqtt-light/build/linux && $(MAKE) clean
	cd $(PROTOCOL_ROOT)plugins/hue/build/linux && $(MAKE) clean
	cd $(PROTOCOL_ROOT)sample-app/linux/fan-control && $(MAKE) clean
	rm -rf release
