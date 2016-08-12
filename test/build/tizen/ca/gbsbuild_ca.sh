#----------------------------------------------------------------------
# Main Script For Building CA for Tizen
#----------------------------------------------------------------------

echo "Creating Temporary Directory CA"
echo "Please wait. It will take a few seconds..."
echo " "

cd build/
homeDir='../..'

tmpDirName='tmpCA'
tmpDir=$homeDir'/'$tmpDirName
orgSource='IotivityOrgSource'
devSourceDir='IotivitySECDevSource'
caSourceDir='CA'
orgTarget='target'
secTestDir='test'
extLibDir='extlibs'
target_arch=$1
rpmPath=$4
target_source=$6
rpmName="com-oic-ca-sim-0.0.1-1.armv7l.rpm"
targetDir=$secTestDir'/bin/tizen'
buildScriptDir='tizen/ca'

target_tranport=$2
target_secured=$3

mkdir $tmpDir
mkdir $tmpDir/packaging
mkdir $tmpDir/$secTestDir


mkdir -p $tmpDir/iotivity/resource/csdk/connectivity

pwd
echo $tmpDir
echo $homeDir



mkdir $tmpDir/iotivity
mkdir -p $tmpDir/iotivity/resource

cp -R $homeDir/resource/* $tmpDir/iotivity/resource/

cp -R $homeDir/$secTestDir/* $tmpDir/$secTestDir

cp $buildScriptDir/com.oic.ca.sim.spec $tmpDir/packaging
cp $buildScriptDir/com.oic.ca.sim.manifest $tmpDir
cp $buildScriptDir/com.oic.ca.sim.xml $tmpDir
#cp $buildScriptDir/SConscript $tmpDir
cp $buildScriptDir/SConstruct $tmpDir

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

gbscommand="gbs build -A "$target_arch" -B ~/GBS-ROOT-OIC --include-all --define 'TARGET_TRANSPORT $2' --define 'SECURED $3'"
echo $gbscommand

if eval $gbscommand; then
   echo "GBS build is successful"
   
   echo "Extracting RPM"
   extractCommand="rpm2cpio "$rpmPath/$rpmName" | cpio -idmv"
   echo $extractCommand

   chmod 777 $rpmPath/$rpmName
   sleep 1
   eval $extractCommand
   echo $(pwd)
   echo "Copying binary files"
   mkdir -p ../$targetDir
   cp usr/apps/com.oic.ca.sim/bin/* ../$targetDir

    echo "Copying RPM"
    cp $rpmPath/$rpmName ../$targetDir

   chmod 777 -R ../$targetDir
else
   echo "GBS build failed."
fi
echo ""
echo "Deleting Tmp Directory"

cd ..
rm -rf $tmpDirName/
