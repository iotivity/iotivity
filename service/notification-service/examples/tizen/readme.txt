Steps fot Tizen setup :-

1. Import NMConsumerApp from “service\notification-service\examples\tizen”.(Do not copy it in your workspace)

2. Build base libraries and Notification Manger libraries using “sh gbsbuild.sh” from Iotivity directory .

3. Extract the required libraries from generated rpm (iotivity-0.9-0.armv7l.rpm & iotivity-service-0.9.2-0.armv7l.rpm) 
    to the “lib” folder of NMSampleApp project. Last four libs can be found in <user-home>/GBS-ROOT/local/BUILD-ROOTS/scratch.armv7l.0/lib
4.	
   Required libraries are
	•	liboc.so
	•	liboctbstack.so
	•	libconnectivity_abstraction.so
	•	liboc_logger.so
	•	liboc_logger_core.so
	•	libnmservice.so
	•	librc_client.so
	•	librc_common.so
	•	librc_client.so
	•	libboost_date_time.so
	•	libboost_system.so
	•	libboost_thread.so
	•	libuuid.so

5. Sample application uses boost libraries. Download it from http://www.boost.org/ and keep it unzipped.

6. Specify the path of the unzipped boost libraries in NMConsumerApp project in the way given below. 

7. Right click the project in IDE -> Properties -> expand C/C++ Build -> Settings -> Tool Settings -> select includes 
   option under C++ Compiler -> add the path of the unzipped boost libraries in the include paths area -> Apply -> OK.
   
8. Build NMConsumerApp project.

9. Run the SampleNotificationProducer of the linux sample -
    • Go to /iotivity/service/mp-notification-manager/out/linux/x86/release
    • Use this command to export the libraries "export LD_LIBRARY_PATH=."
    • run "./sampleNotificationProvider"
	
10. Launch the 'NMConsumerApp' application in Tizen 2.3 device to start testing.
