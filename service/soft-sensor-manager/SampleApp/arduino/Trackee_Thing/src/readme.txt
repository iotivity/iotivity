
Followings should be done befor building Arudno applications 

1. Applications on arduino conform  the Iotivity Base released on 22-Aug-2014.
   (Ver. 22th Aug(oic-resource: commit: cdeeed62bd9b11beb2cdd340168e766088c18dac [cdeeed6]))

   For successful execution, you have to copy the file, local.properties, into the following pathes:
    - oic-resource/csdk/
    - oic-resource/csdk/libcoap-4.1.1


2. Time.c, Time.h should be copied in your machine and they should be refered in the Makefile as followings

   Makefile path:  / oic-resource / csdk / libcoap-4.1.1 / makefile

   path to be modified: 
  
     - INCD_ARD_TIME = -I$(ARDUINO_DIR)/libraries/Time ---> for Time.h
     - SDIR_ARD_TIME = $(ARDUINO_DIR)/libraries/Time ---> for Time.c