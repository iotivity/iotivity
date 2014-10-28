# root path of each PC.
ROOT_DIR=${PWD}/../../../..

# boost folder path.
BOOST_BASE=/home/iotivity/Desktop/boost_1_56_0

# ioc-resource folder path.
IOT_BASE=${ROOT_DIR}/resource
IOT_MAKE_PATH=${IOT_BASE}
IOT_RELEASE=${IOT_MAKE_PATH}/build/linux/release/obj
IOT_CSDK_RELEASE=${IOT_BASE}/csdk/build/linux/release
IOT_LIB=liboc.a
IOT_CSDK_LIB=liboctbstack.a

# service folder path.
FD_SSM=${ROOT_DIR}/service/soft-sensor-manager
FD_SDK=${FD_SSM}/SDK
FD_SSMCORE=${FD_SSM}/SSMCore
FD_SAMPLEAPP=${FD_SSM}/SampleApp
FD_SOFTSENSOR=${FD_SSM}/SoftSensorPlugin
FD_BUILD=${FD_SSM}/build

# outputs directory path.
OUTPUTS_DIR=${FD_BUILD}/linux/release
EXEC_DIR=${OUTPUTS_DIR}
MODEL_DIR="\"${EXEC_DIR}/\""
HIGH_LOCATION="\"${EXEC_DIR}/HighContextDictionary.xml\""

# SoftSensorManager makefile path
MAKE_01=${FD_SDK}/build/linux
MAKE_02=${FD_SSMCORE}/build/linux
MAKE_03=${FD_SAMPLEAPP}/linux
MAKE_04=${FD_SAMPLEAPP}/arduino
MAKE_05=${FD_SOFTSENSOR}/DiscomfortIndexSensor/build/linux

MAKE_LIST=${MAKE_01} ${MAKE_02} ${MAKE_05} ${MAKE_03}

