//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

#ifndef _STRING_CONSTANTS_H_
#define _STRING_CONSTANTS_H_

#include <string>

using namespace std;

namespace OC
{
    namespace InitException
    {
        static const std::string NO_ERROR                   = "No Error";
        static const std::string INVALID_URI                = "Invalid URI";
        static const std::string INVALID_PORT               = "Invalid Port";
        static const std::string INVALID_IP                 = "Invalid IP";
        static const std::string INVALID_CB                 = "Invalid Callback";
        static const std::string INVALID_METHOD             = "Invalid Method";
        static const std::string GENERAL_FAULT              = "General Fault";
        static const std::string UNKNOWN_ERROR              = "Unknown Error";

        static const std::string STACK_INIT_ERROR           = "Error Initializing Stack";
        static const std::string NOT_CONFIGURED_AS_SERVER   =
                          "Cannot static construct a Server when configured as a client";
        static const std::string INVALID_PARAM              = "Invalid Param";
        static const std::string MISSING_URI                = "Missing URI;";
        static const std::string MISSING_TYPE               = "Missing Resource Type;";
        static const std::string MISSING_INTERFACE          = "Missing Interface;";
        static const std::string MISSING_CLIENT_WRAPPER     = "Missing ClientWrapper;";
    }

    namespace Exception // Not To Be Confused With 'InitException'
    {
        static const std::string SVCTYPE_OUTOFPROC          = "ServiceType::OutOfProc";
        static const std::string BIND_TYPE_FAILED           = "Bind Type to resource failed";
        static const std::string BIND_INTERFACE_FAILED      = "Bind Interface to resource failed";
        static const std::string START_PRESENCE_FAILED      = "startPresence failed";
        static const std::string END_PRESENCE_FAILED        = "stopPresence failed";
        static const std::string INVALID_ARRAY              = "Array type should have at least []";
        static const std::string STR_NULL_RESPONSE          = "Response is NULL";
        static const std::string STR_PAYLOAD_OVERFLOW       = "Payload overflow";
        static const std::string NIL_GUARD_NULL             = "nullptr at nil_guard()";
        static const std::string GENERAL_JSON_PARSE_FAILED  = "JSON Parser Error";
        static const std::string RESOURCE_UNREG_FAILED      = "Unregistering resource failed";
        static const std::string OPTION_ID_RANGE_INVALID    =
                            "Error: OptionID valid only from 2048 to 3000 inclusive.";

        static const std::string NO_ERROR                   = "No Error";
        static const std::string RESOURCE_CREATED           = "Resource Created";
        static const std::string RESOURCE_DELETED           = "Resource Deleted";
        static const std::string INVALID_URI                = "Invalid URI";
        static const std::string INVALID_IP                 = "Invalid IP";
        static const std::string INVALID_PORT               = "Invalid Port";
        static const std::string INVALID_CB                 = "Invalid Callback";
        static const std::string INVALID_METHOD             = "Invalid Method";
        static const std::string INVALID_QUERY              = "Invalid Query";
        static const std::string INVALID_PARAM              = "Invalid Param";
        static const std::string INVALID_OBESERVE           = "Invalid Observe Param";
        static const std::string NO_MEMORY                  = "No Memory";
        static const std::string COMM_ERROR                 = "Communication Error";
        static const std::string NOT_IMPL                   = "Not Implemented";
        static const std::string NOT_FOUND                  = "Resource Not Found";
        static const std::string RESOURCE_ERROR             = "Resource Error";
        static const std::string SLOW_RESOURCE              = "Slow Resource";
        static const std::string NO_OBSERVERS               = "No Observers";
        static const std::string OBSV_NO_FOUND              = "Stack observer not found";
        static const std::string OBSV_NOT_ADDED             = "Stack observer not added";
        static const std::string OBSV_NOT_REMOVED           = "Stack observer not removed";
        static const std::string STACK_RESOURCE_DELETED     = "The specified resource has been deleted";
        static const std::string PRESENCE_STOPPED           = "Stack presence stopped";
        static const std::string PRESENCE_TIMEOUT           = "Stack presence timed out";
        static const std::string PRESENCE_NOT_HANDLED       = "Stack presence should not be handled";
        static const std::string INVALID_OPTION             = "Invalid option";
        static const std::string GENERAL_FAULT              = "General Fault";
        static const std::string MALFORMED_STACK_RESPONSE   = "Response from OC_STACK is malformed";
        static const std::string VIRTUAL_DO_NOT_HANDLE      = "Virtual Do Not Handle";
        static const std::string PERSISTENT_BUFFER_REQUIRED = "Persistent response buffer required";
        static const std::string STACK_CONTINUE             = "Stack continue";
        static const std::string INVALID_REQUEST_HANDLE     = "Invalid request handle";
        static const std::string UNKNOWN_ERROR              = "Unknown Error";
        static const std::string INVALID_REPRESENTATION     = "Invalid Payload JSON";
        static const std::string INVALID_JSON_TYPE          = "Unrecognized JSON Type ";
        static const std::string INVALID_JSON_NUMERIC       = "Unrecognized JSON Numeric ";
        static const std::string INVALID_JSON_ARRAY_DEPTH   = "Max JSON Array Depth exceeded";
        static const std::string INVALID_JSON_TYPE_TAG      = "Invalid JSON Type Tag";
        static const std::string INVALID_ATTRIBUTE          = "Invalid Attribute: ";
    }

    namespace Error
    {
        static const std::string INVALID_IP                 = "Invalid IP";
    }

    namespace PlatformCommands
    {
        static const std::string GET                        = "GET";
        static const std::string PUT                        = "PUT";
        static const std::string POST                       = "POST";
        static const std::string DELETE                     = "DELETE";
    }

    namespace Key
    {
        static const std::string OCKEY                      = "oc";
        static const std::string URIKEY                     = "href";
        static const std::string OBSERVABLEKEY              = "obs";
        static const std::string RESOURCETYPESKEY           = "rt";
        static const std::string INTERFACESKEY              = "if";
        static const std::string PROPERTYKEY                = "prop";
        static const std::string REPKEY                     = "rep";
        static const std::string SECUREKEY                  = "sec";
        static const std::string PORTKEY                    = "port";
  }

}

#endif // _STRING_CONSTANTS_H_
