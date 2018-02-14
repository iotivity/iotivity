
echo "gbs build started"
echo ""

iotivity_root=${1}
iotivity_test_root=${2}
target_arch=${3}
gbs_root=${4}
rpm_path=${5}
init_mode=${6}

mkdir -p ${iotivity_test_root}/extlibs/gtest-1.7.0/bin/tizen

tmp_dir=${iotivity_test_root}/bin/tizen/tmp/gtest
rm -rf ${tmp_dir}

mkdir -p ${tmp_dir}
cwd_dir=`pwd`
echo ${cwd_dir}

cp -rf ${iotivity_test_root}/extlibs/gtest-1.7.0/src ${tmp_dir}/src
cp -rf ${iotivity_test_root}/extlibs/gtest-1.7.0/include ${tmp_dir}/include
cp -rf ${iotivity_test_root}/extlibs/gtest-1.7.0/packaging ${tmp_dir}/packaging
cp ${iotivity_test_root}/extlibs/gtest-1.7.0/Makefile ${tmp_dir}/Makefile
cp ${iotivity_test_root}/extlibs/gtest-1.7.0/SConscript ${tmp_dir}/SConscript
cp ${iotivity_root}/tools/tizen/.gbs.conf ${tmp_dir}

cd ${tmp_dir}

echo "Initializing Git repository"
echo " "

if [ ! -d .git ]; then
   git init ./
   git add ./
   git commit -m "Initial commit"
fi

echo "Calling core gbs build command"
gbscommand="gbs build -A ${target_arch} -B ${gbs_root} --repository ./ --include-all ${init_mode}"
echo ${gbscommand}

if eval ${gbscommand}; then
   echo "GBS build is successful"
else
   echo "GBS build failed."
   echo "deleting tmp resources"
   rm -rf .git/
   exit
fi

echo "Extracting RPM"
rpm_name="org.tizen.gtest-1.7.0-0.armv7l.rpm"
extract_command="rpm2cpio "${rpm_path}/${rpm_name}" | cpio -idmv"
echo ${extract_command}

eval "chmod 777 "${rpm_path}/${rpm_name}
sleep 1
eval ${extract_command}

echo "Copying gtest lib"
cp usr/bin/libgtest.a ${iotivity_test_root}/extlibs/gtest-1.7.0/bin/tizen
cp usr/bin/libgtest.a ${iotivity_test_root}/bin/tizen

echo "deleting tmp resources"

cd ${cwd_dir}

rm -rf ${tmp_dir}

eval "chmod 777 -R ${iotivity_test_root}/bin/tizen"
