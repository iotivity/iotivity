#----------------------------------------------------------------------
# Main Script For Building RC for Tizen
#----------------------------------------------------------------------

echo "Creating Temporary Directory RE"
echo "Please wait. It will take a few seconds..."
echo " "

cd build/
homeDir='../..'

tmpDirName='tmpRC'
tmpDir=$homeDir'/'$tmpDirName
orgSource='IotivityOrgSource'
orgTarget='target'
secTestDir='test'
extLibDir='extlibs'
target_arch=$1
rpmPath=$2
target=$4
rpmName="org.tizen.RC_UTC_ITC-0.0.1-1.armv7l.rpm"
targetDir=$secTestDir'/bin/tizen'
buildScriptDir='tizen/rc'
isFailed=false

mkdir $tmpDir
mkdir $tmpDir/packaging
mkdir -p $tmpDir/iotivity/out/tizen
mkdir -p $tmpDir/iotivity/service
mkdir -p $tmpDir/iotivity/resource
mkdir -p $tmpDir/iotivity/extlibs/timer
mkdir -p $tmpDir/iotivity/extlibs/cjson
mkdir -p $tmpDir/iotivity/extlibs/cereal
mkdir -p $tmpDir/iotivity/extlibs/rapidxml
mkdir $tmpDir/$secTestDir
mkdir -p $tmpDir/$extLibDir/gtest-1.7.0
mkdir -p $homeDir/$targetDir
echo `pwd`

cp $buildScriptDir/RC_UTC_ITC.spec $tmpDir/packaging
cp $buildScriptDir/SConstruct $tmpDir
cp $buildScriptDir/../scons-2.1.0-3.1.slp.noarch.rpm $tmpDir
cp -R $homeDir/out/tizen/* $tmpDir/iotivity/out/tizen
cp -R $homeDir/service/* $tmpDir/iotivity/service
cp -R $homeDir/resource/* $tmpDir/iotivity/resource
cp -R $homeDir/extlibs/timer/* $tmpDir/iotivity/extlibs/timer
cp -R $homeDir/extlibs/cjson/* $tmpDir/iotivity/extlibs/cjson
#cp -R $homeDir/iotivity/extlibs/cereal/* $tmpDir/iotivity/extlibs/cereal
cp -R $homeDir/extlibs/rapidxml/* $tmpDir/iotivity/extlibs/rapidxml
cp -R $homeDir/$secTestDir/* $tmpDir/$secTestDir
cp -R $homeDir/$secTestDir/$extLibDir/gtest-1.7.0/* $tmpDir/$extLibDir/gtest-1.7.0

echo "Initializing Git repository"
echo " "

cd $tmpDir

echo `pwd`

if [ ! -d .git ]; then
   git init ./
   git config user.email "you@example.com"
   git config user.name "Your Name"
   git add ./
   git commit -m "Initial commit"
fi

echo "Calling core gbs build command"
#gbscommand="gbs build -A "$target_arch" -B ~/GBS-ROOT-OIC --include-all --keep-packs --skip-conf-repos -R http://download.tizen.org/releases/2.3/2.3-mobile/tizen-2.3-mobile_20150206.1/repos/target/packages/ -R ./"
gbscommand="gbs build -A "$target_arch" -B ~/GBS-ROOT-OIC --include-all --repository ./"
echo $gbscommand

if eval $gbscommand; then
   echo "GBS build is successful"

   sudo chmod 777 $rpmPath/$rpmName
   sleep 1
   
   echo "Copying binary files"
   cp $rpmPath/$rpmName ../$targetDir
   
   sudo chmod 777 -R ../$targetDir
else
   echo "GBS build failed."
   isFailed=true
fi

echo ""
echo "Deleting Tmp Directory"


cd ..
rm -rf $tmpDirName/

if $isFailed; then
    echo "Exiting with ERROR Code 1."
    exit 1
fi
