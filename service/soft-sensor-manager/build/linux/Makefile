IOTIVITY_DIR=${shell cd ../../../../ && pwd}
${shell echo "ROOT_DIR=${IOTIVITY_DIR}" > ./root_path.inc}

-include ./root_path.inc
-include ./environment.mk

MAKE=make
RST_NAME=release

CUR_DIR=${PWD}
SRCLIST=${wildcard **/**/Makefile}
DIRLIST=${patsubst %/, % , ${dir ${SRCLIST}} }


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
	@echo "* And, Modify BOOST_BASE path, which depend on your PC. *"
	@echo "*                                                       *"
	@echo "*********************************************************" 
	@mkdir -p ${RST_NAME}
	@echo " "
#	@cd ${IOT_BASE} &&	${MAKE};
#	@cd ${CUR_DIR};
	
build:
	@for subdir in ${MAKE_LIST} ; do \
	echo ">>>>>>>>>>>> $${subdir}/Makefile run <<<<<<<<<<<<<" ; \
	${MAKE} -C $${subdir} ; \
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
	rm -rf ./root_path.inc
