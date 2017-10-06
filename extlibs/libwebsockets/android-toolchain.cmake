#
# CMake Toolchain file for crosscompiling on ARM.
#

# Android NDK Toolchain path
set(CROSS_PATH $ENV{ANDROID_NDK_TOOLCHAIN_PATH})

# OpenSSL library details.
set(OPENSSL_INCLUDE_DIR "$ENV{OPENSSL_SOURCE}/include")
set(OPENSSL_CRYPTO_LIBRARY "$ENV{OPENSSL_SOURCE}/libcrypto.so")
set(OPENSSL_SSL_LIBRARY "$ENV{OPENSSL_SOURCE}/libssl.so")

# Target architecture and operating system details.
set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_ANDROID_API 21)

if ("$ENV{TARGET_ARM_ARCH}" STREQUAL "x86")
	set(NDK_ARCH "arch-x86")
	set(BUILD_TOOL "i686-linux-android")
elseif ("$ENV{TARGET_ARM_ARCH}" STREQUAL "x86_64")
	set(NDK_ARCH "arch-x86_64")
	set(BUILD_TOOL "x86_64-linux-android")
elseif ("$ENV{TARGET_ARM_ARCH}" STREQUAL "arm64-v8a")
	set(NDK_ARCH "arch-arm64")
	set(BUILD_TOOL "aarch64-linux-android")
else ()
	# For 'armeabi', 'armeabi-v7a', 'armeabi-v7a-hard' and other ARM architectures.
	set(NDK_ARCH "arch-arm")
	set(BUILD_TOOL "arm-linux-androideabi")
endif ()

set(CMAKE_SYSROOT $ENV{ANDROID_NDK_PATH}/platforms/android-21/${NDK_ARCH})

# Compiler and build tools.
set(CMAKE_C_COMPILER "${CROSS_PATH}/bin/${BUILD_TOOL}-gcc-$ENV{ANDROID_NDK_TOOLCHAIN_VERSION}")
set(CMAKE_CXX_COMPILER "${CROSS_PATH}/bin/${BUILD_TOOL}-g++")
set(CMAKE_RANLIB "${CROSS_PATH}/bin/${BUILD_TOOL}-ranlib")
set(CMAKE_LINKER "${CROSS_PATH}/bin/${BUILD_TOOL}-ld")
set(CMAKE_AR "${CROSS_PATH}/bin/${BUILD_TOOL}-ar")
set(CMAKE_MN "${CROSS_PATH}/bin/${BUILD_TOOL}-mn")
set(CMAKE_STRIP "${CROSS_PATH}/bin/${BUILD_TOOL}-strip")

# Path where to look for the target environment.
set(CMAKE_FIND_ROOT_PATH "${CROSS_PATH}")

# Print the path of C compiler.
message("CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")

# Adjust the default behavior of the FIND_XXX() commands:
# search programs in the host environment only.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search headers and libraries in the target environment only.
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)