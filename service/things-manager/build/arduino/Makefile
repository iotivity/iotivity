.PHONY : resource SampleApp

all: .PHONY

resource: clean
	cd ../../../../resource/csdk/build/arduino && $(MAKE) PLATFORM=arduinomega ARDUINOWIFI=0 BUILD=release

SampleApp:
	cd ../../sampleapp/arduino/bookmark && $(MAKE) BUILD=release
	cd ../../sampleapp/arduino/light && $(MAKE) BUILD=release

clean:
	cd ../../../../resource/build/linux && make clean
	cd ../../../../resource/csdk/build/arduino && make deepclean 
	cd ../../sampleapp/arduino/bookmark && $(MAKE) clean
	cd ../../sampleapp/arduino/light && $(MAKE) clean

