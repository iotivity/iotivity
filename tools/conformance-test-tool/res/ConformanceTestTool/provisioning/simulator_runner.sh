#! /bin/bash

client_type=$1

MACHINE_TYPE=`uname -m`

if [ ${MACHINE_TYPE} == 'x86_64' ]; then
	echo 64bits Architecture
	rm -rf 64_bit
	mkdir 64_bit/libs
	cp ../../../../../out/linux/x86_64/release/libc_common.a ./64_bit/libs/libc_common.a
	cp ../../../../../out/linux/x86_64/release/libcoap.a ./64_bit/libs/libcoap.a
	cp ../../../../../out/linux/x86_64/release/libconnectivity_abstraction.a ./64_bit/libs/libconnectivity_abstraction.a
	cp ../../../../../out/linux/x86_64/release/liboc.so ./64_bit/libs/liboc.so
	cp ../../../../../out/linux/x86_64/release/liboc_logger.so ./64_bit/libs/liboc_logger.so
	cp ../../../../../out/linux/x86_64/release/libocpmapi.a ./64_bit/libs/libocpmapi.a
	cp ../../../../../out/linux/x86_64/release/libocprovision.a ./64_bit/libs/libocprovision.a
	cp ../../../../../out/linux/x86_64/release/libocsrm.a ./64_bit/libs/libocsrm.a
	cp ../../../../../out/linux/x86_64/release/liboctbstack.a ./64_bit/libs/liboctbstack.a
	cp ../../../../../out/linux/x86_64/release/liboctbstack.so ./64_bit/libs/liboctbstack.so
	cp ../../../../../out/linux/x86_64/release/libtinydtls.a ./64_bit/libs/libtinydtls.a
	cp ../../../../../out/linux/x86_64/release/resource/provisioning/examples/provisioningclient ./64_bit/provisioningclient
	cp ../../../../../out/linux/x86_64/release/resource/csdk/stack/samples/linux/secure/occlientbasicops ./64_bit/occlientbasicops
	cd ./64_bit
	rm -rf ./PDM.db
	rm -rf ./oic_svr_db_client.json
	ld_lib_path=$(pwd)/libs
	echo $ld_lib_path
	export LD_LIBRARY_PATH=$ld_lib_path
	
	if [ "$client_type" = "1" ];
	then
		echo "1 Seleted"
		cp ../db_files/provisioningcliokent.json ./oic_svr_db_client.json
		./provisioningclient
	else
		echo "2 Selected"
		cp ../db_files/occlientbasicops.json ./oic_svr_db_client.json
		./occlientbasicops -t 3
	fi 
  
else
	echo 32bits Architecture
	rm -rf 32_bit
	mkdir 32_bit/libs
	cp ../../../../../out/linux/x86/release/libc_common.a ./32_bit/libs/libc_common.a
	cp ../../../../../out/linux/x86/release/libcoap.a ./32_bit/libs/libcoap.a
	cp ../../../../../out/linux/x86/release/libconnectivity_abstraction.a ./32_bit/libs/libconnectivity_abstraction.a
	cp ../../../../../out/linux/x86/release/liboc.so ./32_bit/libs/liboc.so
	cp ../../../../../out/linux/x86/release/liboc_logger.so ./32_bit/libs/liboc_logger.so
	cp ../../../../../out/linux/x86/release/libocpmapi.a ./32_bit/libs/libocpmapi.a
	cp ../../../../../out/linux/x86/release/libocprovision.a ./32_bit/libs/libocprovision.a
	cp ../../../../../out/linux/x86/release/libocsrm.a ./32_bit/libs/libocsrm.a
	cp ../../../../../out/linux/x86/release/liboctbstack.a ./32_bit/libs/liboctbstack.a
	cp ../../../../../out/linux/x86/release/liboctbstack.so ./32_bit/libs/liboctbstack.so
	cp ../../../../../out/linux/x86/release/libtinydtls.a ./32_bit/libs/libtinydtls.a
	cp ../../../../../out/linux/x86/release/resource/provisioning/examples/provisioningclient ./32_bit/provisioningclient
	cp ../../../../../out/linux/x86/release/resource/csdk/stack/samples/linux/secure/occlientbasicops ./32_bit/occlientbasicops
	cd ./32_bit
	rm -rf ./PDM.db
	rm -rf ./oic_svr_db_client.json
	ld_lib_path=$(pwd)/libs
	echo $ld_lib_path
	export LD_LIBRARY_PATH=$ld_lib_path
	
	if [ "$client_type" = "1" ];
	then
		echo "1 Seleted"
		cp ../db_files/provisioningclient.json ./oic_svr_db_client.json
		./provisioningclient
	else
		echo "2 Selected"
		cp ../db_files/occlientbasicops.json ./oic_svr_db_client.json
		./occlientbasicops -t 3
	fi
fi
