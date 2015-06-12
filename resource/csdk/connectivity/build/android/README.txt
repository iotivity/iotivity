Compiling Interface APIs FOR Android:
===================================
•Preconditons :
	Please download the following
	1) tinydlts library and keep at $(CA_HOME)/lib/tinydtls
•Supported version : 5.0 ( Lollipop )
•Required NDK version : android-ndk-r10d ( https://developer.android.com/tools/sdk/ndk/index.html )
• Modify Makefile ( connectivity/build/android/Makefile )
    $ cd ${CA_HOME}/connectivity/build/android
    $ vi Makefile
  #  Change the NDK_PATH
    NDK_PATH = ${your_ndk_path}
 Build : $(CA_HOME)/build/android/Makefile
    $ cd ${CA_HOME}/connectivity/build/android
    $ ndk-build clean; ndk-build


