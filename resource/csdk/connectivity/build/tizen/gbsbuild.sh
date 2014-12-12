#!/bin/sh

spec=`ls tizen/packaging/*.spec`
version=`rpm --query --queryformat '%{version}\n' --specfile $spec`

name=`echo $name|cut -d" " -f 1`
version=`echo $version|cut -d" " -f 1`

name=cmsample

sudo rm -rf $name-$version

builddir=`pwd`
sourcedir=`pwd`


cd $sourcedir/tizen
mkdir ./tmp
mkdir -p ./tmp/dtls

cp -R ./* $sourcedir/tizen/tmp/
rm -rf $sourcedir/tizen/tmp/tmp/
cp -R ../../inc/* $sourcedir/tizen/tmp/
cp -R ../../lib/extlibs/tinydtls/* $sourcedir/tizen/tmp/dtls/
cp -R ../../api/* $sourcedir/tizen/tmp/
cp ../../src/adapter_util/* $sourcedir/tizen/tmp/
# Including All CA Adapter code for Tizen Compilation
cp -R ../../common/src/* $sourcedir/tizen/tmp/
cp -R ../../common/inc/* $sourcedir/tizen/tmp/
cp ../../src/* $sourcedir/tizen/tmp/
cp ../../lib/libcoap-4.1.1/*.h $sourcedir/tizen/tmp/
cp -R ../../lib/extlibs/tinydtls/*.h $sourcedir/tizen/tmp/tinydtls/
cp ../../lib $sourcedir/tizen/tmp/ -rf
adapterMacro=""

if echo $1 | grep -q -i "ALL"
then
echo "Building All the Transports"
cp ../../src/wifi_adapter/tizen/* $sourcedir/tizen/tmp/
cp ../../src/wifi_adapter/*.c $sourcedir/tizen/tmp/
cp ../../src/bt_edr_adapter/* $sourcedir/tizen/tmp/
cp ../../src/bt_edr_adapter/tizen/* $sourcedir/tizen/tmp/
cp ../../src/bt_le_adapter/* $sourcedir/tizen/tmp/
cp ../../src/bt_le_adapter/tizen/* $sourcedir/tizen/tmp/
adapterMacro=" -DETHERNET_ADAPTER -DWIFI_ADAPTER -DEDR_ADAPTER -DLE_ADAPTER"
else
if echo $1 | grep -q -i "WIFI"
then
echo "Copying WIFI Adapter Source Codes"
cp ../../src/wifi_adapter/*.c $sourcedir/tizen/tmp/
cp ../../src/wifi_adapter/tizen/* $sourcedir/tizen/tmp/
adapterMacro="$adapterMacro -DWIFI_ADAPTER"
fi

if echo $1 | grep -q -i "BT"
then
echo "Copying BT Adapter Source Codes"
cp ../../src/bt_edr_adapter/* $sourcedir/tizen/tmp/
cp ../../src/bt_edr_adapter/tizen/* $sourcedir/tizen/tmp/
adapterMacro="$adapterMacro -DEDR_ADAPTER"
fi

if echo $1 | grep -q -i "BLE"
then
echo "Copying BLE Adapter Source Codes"
cp ../../src/bt_le_adapter/* $sourcedir/tizen/tmp/
cp ../../src/bt_le_adapter/tizen/* $sourcedir/tizen/tmp/
adapterMacro="$adapterMacro -DLE_ADAPTER"
fi
fi

sed "/ADAPTER_MACRO :=/ c ADAPTER_MACRO :=$adapterMacro" Makefile > Makefile_bkp
cp Makefile_bkp $sourcedir/tizen/tmp/Makefile
rm Makefile_bkp
cd $sourcedir/tizen/tmp/

#removing the files which needs for only arduino to avoid
#multiple definition error.
rm -rf *_singlethread.*

rm -rf *dtls.*

#cd $builddir
#cp -R ./* $sourcedir/tmp/

#cd $sourcedir
#mv ./tmp $builddir/

#cp -R ./* $builddir/tmp

#cd $builddir
#mv tmp $name-$version

#build
#cd $name-$version
git init
gbs build -A armv7l --include-all

rm -rf $sourcedir/tizen/tmp
#cd $builddir
#sudo rm -rf $name-$version
