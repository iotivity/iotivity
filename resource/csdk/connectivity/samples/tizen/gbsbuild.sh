#!/bin/sh

if [ "$1" = "WITH_WIFI" ] 
then
	echo "Building Interface Sample for WIFI transport"
    adapterMacro="$adapterMacro -DWIFI_ADAPTER_TEST"
elif [ "$1" = "WITH_BT" ]
then
	echo "Building Interface Sample for BT transport"
    adapterMacro="$adapterMacro -DBT_ADAPTER_TEST"	
elif [ "$1" = "WITH_BLE" ] 
then
	echo "Building Interface Sample for BLE transport"
    adapterMacro="$adapterMacro -DBLE_ADAPTER_TEST"
elif [ "$1" = "" ]
then
	echo "Building Interface Sample for ALL transports"
    adapterMacro="$adapterMacro -DWIFI_ADAPTER_TEST -DBT_ADAPTER_TEST -DBLE_ADAPTER_TEST -DETHERNET_ADAPTER"	
else
    echo "Please specify valid transport option: WITH_WIFI or WITH_BT or WITH_BLE"
	exit 1	
fi
	
sed "/ADAPTER_MACRO :=/ c ADAPTER_MACRO :=$adapterMacro" Makefile > Makefile.tmp

rm Makefile
mv Makefile.tmp Makefile
dos2unix Makefile
	
git init
gbs build -A armv7l --include-all -B ~/GBS-ROOT-NEW


