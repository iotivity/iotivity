Compiling Interface APIs FOR TIZEN:
===================================
1) Get the latest interface APIs code from repo and build it in Tizen GBS[connectivity/build]
        To Build in Tizen GBS Platform, Use this command "sudo make PLATFORM=tizen" from the above directory.

	By Default, "ALL" the TRANSPORTS are built. To build a specific TRANSPORT [BT,BLE,WIFI], use the following command :
	sudo make PLATFORM=tizen TRANSPORT=BLE
    
	[Note: Combination of Transports can be build by giving the option as TRANSPORT=BLE,BT. Transport names are case insensitive]

2) For Verification, Build Sample Console App and execute in Tizen Device.

Note:
====
For Tizen BLE, please copy headers present in "external\headers\include" folder to your GBS configuration(GBS has old BLE headers).