IOTIVITY_DIR=${shell cd ../../../../ && pwd}
${shell echo "ROOT_DIR=${IOTIVITY_DIR}" > ./root_path.inc}

-include ./root_path.inc
-include ./environment.mk

MAKE=make
RST_NAME=release
CUR_DIR=${PWD}


# Force metatargets to build:
.PHONY: all clean

all: pre_job build post_job 

pre_job:
	@echo " " 
	@echo "*********************************************************" 
	@echo "*           Samsung Electronics. Open Source.           *"
	@echo "*                                                       *"
	@echo "* It's build script for SoftSensorManagement system.    *"
	@echo "* Before build, Open the environment.mk file.           *"
	@echo "* And, Modify ROOT_DIR path, which depend on your PC.   *"
	@echo "* And, Modify BOOST_BASE path, which depend on your PC. *"
	@echo "*                                                       *"
	@echo "*********************************************************" 
	@mkdir -p ${RST_NAME}
	@echo " "

	
build:
	@for subdir in ${MAKE_LIST} ; do \
	echo ">>>>>>>>>>>> $${subdir}/Makefile run <<<<<<<<<<<<<" ; \
	cd $${subdir} && ${MAKE} ${OPTION} ; \
	echo " " ; \
	done
	@echo " "

post_job:
	@echo " " 
	@echo "*********************************************************" 
	@echo "*                                                       *"
	@echo "*               Success. build script.                  *"
	@echo "*                    - End -                            *"
	@echo "*********************************************************" 
	@echo "Enter to ${RST_NAME} folder."
	@echo " " 

clean:

	rm -rf ./${RST_NAME}
	@for subdir in ${MAKE_LIST} ; do \
	echo ">>>>>>>>>>>> $${subdir}/Makefile clean run <<<<<<<<<<<<<<" ; \
	${MAKE} clean -C $${subdir} ; \
	echo " " ; \
	done
