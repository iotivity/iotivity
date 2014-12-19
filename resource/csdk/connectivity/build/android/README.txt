Compiling Interface APIs FOR Android:
===================================
•Preconditons :
	Please download the glibc directory and keep $(CA_HOME)\lib\android\glib-master
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

