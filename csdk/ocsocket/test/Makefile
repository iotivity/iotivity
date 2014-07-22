all: 
	$(MAKE) -C android all
	$(MAKE) -C linux all
	$(MAKE) -C arduino all

install: all
	$(MAKE) -C android install
	$(MAKE) -C arduino install

.PHONY: clean

clean:
	$(MAKE) -C android clean
	$(MAKE) -C linux clean
	$(MAKE) -C arduino clean

