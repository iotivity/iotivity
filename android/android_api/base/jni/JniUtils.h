/*
* //******************************************************************
* //
* // Copyright 2015 Intel Corporation.
* //
* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
* //
* // Licensed under the Apache License, Version 2.0 (the "License");
* // you may not use this file except in compliance with the License.
* // You may obtain a copy of the License at
* //
* //      http://www.apache.org/licenses/LICENSE-2.0
* //
* // Unless required by applicable law or agreed to in writing, software
* // distributed under the License is distributed on an "AS IS" BASIS,
* // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* // See the License for the specific language governing permissions and
* // limitations under the License.
* //
* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

#include "JniOcStack.h"
#include "OCRepresentation.h"

class JniUtils
{
public:
    static void convertJavaMapToQueryParamsMap(JNIEnv *env, jobject hashMap,
        OC::QueryParamsMap &map);
    static jobject convertQueryParamsMapToJavaMap(JNIEnv *env, const OC::QueryParamsMap &map);

    static jobject convertStrVectorToJavaStrList(JNIEnv *env, std::vector<std::string> &vector);
    static void convertJavaStrArrToStrVector(JNIEnv *env, jobjectArray jStrArr,
        std::vector<std::string> &vector);

    static void convertJavaHeaderOptionsArrToVector(JNIEnv *env, jobjectArray jHeaderOptions,
        OC::HeaderOptions& headerOptions);
    static jobject convertHeaderOptionsVectorToJavaList(JNIEnv *env,
        const OC::HeaderOptions& headerOptions);

    static void convertJavaRepresentationArrToVector(JNIEnv *env,
        jobjectArray jRepresentationArray,
        std::vector<OC::OCRepresentation>& representationVector);
    static jobjectArray convertRepresentationVectorToJavaArray(JNIEnv *env,
        const std::vector<OC::OCRepresentation>& representationVector);

    static OC::ServiceType getServiceType(JNIEnv *env, int type)
    {
        switch (type) {
        case 0:
            return OC::ServiceType::InProc;
        case 1:
            return OC::ServiceType::OutOfProc;
        default:
            ThrowOcException(OC_STACK_INVALID_PARAM, "Unexpected service type");
            return OC::ServiceType::OutOfProc;
        };
    }

    static OC::ModeType getModeType(JNIEnv *env, int type)
    {
        switch (type) {
        case 0:
            return OC::ModeType::Server;
        case 1:
            return OC::ModeType::Client;
        case 2:
            return OC::ModeType::Both;
        default:
            ThrowOcException(OC_STACK_INVALID_PARAM, "Unexpected mode type");
            return OC::ModeType::Both;
        };
    }

    static OC::QualityOfService getQOS(JNIEnv *env, int type)
    {
        switch (type) {
        case 0:
            return OC::QualityOfService::LowQos;
        case 1:
            return OC::QualityOfService::MidQos;
        case 2:
            return OC::QualityOfService::HighQos;
        case 3:
            return OC::QualityOfService::NaQos;
        default:
            ThrowOcException(OC_STACK_INVALID_PARAM, "Unexpected quality of service");
            return OC::QualityOfService::NaQos;
        };
    }

    static OC::ObserveType getObserveType(JNIEnv *env, int type)
    {
        switch (type) {
        case 0:
            return OC::ObserveType::Observe;
        case 1:
            return OC::ObserveType::ObserveAll;
        default:
            ThrowOcException(OC_STACK_INVALID_PARAM, "Unexpected observe type");
            return OC::ObserveType::ObserveAll;
        };
    }

    static OCEntityHandlerResult getOCEntityHandlerResult(JNIEnv *env, int type)
    {
        switch (type) {
        case 0:
            return OCEntityHandlerResult::OC_EH_OK;
        case 1:
            return OCEntityHandlerResult::OC_EH_ERROR;
        case 2:
            return OCEntityHandlerResult::OC_EH_RESOURCE_CREATED;
        case 3:
            return OCEntityHandlerResult::OC_EH_RESOURCE_DELETED;
        case 4:
            return OCEntityHandlerResult::OC_EH_SLOW;
        case 5:
            return OCEntityHandlerResult::OC_EH_FORBIDDEN;
        default:
            ThrowOcException(OC_STACK_INVALID_PARAM, "Unexpected OCEntityHandlerResult");
            return OCEntityHandlerResult::OC_EH_ERROR;
        };
    }

    static OCConnectivityType getConnectivityType(JNIEnv *env, int type)
    {
        switch (type) {
        case 0:
            return OCConnectivityType::OC_IPV4;
        case 1:
            return OCConnectivityType::OC_IPV6;
        case 2:
            return OCConnectivityType::OC_EDR;
        case 3:
            return OCConnectivityType::OC_LE;
        case 4:
            return OCConnectivityType::OC_ALL;
        default:
            ThrowOcException(OC_STACK_INVALID_PARAM, "Unexpected connectivity type");
            return OCConnectivityType::OC_ALL;
        };
    }

    static std::string stackResultToStr(const int result)
    {
        switch (result)
        {
        case OC_STACK_OK:
            return "OK";
        case OC_STACK_RESOURCE_CREATED:
            return "RESOURCE_CREATED";
        case OC_STACK_RESOURCE_DELETED:
            return "RESOURCE_DELETED";
        case OC_STACK_CONTINUE:
            return "CONTINUE";
            /* Success status code - END HERE */
            /* Error status code - START HERE */
        case OC_STACK_INVALID_URI:
            return "INVALID_URI";
        case OC_STACK_INVALID_QUERY:
            return "INVALID_QUERY";
        case OC_STACK_INVALID_IP:
            return "INVALID_IP";

        case OC_STACK_INVALID_PORT:
            return "INVALID_PORT";
        case OC_STACK_INVALID_CALLBACK:
            return "INVALID_CALLBACK";
        case OC_STACK_INVALID_METHOD:
            return "INVALID_METHOD";
        case OC_STACK_INVALID_PARAM:
            return "INVALID_PARAM";
        case OC_STACK_INVALID_OBSERVE_PARAM:
            return "INVALID_OBSERVE_PARAM";
        case OC_STACK_NO_MEMORY:
            return "NO_MEMORY";
        case OC_STACK_COMM_ERROR:
            return "COMM_ERROR";
        case OC_STACK_NOTIMPL:
            return "NOTIMPL";
        case OC_STACK_NO_RESOURCE:
            return "NO_RESOURCE";
        case  OC_STACK_RESOURCE_ERROR:
            return "RESOURCE_ERROR";
        case OC_STACK_SLOW_RESOURCE:
            return "SLOW_RESOURCE";
            //case OC_STACK_DUPLICATE_REQUEST:
            //    return "DUPLICATE_REQUEST";
        case OC_STACK_NO_OBSERVERS:
            return "NO_OBSERVERS";
        case OC_STACK_OBSERVER_NOT_FOUND:
            return "OBSERVER_NOT_FOUND";
        case OC_STACK_VIRTUAL_DO_NOT_HANDLE:
            return "VIRTUAL_DO_NOT_HANDLE";
        case OC_STACK_INVALID_OPTION:
            return "INVALID_OPTION";
        case OC_STACK_MALFORMED_RESPONSE:
            return "MALFORMED_RESPONSE";
        case OC_STACK_PERSISTENT_BUFFER_REQUIRED:
            return "PERSISTENT_BUFFER_REQUIRED";
        case OC_STACK_INVALID_REQUEST_HANDLE:
            return "INVALID_REQUEST_HANDLE";
        case OC_STACK_INVALID_DEVICE_INFO:
            return "INVALID_DEVICE_INFO";
            //case OC_STACK_INVALID_JSON:
            //    return "INVALID_JSON";

        case OC_STACK_PRESENCE_STOPPED:
            return "PRESENCE_STOPPED";
        case OC_STACK_PRESENCE_TIMEOUT:
            return "PRESENCE_TIMEOUT";
        case OC_STACK_PRESENCE_DO_NOT_HANDLE:
            return "PRESENCE_DO_NOT_HANDLE";

        case OC_STACK_ERROR:
            return "ERROR";

        case JNI_EXCEPTION:
            return "JNI_EXCEPTION";
        case JNI_NO_NATIVE_POINTER:
            return "JNI_NO_NATIVE_POINTER";
        case JNI_INVALID_VALUE:
            return "JNI_INVALID_VALUE";
        default:
            return "";
        }
    }
};
