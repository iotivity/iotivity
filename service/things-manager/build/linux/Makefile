.PHONY : pre resource sdk sampleapp
#.PHONY : lib tgm sdk sampleapp 
all: .PHONY

pre:
	-mkdir release

resource:
	cd ../../../../resource && $(MAKE)


sdk:
	cd ../../sdk/build/linux && $(MAKE)
	cp -Rdp ../../sdk/build/linux/*.a release/


sampleapp:
#	cd ../../sampleapp/linux/tgmclient && $(MAKE)
#	cp -Rdp ../../sampleapp/linux/tgmclient/tgmclient release/

clean:
	cd ../../sdk/build/linux && $(MAKE) clean
	cd ../../sampleapp/linux/tgmclient && $(MAKE) clean

	rm -rf ./release/*
