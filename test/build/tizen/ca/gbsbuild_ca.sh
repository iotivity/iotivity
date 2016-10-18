#----------------------------------------------------------------------
# Main Script For Building CA for Tizen
#----------------------------------------------------------------------
echo "Creating Temporary Directory CA"
echo "Please wait. It will take a few seconds..."
echo " "

cd build/
home_dir='../..'
tmp_dirName='tmpCA'
tmp_dir=$home_dir'/'$tmp_dirName
rpm_name="com-oic-ca-sim-1.2.0-4.armv7l.rpm"
target_dir=test'/bin/tizen'
build_script_dir='tizen/ca'

target_arch=$1
target_tranport=$2
secured=$3
rpm_path=$4

rm -rf $tmp_dir
mkdir $tmp_dir
mkdir $tmp_dir/packaging

mkdir -p $tmp_dir/extlibs/mbedtls/mbedtls/include
cp -rf $home_dir/extlibs/mbedtls/mbedtls/include/mbedtls/ $tmp_dir/extlibs/mbedtls/mbedtls/include

cp -rf $home_dir/resource/ $tmp_dir/

mkdir -p $tmp_dir/test/extlibs
cp -rf $home_dir/test/extlibs/gtest-1.7.0 $tmp_dir/test/extlibs/

mkdir -p $tmp_dir/test/res
cp -rf $home_dir/test/res/ca_resource $tmp_dir/test/res/

cp -rf $home_dir/test/include $tmp_dir/test

mkdir -p $tmp_dir/test/src/common/commonutil/
cp -rf $home_dir/test/src/common/commonutil/c_cpp $tmp_dir/test/src/common/commonutil/

mkdir -p $tmp_dir/test/src/common/testcase/
cp -rf $home_dir/test/src/common/testcase/gtest $tmp_dir/test/src/common/testcase

mkdir -p $tmp_dir/test/src/tc/ca
cp -rf $home_dir/test/src/tc/ca/gtest $tmp_dir/test/src/tc/ca/

mkdir -p $tmp_dir/test/src/testapp/ca/
cp -rf $home_dir/test/src/testapp/ca/c_cpp $tmp_dir/test/src/testapp/ca/

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
rm -rf $tmp_dirName
