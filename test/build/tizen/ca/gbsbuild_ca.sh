#----------------------------------------------------------------------
# Main Script For Building CA for Tizen
#----------------------------------------------------------------------
echo "Creating Temporary Directory CA"
echo "Please wait. It will take a few seconds..."
echo " "

cd build/
homeDir='../..'

tmp_dirName='tmpCA'
tmp_dir=$homeDir'/'$tmp_dirName
ext_lib_dir='extlibs'
test_dir='test'
rpm_name="com-oic-ca-sim-0.0.1-1.armv7l.rpm"
target_dir=$test_dir'/bin/tizen'
build_script_dir='tizen/ca'

target_arch=$1
target_tranport=$2
secured=$3
rpm_path=$4

mkdir $tmp_dir
mkdir $tmp_dir/packaging
mkdir $tmp_dir/$test_dir
mkdir -p $tmp_dir/iotivity/resource/csdk/connectivity
mkdir $tmp_dir/iotivity
mkdir -p $tmp_dir/iotivity/resource

cp -R $homeDir/resource/* $tmp_dir/iotivity/resource/
cp -R $homeDir/$test_dir/* $tmp_dir/$test_dir
cp $build_script_dir/com.oic.ca.sim.spec $tmp_dir/packaging
cp $build_script_dir/com.oic.ca.sim.manifest $tmp_dir
cp $build_script_dir/com.oic.ca.sim.xml $tmp_dir
cp $build_script_dir/SConstruct $tmp_dir

echo "Initializing Git repository"
echo " "

cd $tmp_dir

echo `pwd`

if [ ! -d .git ]; then
   git init ./
   git config user.email "you@example.com"
   git config user.name "Your Name"
   git add ./
   git commit -m "Initial commit"
fi

echo "Calling core gbs build command"

gbscommand="gbs build -A "$target_arch" -B ~/GBS-ROOT-RI-OIC --include-all --define 'TARGET_TRANSPORT $target_tranport' --define 'SECURED $secured' "
echo $gbscommand

if eval $gbscommand; then
   echo "GBS build is successful"

   echo "Extracting RPM"
   extractCommand="rpm2cpio "$rpm_path/$rpm_name" | cpio -idmv"
   echo $extractCommand

   chmod 777 $rpm_path/$rpm_name
   sleep 1
   eval $extractCommand
   echo $(pwd)
   echo "Copying binary files"
   mkdir -p ../$target_dir
   cp usr/apps/com.oic.ca.sim/bin/* ../$target_dir

    echo "Copying RPM"
    cp $rpm_path/$rpm_name ../$target_dir

   chmod 777 -R ../$target_dir
else
   echo "GBS build failed."
fi
echo ""
echo "Deleting Tmp Directory"

cd ..
rm -rf $tmp_dirName/
