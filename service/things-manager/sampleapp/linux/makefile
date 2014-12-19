MAKE=make

DIRLIST=configuration groupaction  groupsyncaction

# Force metatargets to build:
.PHONY: all clean

all: build

build:
	@for subdir in ${DIRLIST} ; do \
	${MAKE} -C $${subdir} ; \
	echo " " ; \
	done
	@echo " "

clean:
	@for subdir in ${DIRLIST} ; do \
	${MAKE} clean -C $${subdir} ; \
	echo " " ; \
	done
