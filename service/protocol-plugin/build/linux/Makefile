PROTOCOL_ROOT = ../../

.PHONY:pre oic-resource plugin-manager plugins sample-app

all: .PHONY

pre:
	-mkdir release

oic-resource:
	cd $(PROTOCOL_ROOT)../../oic-resource/build/linux && $(MAKE)

plugin-manager:
	cd $(PROTOCOL_ROOT)plugin-manager/build/linux && $(MAKE)
	cp -Rdp $(PROTOCOL_ROOT)plugin-manager/build/linux/ppm release/

plugins:
	cd $(PROTOCOL_ROOT)plugins/mqtt/build/linux && $(MAKE)
	cd $(PROTOCOL_ROOT)plugins/hue/build/linux && $(MAKE)

sample-app:
	cd $(PROTOCOL_ROOT)sample-app/linux/fan-control && $(MAKE)
	cp -Rdp $(PROTOCOL_ROOT)sample-app/linux/fan-control/fanclient release/
	

clean:
	cd $(PROTOCOL_ROOT)plugin-manager/build/linux && $(MAKE) clean
	cd $(PROTOCOL_ROOT)plugins/mqtt/build/linux && $(MAKE) clean
	cd $(PROTOCOL_ROOT)plugins/hue/build/linux && $(MAKE) clean
	cd $(PROTOCOL_ROOT)sample-app/linux/fan-control && $(MAKE) clean
	rm -rf release
