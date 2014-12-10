This is a temporary build instruction for android platform before it can be
merged intot the main build process.

REQUIREMENT:
- Android NDK revision 10 or later
- Android SDK API level 19

INSTRUCTION:

- Native libraries:
	cd resource
	export NDK_DIR=[PATH TO YOUR NDK]
	export ANDROID_NDK_HOME=$NDK_DIR 
	export ANDROID_HOME=[PATH TO YOUR SDK] 
	export PATH=$NDK_DIR/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86_64/bin:$PATH
	export PATH=$ANDROID_HOME/tools:$PATH
	export PATH=$ANDROID_HOME/platform-tools:$PATH
	make PLATFORM=android

	The required native libraries for Android JAVA/JNI layer are built under
		csdk/android/release/liboctbstack.a
		release/obj/liboc.a
		oc_logger/lib/oc_logger.a

- JAVA/JNI libraries:
	cd oic-resource/android/Base
	./gradlew clean; ./gradlew build

	The JAVA/JNI libraries are built under
		app/build/native-libs/armeabi/*.so
		app/build/build/outputs/aar/app.aar

- The sample app SimpleClient
	cd oic-resource/android/Base
	cp -r ./app/build/native-libs/* ../SimpleClient/app/src/main/jniLibs/
	cp ./app/build/outputs/aar/app.aar ../SimpleClient/app/libs/
	cd oic-resource/android/SimpleClient
	./gradlew clean; ./gradlew build

	The output APK is built under
		  app/build/outputs/apk/app-debug.apk
