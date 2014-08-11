//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef __INTEL_OCAPI_H_2014_07_10
 #define __INTEL_OCAPI_H_2014_07_10

#include <string>
#include <vector>
#include <map>

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

 enum class RequestHandlerFlag
 {
    InitFlag,
    RequestFlag,
    ObserverFlag
 };

 // TODO: To find the complete JSon data structure and modify map value type
 // Typedef for attribute values and attribute map. 
 typedef std::vector<std::string> AttributeValues;
 typedef std::map<std::string, AttributeValues> AttributeMap; 

 // Typedef for query parameter map
 typedef std::map<std::string, std::string> QueryParamsMap;

 // const strings for different interfaces

 // Default interface
 const std::string DEFAULT_INTERFACE = "oc.mi.def";

 // Used in discovering (GET) links to other resources of a collection.  
 const std::string LINK_INTERFACE = "oc.mi.ll";

 // Used in GET, PUT, POST, DELETE methods on links to other resources of a collection. 
 const std::string BATCH_INTERFACE = "oc.mi.b";

} // namespace OC

#endif  
