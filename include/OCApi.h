//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef __INTEL_OCAPI_H_2014_07_10
 #define __INTEL_OCAPI_H_2014_07_10

#include <string>
#include <vector>

namespace OC {

class OCResource;

} // namespace OC

namespace OC { namespace OCReflect {

struct entity;

}} // namespace OC::OCReflect

namespace OC {

 enum class OCPlatformStatus {
	PlatformUp,
	PlatformDown
 };

 enum class OCAdvertisementStatus{
	None
 };

 typedef std::string URI;

 enum class ServiceType
 {
	 InProc,
	 OutOfProc
 };

 enum class ModeType
 {
	 Server,
	 Client,
	 Both
 };

 struct PlatformConfig
 {
	 ServiceType serviceType; // This will indicate whether it is InProc or OutOfProc
	 ModeType mode; // This will indicate whether we want to do server, client or both
	 std::string ipAddress; // This is the ipAddress of the server to connect to
     uint16_t port; // Port of the server
 };

} // namespace OC

#endif	
