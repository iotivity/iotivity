
# ioc-resource folder path.
IOT_BASE=${ROOT_DIR}/resource
IOT_MAKE_PATH=${IOT_BASE}
IOT_RELEASE=${IOT_BASE}/release/obj
IOT_CSDK_LINUX_RELEASE=${IOT_BASE}/csdk/linux/release
IOT_CSDK_ARDUINOMEGA_RELEASE=${IOT_BASE}/csdk/arduinomega/release

# oic-service folder path.
FD_SSM=${ROOT_DIR}/service/soft-sensor-manager
FD_SAMPLEAPP=${FD_SSM}/SampleApp
FD_BUILD=${FD_SSM}/build

# outputs directory path.
OUTPUTS_DIR=${FD_BUILD}/arduino/release

#OPTION=PLATFORM=arduinomega ARDUINOWIFI=1
# SoftSensorManager makefile path
MAKE_00=${FD_SAMPLEAPP}/arduino
MAKE_01=${FD_SAMPLEAPP}/arduino/THSensorApp/build
MAKE_03=${FD_SAMPLEAPP}/arduino/Trackee_Thing/build
MAKE_04=${FD_SAMPLEAPP}/arduino/Reference_Thing/build

MAKE_LIST=${MAKE_01} ${MAKE_03} ${MAKE_04}

