# OpenSSL build script for android platform.

# Get into the openssl library.
cd openssl

# Build and install the library
chmod a+x config
./config shared no-ssl2 no-ssl3 no-comp no-hw no-engine --openssldir="$PWD" --prefix="$PWD"

make depend

make CALC_VERSIONS="SHLIB_COMPAT=; SHLIB_SOVER=" MAKE="make -e" all

# Commenting the installation part till we are clear about its importance
# sudo -E make install CC=$ANDROID_TOOLCHAIN/arm-linux-androideabi-gcc RANLIB=$ANDROID_TOOLCHAIN/arm-linux-androideabi-ranlib
