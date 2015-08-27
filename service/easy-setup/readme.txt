
Steps to build and deploy Easysetup Mediator in Android platform

1) Compilation using Scons
	0) Prerequisite exports for Android
		export ANDROID_HOME=<ANDROID_HOME>
		export ANDROID_NDK=<ANDROID_NDK>
	a) In the IoTivity root source folder execute
		 scons TARGET_OS=android TARGET_ARCH=armeabi TARGET_TRANSPORT=IP RELEASE=0
	
	b) If the project is setup correctly, you should see a BUILD SUCCESSFUL message on the terminal
		You should see the .aar files generated inside of '<iotivity>/android/android_api/base/build/outputs/aar' directory. The .aar files contain jni directory and also a .jar file
		
	c) Now navigate to the Easysetup NDK folder 
		<iotivity-root>/service/easy-setup/sdk/mediator/android
		
	d) Execute NDK-Build with the following command to cross compiled Easysetup JNI .so files
		 ~/madan/android-ndk-r10d/ndk-build
		 
	e) Step (d) will generate cross compiled Easysetup JNI .so files in the following folder
		
2) Compilation using Android Studio
	
	0) Prerequisite for running the Android application
		Install Android Studio and use the Android SDK manager to update the Android API to 21
	
	a) Import following projects to the Studio using the "File->New->Import Project"
		- service\easy-setup\sdk\android\EasySetupCore
		- service\easy-setup\sampleapp\android\EasySetup
	
	b) EasySetupCore is dependent on the .aar files generated in 1.b 
		1) To add an .aar file to the 'EasySetupCore' project,
			a.Right click on EasySetup->New->Module->Import .JAR or .AAR Package
			b.Select the .aar file(iotivity-armeabi-base-*.aar) from the location in steps 1.b
			c.Right click on EasySetup->Open Module Settings
			d.Select each sample app module under 'Modules'->Dependencies->Add->Module Dependency
			
	c)	Build EasySetupCore project using "Android Studio->Build->Make Module "easySetupCore"
		If the project is setup correctly, you should see a BUILD SUCCESSFUL in the Android Studio Gradle Build terminal
			You should see the .aar file generated inside of 
			'iotivity\service\easy-setup\sdk\mediator\android\easySetupCore\build\outputs\aar'
	
	d) EasySetup application is also dependent on the .so files generated in 1.d step
		1) Drag and drop the following .so files in to the EasySetup\app\src\main\jniLibs\armeabi folder
			- libeasysetup-jni.so [Easy Setup file]
			
	e) EasySetup is also dependent on the .aar files generated in 1.b and 2.c steps
		1) To add an .aar file to the 'EasySetup' project,
			a.Right click on EasySetup->New->Module->Import .JAR or .AAR Package
			b.Select the .aar file from the location in steps 1.b and 2.b above
			c.Right click on EasySetup->Open Module Settings
			d.Select each sample app module under 'Modules'->Dependencies->Add->Module Dependency
			e.The .aar files (easySetupCore-*.aar & iotivity-armeabi-base-*.aar) selected in step 2.c.1.b above should be present. Select it.
	
			
	e) Run "Easy Setup" as an Android application
	
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


