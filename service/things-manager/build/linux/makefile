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
	cd ../../sampleapp/linux && $(MAKE)
	cp -Rdp ../../sampleapp/linux/configuration/con-server release/
	cp -Rdp ../../sampleapp/linux/configuration/con-client release/
	cp -Rdp ../../sampleapp/linux/configuration/bootstrapserver release/
	cp -Rdp ../../sampleapp/linux/groupaction/groupserver release/
	cp -Rdp ../../sampleapp/linux/groupaction/bookmark release/
	cp -Rdp ../../sampleapp/linux/groupaction/lightserver release/
	cp -Rdp ../../sampleapp/linux/groupsyncaction/group release/
	cp -Rdp ../../sampleapp/linux/groupsyncaction/musicplayer release/
	cp -Rdp ../../sampleapp/linux/groupsyncaction/phone release/
	cp -Rdp ../../sampleapp/linux/groupsyncaction/speaker release/

clean:
	cd ../../sdk/build/linux && $(MAKE) clean
	cd ../../sampleapp/linux && $(MAKE) clean
	rm -rf ./release
