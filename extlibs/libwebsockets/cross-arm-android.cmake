#
# CMake Toolchain file for crosscompiling on ARM.
#
# This can be used when running cmake in the following way:
#  cd build/
#  cmake .. -DCMAKE_TOOLCHAIN_FILE=../cross-arm-android.cmake
#

# Android NDK path
set(CROSS_PATH $ENV{ANDROID_NDK_TOOLCHAIN_PATH})

# OpenSSL library path
set(OPENSSL_INCLUDE_DIR "$ENV{OPENSSL_SOURCE}/include")
set(OPENSSL_CRYPTO_LIBRARY "$ENV{OPENSSL_SOURCE}/libcrypto.so")
set(OPENSSL_SSL_LIBRARY "$ENV{OPENSSL_SOURCE}/libssl.so")

# Target operating system name.
set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_ANDROID_API 21)
set(CMAKE_SYSROOT $ENV{ANDROID_NDK_PATH}/platforms/android-21/arch-arm)

# Name of C compiler.
set(CMAKE_C_COMPILER "${CROSS_PATH}/bin/arm-linux-androideabi-gcc-$ENV{ANDROID_NDK_TOOLCHAIN_VERSION}")
message("complier path:${CMAKE_C_COMPILER}")
set(CMAKE_CXX_COMPILER "${CROSS_PATH}/bin/arm-linux-androideabi-g++")
set(CMAKE_RANLIB "${CROSS_PATH}/bin/arm-linux-androideabi-ranlib")
set(CMAKE_LINKER "${CROSS_PATH}/bin/arm-linux-androideabi-ld")
set(CMAKE_AR "${CROSS_PATH}/bin/arm-linux-androideabi-ar")
set(CMAKE_MN "${CROSS_PATH}/bin/arm-linux-androideabi-mn")
set(CMAKE_STRIP "${CROSS_PATH}/bin/arm-linux-androideabi-strip")

# Where to look for the target environment. (More paths can be added here)
set(CMAKE_FIND_ROOT_PATH "${CROSS_PATH}")

# Adjust the default behavior of the FIND_XXX() commands:
# search programs in the host environment only.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search headers and libraries in the target environment only.
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
