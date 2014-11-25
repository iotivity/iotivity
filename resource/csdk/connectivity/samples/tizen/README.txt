Compiling Interface APIs:
=========================
1) Get the latest interface APIs code.
2) Copy the oic-resource code to linux machine.
3) Change the ROOT_DIR in interface APIs makefile such that it points to oic-resource/csdk in local linux environment.
4) Do "make" inside the interface APIs folder.

Compiling Sample App FOR TIZEN:
===============================
1) Get the oic-resource code(Changes done for Logging by SRI-B) from SRI-B and build it in Tizen GBS
2) Get the latest interface APIs code from repo and build it in Tizen GBS
	To Build in Tizen GBS Platform, Use this command "sudo gbs build -A armv7l --include-all"
3) For Verification, Build Sample Console App and execute in Tizen Device	

Procedure to Execute Sample App:
=================================
	1) Copy the generated rpm(com.samsung.interfacesample-0.1-1.armv7l.rpm) to Tizen Device (say /opt/usr/media/xxx/)
	2) sdb shell,
	3) su
	4) cd  /opt/usr/media/xxx/
	
	5) change-booting-mode.sh --update
	
	6) Install interfacesample(console app). It contains Interface API libs and OIC-CORE Libs
		i) rpm -Uvh com.samsung.interfacesample-0.1-1.armv7l.rpm --force --nodeps
		
	7) Execute smack_reload.sh
		
	8) rpm will be installed in /usr/apps/  Execute the sample app by below commands
		i) cd /usr/apps/com.samsung.interfacesample/bin
		ii) ./sample