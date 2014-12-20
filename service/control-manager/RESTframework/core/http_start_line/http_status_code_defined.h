/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef WEBSERVICE_HTTP_STATUS_CODE_DEFINED_H_
#define WEBSERVICE_HTTP_STATUS_CODE_DEFINED_H_

#include <string>

namespace webservice
{

    typedef enum
    {
        kHttpStatusCodeProcessing = 100,
        kHttpStatusCodeSuccessOk = 200,
        kHttpStatusCodeSuccessCreated = 201,
        kHttpStatusCodeAccepted = 202,
        kHttpStatusCodeSuccessNoContent = 204,
        kHttpStatusCodeSuccessPartialContent = 206,
        kHttpStatusCodeRedirectSpecialResponse = 300,
        kHttpStatusCodeRedirectMovedPermanently = 301,
        kHttpStatusCodeRedirectMovedTemporarily = 302,
        kHttpStatusCodeRedirectSeeOther = 303,
        kHttpStatusCodeRedirectNotModified = 304,
        kHttpStatusCodeRedirectTemporaryRedirect = 307,
        kHttpStatusCodeErrClientBadRequest = 400,
        kHttpStatusCodeErrClientUnauthorised = 401,
        kHttpStatusCodeErrClientForbidden = 403,
        kHttpStatusCodeErrClientNotFound = 404,
        kHttpStatusCodeErrClientNotAllowed = 405,
        kHttpStatusCodeErrClientNotAcceptable = 406,
        kHttpStatusCodeErrClientRequestTimeOut = 408,
        kHttpStatusCodeErrClientConflict = 409,
        kHttpStatusCodeErrClientLengthRequired = 411,
        kHttpStatusCodeErrClientPreconditionFailed = 412,
        kHttpStatusCodeErrClientRequestEntityTooLarge = 413,
        kHttpStatusCodeErrClientRequestUriTooLarge = 414,
        kHttpStatusCodeErrClientUnsupportedMediaType = 415,
        kHttpStatusCodeErrClientRangeNotSatisfiable = 416,
        kHttpStatusCodeErrServerInternalServerError = 500,
        kHttpStatusCodeErrServerNotImplemented = 501,
        kHttpStatusCodeErrServerBadGateway = 502,
        kHttpStatusCodeErrServerServiceUnavailable = 503,
        kHttpStatusCodeErrServerGatewayTimeOut = 504,
        kHttpStatusCodeErrServerInsufficientStorage = 507,
    } HttpStatusCode;

// 201
    const char *const kCreated                    =   "Created";

// 202
    const char *const kAccepted                   =   "Accepted";

// 204
    const char *const kNoContentNoBody            =
        "No Content, The server can not find any body data.";
    const char *const kNoContent                  =   "No Content";

// 304
    const char *const kNotModified                =   "Not Modified";

// 400
    const char *const kBadRequest                 =   "Bad Request";
    const char *const kBadRequestNotPublic        =
        "Bad Request, The attribute is not allowed to write";
    const char *const kBadRequestRestrictionValue =   "Bad Request, The attribute is restriction value";
    const char *const kBadRequestInvalidBody      =
        "Bad Request, Some attribute/object is not allowed";
    const char *const kBadRequestIDCreationFail   =   "Bad Request, ID Creation fail";
    const char *const kBadRequestBodySizeIsZero   =   "Bad Request, Request body size is zero!";
    const char *const kBadRequestIncorrectBody    =   "Bad Request, Can not parse request body!";
    const char *const kBadRequestDeviceFail       =   "Bad Request, Device Fail";
    const char *const kBadRequestAccountFail      =   "Bad Request, Account Fail";
    const char *const kBadRequestNotReady         =   "Bad Request, This resource is not ready!";
    const char *const kBadRequestNotAllowedDevice =   "Bad Request, Not Allowed Device";
    const char *const kBadRequestIncorrectJsonKey =
        "Bad Request, Please check your request body, root key is incorrect!";
    const char *const kBadRequestIncorrectType    =   "Bad Request, Incorrect Device Type";
    const char *const kBadRequestJSONBody         =
        "Bad Request, Please check Request body, is it JSON type?";
    const char *const kBadRequestIncorrectField   =   "Bad Request, Some field does NOT meet SHP spec!";
    const char *const kBadRequestXAPIVersion      =   "Bad Request, You need checking X-API-Version!";
    const char *const kBadRequestXAPISessionID    =   "Bad Request, You need checking X-API-SessionId!";
    const char *const kBadRequestNoResources      =   "Bad Request, Device has no resources!";
    const char *const kBadRequestAccountNetwork   =
        "Bad Request, Login fail, related network connection";
    const char *const kBadRequestLoginFail        =   "Bad Request, Invalid auth code!";
    const char *const kBadRequestInvalidSessionId =   "Bad Request, Invalid SessionId!";
    const char *const kBadRequestMaxLimitExceeded =   "Bad Request, Action: Max limit exceeded!";
    const char *const kBadRequestXAPIVerMismatch  =   "Bad Request, X-API Version mismatch!";

// 404
    const char *const kNotFound                     =   "Not Found";
    const char *const kNotFoundObjectIDManager      =   "Not Found, ID Validator";
    const char *const kNotFoundObjectIDList         =   "Not Found, ID List";
    const char *const kNotFoundURI                  =   "Not Found, You need checking request URI!";
    const char *const kNotFoundAccountID            =   "Not Found, Account ID";
    const char *const kNotFoundDeviceID             =   "Not Found, Device ID";
    const char *const kNotFoundAccessPointID        =   "Not Found, AccessPoint ID";
    const char *const kNotFoundZoneID               =   "Not Found, Zone ID";
    const char *const kNotFoundSubDeviceID          =   "Not Found, Sub Device ID";
    const char *const kNotFoundCamera               =   "Not Found, Camera";
    const char *const kNotFoundAVSourceID           =   "Not Found, AVSource ID";
    const char *const kNotFoundCaptureID            =   "Not Found, Capture ID";
    const char *const kNotFoundTemperatureID        =   "Not Found, Temperature ID";
    const char *const kNotFoundDoorID               =   "Not Found, Door ID";
    const char *const kNotFoundVersionID            =   "Not Found, Version ID";
    const char *const kNotFoundAlarmID              =   "Not Found, Alarm ID";
    const char *const kNotFoundModeID               =   "Not Found, Mode ID";
    const char *const kNotFoundGroupID              =   "Not Found, Group ID";
    const char *const kNotFoundActionID             =   "Not Found, Action ID";
    const char *const kNotFoundNetworkID            =   "Not Found, Network ID";
    const char *const kNotFoundPeerID               =   "Not Found, Peer ID in request header";
    const char *const kNotFoundServiceOutDict       =   "Not Found, Service Output data";
    const char *const kNotFoundServiceFail          =
        "Not Found, The server fails to set your requested data!";
    const char *const kNotFoundSubscription         =   "Not Found, subscription!";
    const char *const kNotFoundUUID                 =   "Not Found, Client UUID";
    const char *const kNoSubscriptions              =   "Not Found, any subscriptions";
    const char *const kNoSubscriptionsFoundUUID     =
        "Not Found, active subscriptions pertaing to cleint UUID";
    const char *const kNoSubscriptionsFoundSession  =
        "Not Found, active subscriptions pertaing to cleint session";
    const char *const kNotFoundSessionID            =   "Not Found, SessionId";
    const char *const kNotFoundSubID                =   "Not found, Subscription ID";
    const char *const kNotFoundSessionIDorSubID     =   "Not Found, SessionId or SubID";
    const char *const kNotFoundUUIDorSubID          =   "Not Found, UUID or SubID";
    const char *const kNotFoundSubscriptionID       =   "Not Found, SubscriptionID";
    const char *const kNotFoundMandatoryAttribute   =   "Not Found, the attribute in the system";
    const char *const kNotFoundMessageID            =   "Not Found, Message ID";
    const char *const kNotFoundUserUUID             =   "Not Found, User UUID";
    const char *const kNotFoundUsageLimitID         =   "Not Found, Usage Limit ID";

// 405
    const char *const kMethodNotAllowed                 =   "Method Not Allowed";
    const char *const kMethodNotAllowedSHP              =   "Method Not Allowed (SHP)";
    const char *const kMethodNotAllowedInvalidTimeout   =
        "Invalid Timout range (allowed range is 1~30)";
    const char *const kMethodNotAllowedInvalidDuration  =
        "Invalid Duration range (allowed range is 1~30)";

// 406
    const char *const kNotAcceptable              =   "Not Acceptable";
    const char *const kNotAcceptableAccept        =
        "Can NOT return to defined content-type in your request header";

// 408
    const char *const kRequestTimeOut             =   "Request timed out";

// 415
    const char *const kUnsupportedMediaType       =   "Does NOT support your content type.";

// 500
    const char *const kInternalErrorGeneral         =   "Internal Server Error";
    const char *const kInternalErrorConfigData      =   "Internal Server Error, Config Data Fail";
    const char *const kInternalErrorAllocationFail  =   "Internal Server Error, Memory Allocation Fail";
    const char *const kInternalErrorAccountCompFail =
        "Internal Server Error(Account Component Loading failed)";
    const char *const kInternalErrorDeviceCompFail  =
        "Internal Server Error(Device Component Loading failed)";
    const char *const kInternalErrorZoneCompFail    =
        "Internal Server Error(Zone Component Loading failed)";
    const char *const kInternalErrorService         =
        "Internal Server Error(Execute Service is failed)";
    const char *const kInternalErrorMonitoring      =   "Internal Server Error(Mt)";
    const char *const kInternalErrorEnergy          =   "Internal Server Error(E)";
    const char *const kInternalErrorMode            =   "Internal Server Error(Md)";
    const char *const kInternalErrorGroup           =   "Internal Server Error(G)";
    const char *const kInternalErrorAction          =   "Internal Server Error(Act)";
    const char *const kInternalErrorMessage         =   "Internal Server Error(Msg)";
    const char *const kInternalErrorSmartTV         =   "Internal Server Error(T)";
    const char *const kInternalErrorResponse        =   "Internal Server Error(R)";
    const char *const kInternalErrorRegistration    =
        "Internal Server Error, Device Registration Fail";
    const char *const kInternalErrorRemove          =   "Internal Server Error, Device Remove Fail";
    const char *const kInternalErrorDiscovery       =   "Internal Server Error, Device Discovery Fail";
    const char *const kInternalErrorSubscription    =   "Internal Server Error(S)";
    const char *const kInternalErrorGroupCompFail   =
        "Internal Server Error(Group Component Loading failed)";
    const char *const kInternalErrorPairUnpairFail  =
        "Internal Server Error, Please check if the previous request is finished.";
    const char *const kInternalErrorLogManager      =   "Internal Server Error(Log Manager)";

// 501
    const char *const kNotImplemented                   = "Not Implemented";
    const char *const kNotImplementedContentType        =
        "Not Implemented, Can NOT support your content type format";
    const char *const kNotImplementedResourceObject     =
        "Not Implemented, request URI is not implemented";
    const char *const kNotImplementedAttributeParser    =
        "Not Implemented, attribute parser is not implemented";
    const char *const kNotImplementedChildObject        =
        "Not Implemented, Child object does not supported";

// 503
    const char *const kWebserviceUnavailable = "Service unavailable, web service is not up";

// Unknown
    const char *const kUnknownDescription           =   "Unknown Error Type";

// Error codes that are returned from sub function
    typedef enum
    {
        // 200
        RT_200_START = 1,
        RT_200_OK,
        RT_200_END,
        // 201
        RT_201_START,
        RT_201_CREATE,
        RT_201_END,
        // 202
        RT_202_START,
        RT_202_ACCEPTED,
        RT_202_END,
        // 204
        RT_204_START,
        RT_204_NO_CONTENT,
        RT_204_NO_BODY,
        RT_204_END,
        // 300
        RT_300_START,
        RT_300_END,
        // 304
        RT_304_START,
        RT_304_NOT_MODIFIED,
        RT_304_END,
        // 400
        RT_400_START,
        RT_400_BAD_REQUEST,
        RT_400_NOT_PUBLIC,
        RT_400_RESTRICTION_VALUE,
        RT_400_INVALID_BODY,
        RT_400_ID_CREATION_FAIL,
        RT_400_DEVICE_FAIL,
        RT_400_REQUEST_BODY_SIZE_IS_ZERO,
        RT_400_INCORRECT_BODY,
        RT_400_NOT_SUPPORTED_X_API_VERSION,
        RT_400_NOT_SUPPORTED_X_API_SESSION_ID,
        RT_400_ACCOUNT_FAIL,
        RT_400_NOT_READY,
        RT_400_NOT_ALLOWED_DEVICE,
        RT_400_INCORRECT_JSON_KEY,
        RT_400_INCORRECT_DEVICE_TYPE,
        RT_400_INCORRECT_JSON_TYPE,
        RT_400_INCORRECT_FIELD,
        RT_400_INCORRECT_RESOURCES,
        RT_400_ACCOUNT_NETWORK_CONNECTION,
        RT_400_LOGIN_FAIL,
        RT_400_INVALID_SESSION_ID,
        RT_400_XAPI_VER_MISMATCH,
        RT_400_INCORRECT_ENUM_TYPE,
        RT_400_PAIRING_FAIL,
        RT_400_ACTION_MANAGER,
        RT_400_END,
        // 404
        RT_404_START,
        RT_404_NOT_FOUND,
        RT_404_NOT_FOUND_OBJECT_ID_MANAGER,
        RT_404_NOT_FOUND_OBJECT_ID_LIST,
        RT_404_NOT_FOUND_URI,
        RT_404_ACCOUNT_ID,
        RT_404_DEVICE_ID,
        RT_404_ACCESSPOINT_ID,
        RT_404_ZONE_ID,
        RT_404_CAMERA_ID,
        RT_404_AVSOURCE_ID,
        RT_404_CAPTURE_ID,
        RT_404_TEMPERATURE_ID,
        RT_404_DOOR_ID,
        RT_404_VERSION_ID,
        RT_404_ALARM_ID,
        RT_404_MODE_ID,
        RT_404_GROUP_ID,
        RT_404_ACTION_ID,
        RT_404_NETWORK_ID,
        RT_404_SUB_DEVICE_ID,
        RT_404_MESSAGE_ID,
        RT_404_PEER_ID,
        RT_404_SERVICE_OUTDICT,
        RT_404_SERVICE_FAIL,
        RT_404_SUBSCRIPTION,
        RT_404_SUBSCRIPTION_UUID_ID,
        RT_404_NO_SUBSCRIPTION_UUID_ID,
        RT_404_NO_SUBSCRIPTIONS,
        RT_404_NO_SUBSCRIPTION_SESSION_ID,
        RT_404_SUBSCRIPTION_SESSION_ID,
        RT_404_SUBSCRIPTION_SUBID,
        RT_404_SUBSCRIPTION_SESSION_ID_SUBID,
        RT_404_SUBSCRIPTION_UUID_ID_SUBID,
        RT_404_SUBSCRIPTION_MANAGER_SUBID,
        RT_404_MANDATORY_ATTRIBUTE,
        RT_404_USER_UUID,
        RT_404_USAGE_LIMIT_ID,
        RT_404_END,
        // 405
        RT_405_START,
        RT_405_METHOD_NOT_ALLOWED,
        RT_405_SHP,
        RT_405_INVALID_TIMOUT_RANGE,
        RT_405_INVALID_DURATION_RANGE,
        RT_405_END,
        // 406
        RT_406_START,
        RT_406_NOT_ACCEPTABLE,
        RT_406_NOT_ACCEPTABLE_ACCEPT,
        RT_406_END,
        // 408
        RT_408_START,
        RT_408_REQUEST_TIMEOUT,
        RT_408_END,
        // 415
        RT_415_START,
        RT_415_UNSUPPORTED_MEDIA_TYPE,
        RT_415_END,
        // 500
        RT_500_START,
        RT_500_INTERNAL_SERVER_ERROR,
        RT_500_ALLOC_FAIL,
        RT_500_ACCOUNT_COMP_FAIL,
        RT_500_DEVICE_COMP_FAIL,
        RT_500_ZONE_COMP_FAIL,
        RT_500_CONFIG_FILE_LOAD_FAIL,
        RT_500_SERVICE_MANAGER,
        RT_500_MONITORING_MANAGER,
        RT_500_ENERGY_MANAGER,
        RT_500_MODE_MANAGER,
        RT_500_GROUP_MANAGER,
        RT_500_ACTION_MANAGER,
        RT_500_MESSAGE_MANAGER,
        RT_500_GROUP_COMP_FAIL,
        RT_500_DEVICE_REGISTRATION_FAIL,
        RT_500_DEVICE_REMOVE_FAIL,
        RT_500_DEVICE_ATTRIBUTE_FAIL,
        RT_500_DEVICE_DISCOVERY_FAIL,
        RT_500_SUBSCRIPTION_MANAGER,
        RT_500_LOG_MANAGER,
        RT_500_SMART_TV_MANAGER,
        RT_500_END,
        // 501
        RT_501_START,
        RT_501_NOT_IMPLEMENTED,
        RT_501_CONTENT_TYPE,
        RT_501_RESOURCE_OBJECT,
        RT_501_ATTRIBUTE_PARSER,
        RT_501_CHILD_OBJECT,
        RT_501_END,
        // 503
        RT_503_START,
        RT_503_SERVICE_UNAVAILABLE,
        RT_503_END,
        // 100
        RT_100_START,
        RT_100_CONTINUE,
        RT_100_END,
        RT_UNKNOWN,
        RT_IGNORE,
        RT_MAX
    } HTTPResponseType;

    typedef struct _stResponseType
    {
        HTTPResponseType response_type;
        std::string response_description;
    } stResponseType;

    /* mapped table between response code and description */
    static stResponseType response_type_description_tbl_[] =
    {
        {RT_200_OK, kInternalErrorResponse},

        {RT_201_CREATE, kCreated},

        {RT_202_ACCEPTED, kAccepted},

        {RT_204_NO_CONTENT, kNoContent},
        {RT_204_NO_BODY,    kNoContentNoBody},

        {RT_304_NOT_MODIFIED, kNotModified},

        {RT_400_BAD_REQUEST,                    kBadRequest},
        {RT_400_NOT_PUBLIC,                     kBadRequestNotPublic},
        {RT_400_RESTRICTION_VALUE,              kBadRequestRestrictionValue},
        {RT_400_INVALID_BODY,                   kBadRequestInvalidBody},
        {RT_400_ID_CREATION_FAIL,               kBadRequestIDCreationFail},
        {RT_400_REQUEST_BODY_SIZE_IS_ZERO,      kBadRequestBodySizeIsZero},
        {RT_400_INCORRECT_BODY,                 kBadRequestIncorrectBody},
        {RT_400_NOT_SUPPORTED_X_API_VERSION,    kBadRequestXAPIVersion},
        {RT_400_NOT_SUPPORTED_X_API_SESSION_ID, kBadRequestXAPISessionID},
        {RT_400_ACCOUNT_FAIL,                   kBadRequestAccountFail},
        {RT_400_DEVICE_FAIL,                    kBadRequestDeviceFail},
        {RT_400_NOT_READY,                      kBadRequestNotReady},
        {RT_400_NOT_ALLOWED_DEVICE,             kBadRequestNotAllowedDevice},
        {RT_400_INCORRECT_JSON_KEY,             kBadRequestIncorrectJsonKey},
        {RT_400_INCORRECT_DEVICE_TYPE,          kBadRequestIncorrectType},
        {RT_400_INCORRECT_JSON_TYPE,            kBadRequestJSONBody},
        {RT_400_INCORRECT_FIELD,                kBadRequestIncorrectField},
        {RT_400_INCORRECT_RESOURCES,            kBadRequestNoResources},
        {RT_400_ACCOUNT_NETWORK_CONNECTION,     kBadRequestAccountNetwork},
        {RT_400_LOGIN_FAIL,                     kBadRequestLoginFail},
        {RT_400_INVALID_SESSION_ID,             kBadRequestInvalidSessionId},
        {RT_400_XAPI_VER_MISMATCH,              kBadRequestXAPIVerMismatch},
        {RT_400_INCORRECT_ENUM_TYPE,            kUnknownDescription},
        {RT_400_PAIRING_FAIL,                   kUnknownDescription},
        {RT_400_ACTION_MANAGER,                 kBadRequestMaxLimitExceeded},

        {RT_404_NOT_FOUND,                      kNotFound},
        {RT_404_NOT_FOUND_OBJECT_ID_MANAGER,    kNotFoundObjectIDManager},
        {RT_404_NOT_FOUND_OBJECT_ID_LIST,       kNotFoundObjectIDList},
        {RT_404_NOT_FOUND_URI,                  kNotFoundURI},
        {RT_404_ACCOUNT_ID,                     kNotFoundAccountID},
        {RT_404_DEVICE_ID,                      kNotFoundDeviceID},
        {RT_404_ACCESSPOINT_ID,                 kNotFoundAccessPointID},
        {RT_404_ZONE_ID,                        kNotFoundZoneID},
        {RT_404_CAMERA_ID,                      kNotFoundCamera},
        {RT_404_AVSOURCE_ID,                    kNotFoundAVSourceID},
        {RT_404_CAPTURE_ID,                     kNotFoundCaptureID},
        {RT_404_TEMPERATURE_ID,                 kNotFoundTemperatureID},
        {RT_404_DOOR_ID,                        kNotFoundDoorID},
        {RT_404_VERSION_ID,                     kNotFoundVersionID},
        {RT_404_ALARM_ID,                       kNotFoundAlarmID},
        {RT_404_MODE_ID,                        kNotFoundModeID},
        {RT_404_GROUP_ID,                       kNotFoundGroupID},
        {RT_404_ACTION_ID,                      kNotFoundActionID},
        {RT_404_NETWORK_ID,                     kNotFoundNetworkID},
        {RT_404_SUB_DEVICE_ID,                  kUnknownDescription},
        {RT_404_MESSAGE_ID,                     kNotFoundMessageID},
        {RT_404_PEER_ID,                        kNotFoundPeerID},
        {RT_404_SERVICE_OUTDICT,                kNotFoundServiceOutDict},
        {RT_404_SERVICE_FAIL,                   kNotFoundServiceFail},
        {RT_404_SUBSCRIPTION,                   kNotFoundSubscription},
        {RT_404_SUBSCRIPTION_UUID_ID,           kNotFoundUUID},
        {RT_404_NO_SUBSCRIPTION_UUID_ID,        kNoSubscriptionsFoundUUID},
        {RT_404_NO_SUBSCRIPTIONS,               kNoSubscriptions},
        {RT_404_NO_SUBSCRIPTION_SESSION_ID,     kNoSubscriptionsFoundSession},
        {RT_404_SUBSCRIPTION_SESSION_ID,        kNotFoundSessionID},
        {RT_404_SUBSCRIPTION_SUBID,             kNotFoundSubID},
        {RT_404_SUBSCRIPTION_SESSION_ID_SUBID,  kNotFoundSessionIDorSubID},
        {RT_404_SUBSCRIPTION_UUID_ID_SUBID,     kNotFoundUUIDorSubID},
        {RT_404_SUBSCRIPTION_MANAGER_SUBID,     kUnknownDescription},
        {RT_404_MANDATORY_ATTRIBUTE,            kNotFoundMandatoryAttribute},
        {RT_404_USER_UUID,                      kNotFoundUserUUID},
        {RT_404_USAGE_LIMIT_ID,                 kNotFoundUsageLimitID},

        {RT_405_METHOD_NOT_ALLOWED,     kMethodNotAllowed},
        {RT_405_SHP,                    kMethodNotAllowedSHP},
        {RT_405_INVALID_TIMOUT_RANGE,   kMethodNotAllowedInvalidTimeout},
        {RT_405_INVALID_DURATION_RANGE, kMethodNotAllowedInvalidDuration},

        {RT_406_NOT_ACCEPTABLE,         kNotAcceptable},
        {RT_406_NOT_ACCEPTABLE_ACCEPT,  kNotAcceptableAccept},

        {RT_408_REQUEST_TIMEOUT,  kRequestTimeOut},

        {RT_415_UNSUPPORTED_MEDIA_TYPE, kUnsupportedMediaType},

        {RT_500_INTERNAL_SERVER_ERROR,    kInternalErrorGeneral},
        {RT_500_ALLOC_FAIL,               kInternalErrorAllocationFail},
        {RT_500_ACCOUNT_COMP_FAIL,        kInternalErrorAccountCompFail},
        {RT_500_DEVICE_COMP_FAIL,         kInternalErrorDeviceCompFail},
        {RT_500_ZONE_COMP_FAIL,           kInternalErrorZoneCompFail},
        {RT_500_CONFIG_FILE_LOAD_FAIL,    kInternalErrorConfigData},
        {RT_500_SERVICE_MANAGER,          kInternalErrorService},
        {RT_500_MONITORING_MANAGER,       kInternalErrorMonitoring},
        {RT_500_ENERGY_MANAGER,           kInternalErrorEnergy},
        {RT_500_MODE_MANAGER,             kInternalErrorMode},
        {RT_500_GROUP_MANAGER,            kInternalErrorGroup},
        {RT_500_ACTION_MANAGER,           kInternalErrorAction},
        {RT_500_MESSAGE_MANAGER,          kInternalErrorMessage},
        {RT_500_GROUP_COMP_FAIL,          kInternalErrorGroupCompFail},
        {RT_500_DEVICE_REGISTRATION_FAIL, kInternalErrorRegistration},
        {RT_500_DEVICE_REMOVE_FAIL,       kInternalErrorRemove},
        {RT_500_DEVICE_ATTRIBUTE_FAIL,    kInternalErrorDeviceCompFail},
        {RT_500_DEVICE_DISCOVERY_FAIL,    kInternalErrorDiscovery},
        {RT_500_SUBSCRIPTION_MANAGER,     kInternalErrorSubscription},
        {RT_500_LOG_MANAGER,              kInternalErrorLogManager},
        {RT_500_SMART_TV_MANAGER,         kInternalErrorSmartTV},

        {RT_501_NOT_IMPLEMENTED,      kNotImplemented},
        {RT_501_CONTENT_TYPE,         kNotImplementedContentType},
        {RT_501_RESOURCE_OBJECT,      kNotImplementedResourceObject},
        {RT_501_ATTRIBUTE_PARSER,     kNotImplementedAttributeParser},
        {RT_501_CHILD_OBJECT,         kNotImplementedChildObject},

        {RT_503_SERVICE_UNAVAILABLE, kWebserviceUnavailable},

        {RT_100_CONTINUE, kUnknownDescription},

        {RT_UNKNOWN, kUnknownDescription},

        {RT_MAX, kUnknownDescription}
    };

}

#endif /* WEBSERVICE_HTTP_STATUS_CODE_DEFINED_H_ */

