
-include ../../build/linux/environment.mk

MAKE=make

CUR_DIR=${PWD}
SRCLIST=${wildcard **/**/Makefile}
DIRLIST=${patsubst %/, % , ${dir ${SRCLIST}} }


# Force metatargets to build:
.PHONY: all clean

all: pre_job build post_job 

pre_job:
	@echo " " 
	@echo "============= Sample Folder ============" 
	@echo " "
	@echo "${DIRLIST}"
	
build:
	@for subdir in ${DIRLIST} ; do \
	${MAKE} -C $${subdir} ; \
	echo " " ; \
	done
	@echo " "

post_job:
	@echo " " 
	@echo "============ Sample Folder Successful. ============="
	@echo " " 


clean:
	@for subdir in ${DIRLIST} ; do \
	${MAKE} clean -C $${subdir} ; \
	echo " " ; \
	done
