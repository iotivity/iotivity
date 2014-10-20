BUILD := release
ARDUINO_PORT := /dev/ttyACM0
APP_INSTALL := ocserver

ocserver:
	$(MAKE) -c ./$@ "BUILD=$(BUILD)"

occlient:
	$(MAKE) -c ./$@ "BUILD=$(BUILD)"

install:
	$(MAKE) -C ./$(APP_INSTALL) "install" "ARDUINO_PORT=$(ARDUINO_PORT)" "BUILD=$(BUILD)"

clean:
	$(MAKE) -C ./occlient "clean"
	$(MAKE) -C ./ocserver "clean"
