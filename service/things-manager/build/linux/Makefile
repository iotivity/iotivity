.PHONY : pre resource lib tgm sdk sampleapp

#.PHONY : lib tgm sdk sampleapp 
all: .PHONY

pre:
	-mkdir release

resource:
	cd ../../../../resource/build/linux && $(MAKE)

lib:
	cd ../../lib/jsoncpp && $(MAKE)
	cp -Rdp ../../lib/jsoncpp/libjsoncpp.a release/
	cd ../../lib/sqlite && $(MAKE)
	cp -Rdp ../../lib/sqlite/libsqlite.a release/

sdk:
	cd ../../sdk/build/linux && $(MAKE)
	cp -Rdp ../../sdk/build/linux/*.a release/

tgm:
	cd ../../tgm/build/linux && $(MAKE)
	cp -Rdp ../../tgm/build/linux/*.a release/
	cp -Rdp ../../tgm/build/linux/tgmserver release/

sampleapp:
	cd ../../sampleapp/linux/bookmark && $(MAKE)
	cp -Rdp ../../sampleapp/linux/bookmark/bookmark release/
	cd ../../sampleapp/linux/switch && $(MAKE)
	cp -Rdp ../../sampleapp/linux/switch/controller release/
	cd ../../sampleapp/linux/light && $(MAKE)
	cp -Rdp ../../sampleapp/linux/light/light release/
	cd ../../sampleapp/linux/musicapp && $(MAKE)
	cp -Rdp ../../sampleapp/linux/musicapp/musicplayer release/

clean:
	cd ../../sdk/build/linux && $(MAKE) clean
	cd ../../tgm/build/linux && $(MAKE) clean
	cd ../../sampleapp/linux/bookmark && $(MAKE) clean
	cd ../../sampleapp/linux/switch && $(MAKE) clean
	cd ../../sampleapp/linux/light && $(MAKE) clean
	cd ../../sampleapp/linux/musicapp && $(MAKE) clean
	rm -rf ./release/*
	cd ../arduino && $(MAKE) clean
