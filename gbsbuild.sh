#!/bin/sh

spec=`ls tools/tizen/*.spec`
version=`rpm --query --queryformat '%{version}\n' --specfile $spec`

name=`echo $name|cut -d" " -f 1`
version=`echo $version|cut -d" " -f 1`

name=iotivity

rm -rf $name-$version

builddir=`pwd`
sourcedir=`pwd`

echo `pwd`

# Clean tmp directory.
rm -rf ./tmp

# Create directory structure for GBS Build
mkdir ./tmp
mkdir ./tmp/extlibs/
mkdir ./tmp/packaging
cp -R ./build_common $sourcedir/tmp
cp -R ./examples $sourcedir/tmp

# tinycbor is available as soft-link, so copying with 'dereference' option.
cp -LR ./extlibs/tinycbor $sourcedir/tmp/extlibs
rm -rf $sourcedir/tmp/extlibs/tinycbor/tinycbor/.git

cp -R ./extlibs/cjson $sourcedir/tmp/extlibs
cp -R ./extlibs/mbedtls $sourcedir/tmp/extlibs
cp -R ./extlibs/gtest $sourcedir/tmp/extlibs
cp -R ./extlibs/tinydtls $sourcedir/tmp/extlibs
cp -LR ./extlibs/sqlite3 $sourcedir/tmp/extlibs
cp -R ./extlibs/timer $sourcedir/tmp/extlibs
cp -R ./extlibs/rapidxml $sourcedir/tmp/extlibs
cp -R ./extlibs/libcoap $sourcedir/tmp/extlibs
cp -R ./resource $sourcedir/tmp
cp -R ./service $sourcedir/tmp
cp ./extra_options.scons $sourcedir/tmp
cp ./tools/tizen/*.spec ./tmp/packaging
cp ./tools/tizen/*.manifest ./tmp/packaging
cp ./SConstruct ./tmp
cp ./LICENSE.md ./tmp

# copy dependency RPMs and conf files for tizen build
cp ./tools/tizen/*.rpm ./tmp
cp ./tools/tizen/.gbs.conf ./tmp
cp ./tools/tizen/*.rpm $sourcedir/tmp/service/easy-setup/sampleapp/enrollee/tizen-sdb/EnrolleeSample
cp ./tools/tizen/.gbs.conf ./tmp/service/easy-setup/sampleapp/enrollee/tizen-sdb/EnrolleeSample

cp -R $sourcedir/iotivity.pc.in $sourcedir/tmp

cd $sourcedir/tmp

withtcp=0
withcloud=0
withproxy=0
withmq=OFF
secured=0
for ARGUMENT_VALUE in $*
do
   echo $ARGUMENT_VALUE
   if [ "WITH_TCP" = $ARGUMENT_VALUE ];then
       withtcp=1
   fi

   if [ "WITH_CLOUD" = $ARGUMENT_VALUE ];then
       withcloud=1
   fi

   if [ "WITH_PROXY" = $ARGUMENT_VALUE ];then
       withproxy=1
   fi

   if [ "WITH_MQ" = $ARGUMENT_VALUE ];then
       withmq=PUB,SUB,BROKER
   fi

   if [ "SECURED" = $ARGUMENT_VALUE ];then
       secured=1
   fi
done

if [ $secured -eq 1 ];then
  echo `pwd`
  if [ -d ./extlibs/mbedtls/mbedtls ];then
      cd ./extlibs/mbedtls/mbedtls
      git reset --hard ad249f509fd62a3bbea7ccd1fef605dbd482a7bd ; git apply ../ocf.patch
      cd -
      rm -rf ./extlibs/mbedtls/mbedtls/.git*

  else
      echo ""
      echo "*********************************** Error: ****************************************"
      echo "* Please download mbedtls using the following command:                            *"
      echo "*     $ git clone https://github.com/ARMmbed/mbedtls.git extlibs/mbedtls/mbedtls  *"
      echo "***********************************************************************************"
      echo ""
      exit
  fi
fi

rm -rf ./extlibs/tinycbor/tinycbor/.git*

# Initialize Git repositoryㅣ
if [ ! -d .git ]; then
   git init ./
   git config user.email "you@example.com"
   git config user.name "Your Name"
   git add ./
   git commit -m "Initial commit"
fi

echo "Calling core gbs build command"
gbscommand="gbs build -A armv7l --define 'WITH_TCP $withtcp' --define 'WITH_CLOUD $withcloud' --define 'WITH_PROXY $withproxy' --define 'WITH_MQ $withmq' --define 'SECURED $secured' -B ~/GBS-ROOT-OIC --include-all --repository ./"
echo $gbscommand
if eval $gbscommand; then
    echo "Build is successful"
else
    echo "Build failed!"
    exit 1
fi


rm -rf tmp
cd $sourcedir
rm -rf $sourcedir/tmp

exit 0
