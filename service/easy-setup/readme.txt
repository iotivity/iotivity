
Steps to build and deploy Easysetup Mediator in Android platform

1) Compilation
	0) Prerequisite exports for Android
		export ANDROID_HOME=<ANDROID_HOME>
		export ANDROID_NDK=<ANDROID_NDK>
	a) In the IoTivity root source folder execute
		 scons TARGET_OS=android TARGET_ARCH=armeabi TARGET_TRANSPORT=IP RELEASE=0
	
	b) Step (a) will generate cross compiled base libraries and places them in the folder
		/out/android/armeabi/debug
		
	c) Now navigate to the Easysetup NDK folder 
		<iotivity-root>/service/easy-setup/sdk/android
		
	d) Execute NDK-Build with the following command to cross compiled Easysetup JNI .so files
		 ~/madan/android-ndk-r10d/ndk-build
		 
	e) Step (d) will generate cross compiled Easysetup JNI .so files in the following folder
		
		
		
2) Execution
	
	0) Prerequisite for running the Android application
		Install Android SDK and ADK in eclipse using the Android SDK manager
	
	a) Import following projects to the Eclipse using the "Import->Android->Existing Android Code Into Workspace"
		- service\easy-setup\sdk\android\EasySetupCore
		- service\easy-setup\sampleapp\android\EasySetup
		 
	b) Drag and drop the following .so files in to the EasySetup->libs->armeabi folder
		- libca-interface.so
		- libconnectivity_abstraction.so
		- libeasysetup-jni.so [Easy Setup file]
		- libESSDK.so [Easy Setup file]
		- libgnustl_shared.so
		- liboc_logger.so
		- liboc.so
		- libocstack-jni.so
		- liboctbstack.so
	c) Run "Easy Setup" as an Android application
	
Folder structure
iotivity/
└── service
	└── easy-setup/
		│── SConscript
		│── sdk
		│	└── inc
		│	└── src
		│	└── android
		│	└── arduino
		│		└──wifi
		│			└──inc		
		│			└──src		
		│── sampleapp
		│	└──android
		│	└──arduino		
		│	└──linux
		└── README - You are reading this.


