
const char* decode_oc_stack_result (OCStackResult result)
{
    switch (result)
    {
        case OC_STACK_OK:
            return "OC_STACK_OK";
        case OC_STACK_RESOURCE_DELETED:
            return "OC_STACK_RESOURCE_DELETED";
        case OC_STACK_CONTINUE:
            return "OC_STACK_CONTINUE";
        case OC_STACK_RESOURCE_CREATED:
            return "OC_STACK_RESOURCE_CREATED";
        case OC_STACK_RESOURCE_CHANGED:
            return "OC_STACK_RESOURCE_CHANGED";
        case OC_STACK_INVALID_URI:
            return "OC_STACK_INVALID_URI";
        case OC_STACK_INVALID_QUERY:
            return "OC_STACK_INVALID_QUERY";
        case OC_STACK_INVALID_IP:
            return "OC_STACK_INVALID_IP";
        case OC_STACK_INVALID_PORT:
            return "OC_STACK_INVALID_PORT";
        case OC_STACK_INVALID_CALLBACK:
            return "OC_STACK_INVALID_CALLBACK";
        case OC_STACK_INVALID_METHOD:
            return "OC_STACK_INVALID_METHOD";
        case OC_STACK_INVALID_PARAM:
            return "OC_STACK_INVALID_PARAM";
        case OC_STACK_INVALID_OBSERVE_PARAM:
            return "OC_STACK_INVALID_OBSERVE_PARAM";
        case OC_STACK_NO_MEMORY:
            return "OC_STACK_NO_MEMORY";
        case OC_STACK_COMM_ERROR:
            return "OC_STACK_COMM_ERROR";
        case OC_STACK_TIMEOUT:
            return "OC_STACK_TIMEOUT";
        case OC_STACK_ADAPTER_NOT_ENABLED:
            return "OC_STACK_ADAPTER_NOT_ENABLED";
        case OC_STACK_NOTIMPL:
            return "OC_STACK_NOTIMPL";
        case OC_STACK_NO_RESOURCE:
            return "OC_STACK_NO_RESOURCE";
        case OC_STACK_RESOURCE_ERROR:
            return "OC_STACK_RESOURCE_ERROR";
        case OC_STACK_SLOW_RESOURCE:
            return "OC_STACK_SLOW_RESOURCE";
        case OC_STACK_DUPLICATE_REQUEST:
            return "OC_STACK_DUPLICATE_REQUEST";
        case OC_STACK_NO_OBSERVERS:
            return "OC_STACK_NO_OBSERVERS";
        case OC_STACK_OBSERVER_NOT_FOUND:
            return "OC_STACK_OBSERVER_NOT_FOUND";
        case OC_STACK_VIRTUAL_DO_NOT_HANDLE:
            return "OC_STACK_VIRTUAL_DO_NOT_HANDLE";
        case OC_STACK_INVALID_OPTION:
            return "OC_STACK_INVALID_OPTION";
        case OC_STACK_MALFORMED_RESPONSE:
            return "OC_STACK_MALFORMED_RESPONSE";
        case OC_STACK_PERSISTENT_BUFFER_REQUIRED:
            return "OC_STACK_PERSISTENT_BUFFER_REQUIRED";
        case OC_STACK_INVALID_REQUEST_HANDLE:
            return "OC_STACK_INVALID_REQUEST_HANDLE";
        case OC_STACK_INVALID_DEVICE_INFO:
            return "OC_STACK_INVALID_DEVICE_INFO";
        case OC_STACK_INVALID_JSON:
            return "OC_STACK_INVALID_JSON";
        case OC_STACK_UNAUTHORIZED_REQ:
            return "OC_STACK_UNAUTHORIZED_REQ";
        case OC_STACK_PDM_IS_NOT_INITIALIZED:
            return "OC_STACK_PDM_IS_NOT_INITIALIZED";
        case OC_STACK_DUPLICATE_UUID:
            return "OC_STACK_DUPLICATE_UUID";
        case OC_STACK_INCONSISTENT_DB:
            return "OC_STACK_INCONSISTENT_DB";
        case OC_STACK_AUTHENTICATION_FAILURE:
            return "OC_STACK_AUTHENTICATION_FAILURE";
        case OC_STACK_ERROR:
            return "OC_STACK_ERROR";
        case OC_STACK_TOO_LARGE_REQ:
            return "OC_STACK_TOO_LARGE_REQ";
        case OC_STACK_NOT_ALLOWED_OXM:
            return "OC_STACK_NOT_ALLOWED_OXM";
        case OC_STACK_BAD_ENDPOINT:
            return "OC_STACK_BAD_ENDPOINT";
        case OC_STACK_USER_DENIED_REQ:
            return "OC_STACK_USER_DENIED_REQ";
        case OC_STACK_NOT_ACCEPTABLE:
            return "OC_STACK_NOT_ACCEPTABLE";
        case OC_STACK_FORBIDDEN_REQ:
            return "OC_STACK_FORBIDDEN_REQ";
        case OC_STACK_INTERNAL_SERVER_ERROR:
            return "OC_STACK_INTERNAL_SERVER_ERROR";
        case OC_STACK_GATEWAY_TIMEOUT:
            return "OC_STACK_GATEWAY_TIMEOUT";
        case OC_STACK_SERVICE_UNAVAILABLE:
            return "OC_STACK_SERVICE_UNAVAILABLE";
    #ifdef WITH_PRESENCE
        case OC_STACK_PRESENCE_STOPPED:
            return "OC_STACK_PRESENCE_STOPPED";
        case OC_STACK_PRESENCE_TIMEOUT:
            return "OC_STACK_PRESENCE_TIMEOUT";
        case OC_STACK_PRESENCE_DO_NOT_HANDLE:
            return "OC_STACK_PRESENCE_DO_NOT_HANDLE";
    #endif
        default:
            return "Unknown result!";
    }
}

const char* decode_oc_eh_result (OCEntityHandlerResult result)
{
    switch(result)
    {
        case OC_EH_OK:
            return "OC_EH_OK";
        case OC_EH_ERROR:
            return "OC_EH_ERROR";
        case OC_EH_SLOW:
            return "OC_EH_SLOW";
        case OC_EH_RESOURCE_CREATED:
            return "OC_EH_RESOURCE_CREATED";
        case OC_EH_RESOURCE_DELETED:
            return "OC_EH_RESOURCE_DELETED";
        case OC_EH_VALID:
            return "OC_EH_VALID";
        case OC_EH_CHANGED:
            return "OC_EH_CHANGED";
        case OC_EH_CONTENT:
            return "OC_EH_CONTENT";
        case OC_EH_BAD_REQ:
            return "OC_EH_BAD_REQ";
        case OC_EH_UNAUTHORIZED_REQ:
            return "OC_EH_UNAUTHORIZED_REQ";
        case OC_EH_BAD_OPT:
            return "OC_EH_BAD_OPT";
        case OC_EH_FORBIDDEN:
            return "OC_EH_FORBIDDEN";
        case OC_EH_RESOURCE_NOT_FOUND:
            return "OC_EH_RESOURCE_NOT_FOUND";
        case OC_EH_METHOD_NOT_ALLOWED:
            return "OC_EH_METHOD_NOT_ALLOWED";
        case OC_EH_NOT_ACCEPTABLE:
            return "OC_EH_NOT_ACCEPTABLE";
        case OC_EH_TOO_LARGE:
            return "OC_EH_TOO_LARGE";
        case OC_EH_UNSUPPORTED_MEDIA_TYPE:
            return "OC_EH_UNSUPPORTED_MEDIA_TYPE";
        case OC_EH_INTERNAL_SERVER_ERROR:
            return "OC_EH_INTERNAL_SERVER_ERROR";
        case OC_EH_BAD_GATEWAY:
            return "OC_EH_BAD_GATEWAY";
        case OC_EH_SERVICE_UNAVAILABLE:
            return "OC_EH_SERVICE_UNAVAILABLE";
        case OC_EH_RETRANSMIT_TIMEOUT:
            return "OC_EH_RETRANSMIT_TIMEOUT";
        default:
            return "Unknown result!";
    }
}

const char* decode_oc_method (OCMethod method)
{
    switch(method)
    {
        case OC_REST_NOMETHOD:
            return "OC_REST_NOMETHOD";
        case OC_REST_GET:
            return "OC_REST_GET";
        case OC_REST_PUT:
            return "OC_REST_PUT";
        case OC_REST_POST:
            return "OC_REST_POST";
        case OC_REST_DELETE:
            return "OC_REST_DELETE";
        case OC_REST_OBSERVE:
            return "OC_REST_OBSERVE";
        case OC_REST_OBSERVE_ALL:
            return "OC_REST_OBSERVE_ALL";
        case OC_REST_DISCOVER:
            return "OC_REST_DISCOVER";
    #ifdef WITH_PRESENCE
        case OC_REST_PRESENCE:
            return "OC_REST_PRESENCE";
    #endif
        default:
            return "Unknown method";
    }
}

const char* decode_oc_eh_flag (OCEntityHandlerFlag flag)
{
    switch(flag)
    {
        case OC_REQUEST_FLAG:
            return "OC_REQUEST_FLAG";
        case OC_OBSERVE_FLAG:
            return "OC_OBSERVE_FLAG";
        default:
            return "Unknown oc eh flag";
    }
}
