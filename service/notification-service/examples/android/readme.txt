Steps to be followed for android build:

1. Build iotivity: 
	sudo scons TARGET_OS=android TARGET_ARCH=armeabi TARGET_TRANSPORT=IP RELEASE=0 LOGGING=true

2. NDK-BUILD:
	2.1 export android-ndk path:
		<path-to-iotivity>/extlibs/android/ndk/android-ndk-r10d/ndk-build
	2.2 navigate to android-sdk folder
		<path-to-iotivity>/service/notification-service/src/android/
	2.3 build command
		<path-to-iotivity>/extlibs/android/ndk/android-ndk-r10d/ndk-build
	2.4 This step will generate the libnotification_service_jni.so in the libs folder
	
3. Setting up SDK projetc
	3.1 Import NotificationService SDK Project(service/notification-service/src/android)  in android studio
	3.2 add  iotivity-iotivity-base-armeabi-debug.aar to this project 
	3.3 Build project
	3.4 The above step will generate app-debug.aar file which will be given to the sample APP

4. Set up Sample Consumer and Producer Applications:
	4.1 Import sample consumer project in android studio
	4.2 Import iotivity-base-armeabi-debug.aar 
	4.3 Import app-debug.aar from the NotificationService SDK Project(build/outputs/aar)
	4.3 Add the following jni in jnilibs/armeabi folder(generated during the ndk-build)
		-libnotification_service_jni.so
		-librcs_client.so
		-librcs_common.so
		-librcs_server.so
	4.4 Follow the same steps for sample producer project