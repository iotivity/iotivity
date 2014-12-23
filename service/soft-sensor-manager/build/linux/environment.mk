# boost folder path.
BOOST_BASE=/usr/include

# ioc-resource folder path.
IOT_BASE=${ROOT_DIR}/resource
IOT_MAKE_PATH=${IOT_BASE}
IOT_RELEASE=${IOT_MAKE_PATH}/release/obj
IOT_CSDK_RELEASE=${IOT_BASE}/csdk/linux/release
IOT_LOG_DIR=${IOT_BASE}/oc_logger
IOT_LIB=liboc.a
IOT_CSDK_LIB=liboctbstack.a
IOT_LOG_LIB=oc_logger.a

# service folder path.
FD_SSM=${ROOT_DIR}/service/soft-sensor-manager
FD_SDK=${FD_SSM}/SDK/cpp
FD_SSMCORE=${FD_SSM}/SSMCore
FD_SAMPLEAPP=${FD_SSM}/SampleApp
FD_SOFTSENSOR=${FD_SSM}/SoftSensorPlugin
FD_BUILD=${FD_SSM}/build

# outputs directory path.
OUTPUTS_DIR=${FD_BUILD}/linux/release
EXEC_DIR=${OUTPUTS_DIR}
SSXML_NAME="SoftSensorDescription.xml"
SSM_LIB = libSSMSDK.a
SSMCORE_LIB=libSSMCore.a

# SoftSensorManager makefile path
MAKE_01=${FD_SSMCORE}/build/linux
MAKE_02=${FD_SDK}/build/linux
MAKE_03=${FD_SAMPLEAPP}/linux
MAKE_04=${FD_SAMPLEAPP}/arduino
MAKE_05=${FD_SOFTSENSOR}/DiscomfortIndexSensor/build/linux
MAKE_06=${FD_SOFTSENSOR}/IndoorTrajectorySensor/build/linux


MAKE_LIST=${MAKE_01} ${MAKE_02} ${MAKE_03} ${MAKE_05} ${MAKE_06}