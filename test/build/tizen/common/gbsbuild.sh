#!/bin/bash
set -e
#----------------------------------------------------------------------
# Main Build Script For all module in tizen platform
#----------------------------------------------------------------------

args=''

for i in `seq 1 $#`
do
    eval arg=\${${i}}
    arg=${arg// /+}
    args+=${arg}' '
done

arg_parts=(${args//=/ })
len=${#arg_parts[@]}

# Default values if unset
iotivity_root="${PWD}"
iotivity_test_root="${iotivity_root}/test"
target_arch="armv7l"
gbs_root="${iotivity_root}/tmp/gbs"

parameters=''
offline_mode=''
project_version=''
i=0
while [ ${i} -lt ${len} ]; do
    arg_value=${arg_parts[i+1]}
    arg_value=${arg_value//+/}
    arg_value=${arg_value//\'/}
    arg_value=${arg_value//[/}
    arg_value=${arg_value//]/}
    if [[ "${arg_parts[i]}" = "IOTIVITY_ROOT" ]]; then
         iotivity_root=${arg_value}
    elif [[ "${arg_parts[i]}" = "IOTIVITY_TEST_ROOT" ]]; then
         iotivity_test_root=${arg_value}
    elif [[ "${arg_parts[i]}" = "MODULE" ]]; then
         module=${arg_value}
         parameters+=" --define 'MODULE ${module}'"
    elif [[ "${arg_parts[i]}" = "TARGET_TRANSPORT" ]]; then
         target_transport=${arg_value}
         parameters+=" --define 'TARGET_TRANSPORT ${target_transport}'"
    elif [[ "${arg_parts[i]}" = "TARGET_ARCH" ]]; then
         target_arch=${arg_value}
         parameters+=" --define 'TARGET_ARCH ${target_arch}'"
    elif [[ "${arg_parts[i]}" = "TEST" ]]; then
         test=${arg_value}
         parameters+=" --define 'TEST ${test}'"
    elif [[ "${arg_parts[i]}" = "SDK" ]]; then
         sdk=${arg_value}
        parameters+=" --define 'SDK ${sdk}'"
    elif [[ "${arg_parts[i]}" = "KAFKA_ROOT" ]]; then
        kafka_root=${arg_value}
        parameters+=" --define 'KAFKA_ROOT ${kafka_root}'"
    elif [[ "${arg_parts[i]}" = "TLS_MODE" ]]; then
        tls_mode=${arg_value}
        parameters+=" --define 'TLS_MODE ${tls_mode}'"
    elif [[ "${arg_parts[i]}" = "CLOUD_SERVICE" ]]; then
        cloud_service=${arg_value}
        parameters+=" --define 'CLOUD_SERVICE ${cloud_service}'"
    elif [[ "${arg_parts[i]}" = "GBS_ROOT" ]]; then
        gbs_root=${arg_value}
    elif [[ "${arg_parts[i]}" = "TIZEN_SDB" ]]; then
        tizen_sdb=${arg_value}
    elif [[ "${arg_parts[i]}" = "RPMS_PATH" ]]; then
        rpms_path=${arg_value}
    elif [[ "${arg_parts[i]}" = "PROJECT_VERSION" ]]; then
        project_version=${arg_value}
    elif [[ "${arg_parts[i]}" = "PUSH" ]]; then
        push=${arg_value}
    elif [[ "${arg_parts[i]}" = "SECURED" ]]; then
         parameters+=" --define 'SECURED ${arg_value}'"
    elif [[ "${arg_parts[i]}" = "lib_rpm_names" ]]; then
        temp=${arg_value}
        lib_rpm_names=(${temp//,/ })
    elif [[ "${arg_parts[i]}" = "test_res_files" ]]; then
        test_res_files=${arg_value}
        test_res_files=(${test_res_files//,/ })
    elif [[ "${arg_parts[i]}" = "OFFLINE" ]]; then
        offline_mode=${arg_value}
    fi
    let i=i+2
done

parameters+=' '${offline_mode}

if [[ "${project_version}" == "" ]]; then
    spec=`ls ${iotivity_root}/tools/tizen/*.spec`
    version=`rpm --query --queryformat '%{version}\n' --specfile $spec`
    release=`rpm --query --queryformat '%{release}\n' --specfile $spec`
    name=`echo $name|cut -d" " -f 1`
    version=`echo $version|cut -d" " -f 1`
    release_number=`echo $release|cut -d" " -f 1`
    project_version=${version}'-'${release_number}
else
    IFS='-' read -a tmp_array <<< "${project_version}"
    len=${#arg_parts[@]}
    if [[ ${len} -lt 2 ]]; then
        echo 'Problem with PROJECT_VERSION value. PROJECT_VERSION format: w.x.x-z'
        exit 127
    fi

    version=${tmp_array[0]}
    release_number=${tmp_array[1]}
fi

echo 'Creating Temporary Directory '${module}
echo 'Please wait. It will take a few seconds...'

tmp_dir=${iotivity_test_root}/'bin/tizen/tmp/'${module}
target_dir=${iotivity_test_root}/'bin/tizen'

test_rpm_name='com-oic-'${module}'-test-'${project_version}'.'${target_arch}'.rpm'

rm -rf ${tmp_dir}

mkdir -p ${tmp_dir}
mkdir ${tmp_dir}/packaging
mkdir ${tmp_dir}/test
mkdir ${tmp_dir}/iotivity
mkdir ${tmp_dir}/iotivity/extlibs
mkdir -p ${tmp_dir}/iotivity/extlibs/mbedtls/mbedtls/include

cp -R ${iotivity_root}/resource ${tmp_dir}/iotivity/
cp -R ${iotivity_root}/service ${tmp_dir}/iotivity/
cp -R ${iotivity_root}/extlibs/cjson ${tmp_dir}/iotivity/extlibs
cp -R ${iotivity_root}/extlibs/tinycbor ${tmp_dir}/iotivity/extlibs
cp -rf ${iotivity_root}/extlibs/mbedtls/mbedtls/include/mbedtls/ ${tmp_dir}/iotivity/extlibs/mbedtls/mbedtls/include
cp -R ${iotivity_test_root}/src ${tmp_dir}/test/
cp -R ${iotivity_test_root}/include ${tmp_dir}/test/
cp -R ${iotivity_test_root}/res ${tmp_dir}/test/
cp -R ${iotivity_test_root}/extlibs ${tmp_dir}/test/

#cp ${iotivity_test_root}/build/tizen/${module}/com.oic.${module}.test.spec ${tmp_dir}/packaging/com.oic.${module}.test.spec
cp ${iotivity_test_root}/build/tizen/common/com.oic.common.test.spec ${tmp_dir}/packaging/com.oic.${module}.test.spec
cp ${iotivity_test_root}/build/tizen/common/com.oic.common.test.manifest ${tmp_dir}/com.oic.${module}.test.manifest
cp ${iotivity_test_root}/build/tizen/common/com.oic.common.test.xml ${tmp_dir}/com.oic.${module}.test.xml

sed -i "s/Version: w.x.y/Version: ${version}/g" ${tmp_dir}/packaging/com.oic.${module}.test.spec
sed -i "s/Release: z/Release: ${release_number}/g" ${tmp_dir}/packaging/com.oic.${module}.test.spec
sed -i "s/com-oic-common-test/com-oic-${module}-test/g" ${tmp_dir}/packaging/com.oic.${module}.test.spec
sed -i "s/com_oic_common_test/com_oic_${module}_test/g" ${tmp_dir}/com.oic.${module}.test.manifest
sed -i "s/com.oic.common.test/com.oic.${module}.test/g" ${tmp_dir}/com.oic.${module}.test.manifest
sed -i "s/com_oic_common_test/com_oic_${module}_test/g" ${tmp_dir}/com.oic.${module}.test.xml
sed -i "s/com.oic.common.test/com.oic.${module}.test/g" ${tmp_dir}/com.oic.${module}.test.xml

#if [[ "${module}" != "ca" ]]; then
sed -i "s/needed-common-libs/${module}/g" ${tmp_dir}/packaging/com.oic.${module}.test.spec
#fi

sed -i "s/common_test/${module}_test/g" ${tmp_dir}/packaging/com.oic.${module}.test.spec
sed -i "s/common_test/${module}_test/g" ${tmp_dir}/com.oic.${module}.test.manifest
sed -i "s/common_test/${module}_test/g" ${tmp_dir}/com.oic.${module}.test.xml

cp ${iotivity_test_root}/build/tizen/${module}/SConstruct ${tmp_dir}
cp ${iotivity_root}/tools/tizen/.gbs.conf ${tmp_dir}

echo "Initializing Git repository"

cwd_dir=`pwd`
cd ${tmp_dir}

echo `pwd`

if [ ! -d .git ]; then
   git init ./
   git config user.email "you@example.com"
   git config user.name "Your Name"
   git add ./
   git commit -m "Initial commit"
fi

echo "Calling core gbs build command"

gbscommand="gbs build -A ${target_arch} -B ${gbs_root} --repository ./ --include-all ${parameters}"

echo ${gbscommand}

if eval ${gbscommand}; then
    echo "GBS build is successful"
    cd ${cwd_dir}
    echo "Extracting RPM"

    cd ${tmp_dir}
    extractCommand="rpm2cpio "${rpms_path}/${test_rpm_name}" | cpio -idmv"
    echo ${extractCommand}

    chmod 777 ${rpms_path}/${test_rpm_name}
    sleep 1
    eval ${extractCommand}

    echo "Copying RPM of "${module}
    cp ${rpms_path}/${test_rpm_name} ${target_dir}

    chmod 777 -R ${target_dir}

    if [[ "${push}" == *"exe"* || "${push}" == *"lib"* ]]; then
        devices=($(${tizen_sdb} devices | awk '/[a-zA-Z0-9]+[ \t]+device[ \t]+[a-zA-Z0-9-]+$/{print $1}'))
        for device in "${devices[@]}";
        do
            ${tizen_sdb} -s ${device} root on

            ${tizen_sdb} -s ${device} shell rm -rf /usr/bin/${module}
            ${tizen_sdb} -s ${device} shell rm -rf /usr/apps/com.oic.${module}.test
            ${tizen_sdb} -s ${device} shell mkdir -p /usr/bin/${module}/bin
            ${tizen_sdb} -s ${device} shell mkdir -p /usr/apps/com.oic.${module}.test/bin

            if [[ "${push}" == *"lib"* ]]; then
                for lib_rpm_name in "${lib_rpm_names[@]}";
                do
                    if ! [[ -f ${rpms_path}/${lib_rpm_name}'-'${project_version}'.'${target_arch}'.rpm' ]]; then
                        echo 'Unable to find '${rpms_path}/${lib_rpm_name}'-'${project_version}'.'${target_arch}'.rpm'
                        exit 127
                    fi

                    chmod 777 ${rpms_path}/${lib_rpm_name}
                    extractCommand='rpm2cpio '${rpms_path}/${lib_rpm_name}'-'${project_version}'.'${target_arch}'.rpm | cpio -idmv'
                    eval ${extractCommand}
                    ${tizen_sdb} -s ${device} push ${rpms_path}'/'${lib_rpm_name}'-'${project_version}'.'${target_arch}'.rpm' /tmp
                    ${tizen_sdb} -s ${device} shell rpm -Uvh 'tmp/'${lib_rpm_name}'-'${project_version}'.'${target_arch}'.rpm' --force --nodeps
                done
                if [[ "${module}" == "rc" ]]; then
                    ${tizen_sdb} -s ${device} push ${tmp_dir}/usr/lib/* /usr/bin/${module}/
                    ${tizen_sdb} -s ${device} push ${tmp_dir}/usr/lib/* /opt/usr/media/bin/
                    ${tizen_sdb} -s ${device} push ${tmp_dir}/usr/lib/* /usr/apps/com.oic.${module}.test/bin
                fi
            fi
            if [[ "${push}" == *"exe"* ]]; then
                ${tizen_sdb} -s ${device} push ${rpms_path}'/'${test_rpm_name} /tmp
                ${tizen_sdb} -s ${device} shell rpm -Uvh 'tmp/'${test_rpm_name} --force --nodeps
                ${tizen_sdb} -s ${device} shell cp -rf /usr/apps/com.oic.${module}.test/bin /opt/usr/media/
                for test_res_file in "${test_res_files[@]}";
                do
                    echo 'push ${test_res_file} file'
                    ${tizen_sdb} -s ${device} push ${test_res_file} /opt/usr/media/
                    ${tizen_sdb} -s ${device} push ${test_res_file} /usr/bin/${module}/
                    ${tizen_sdb} -s ${device} push ${test_res_file} /opt/usr/media/bin/
                    ${tizen_sdb} -s ${device} push ${test_res_file} /usr/apps/com.oic.${module}.test
                    ${tizen_sdb} -s ${device} push ${test_res_file} /usr/apps/com.oic.${module}.test/bin
                done
            fi
        done
    fi
    cd ${cwd_dir}
else
    echo "GBS build failed."
    cd ${cwd_dir}
    exit 127
fi

#echo "Deleting Tmp Directory"
#rm -rf ${tmp_dir}
