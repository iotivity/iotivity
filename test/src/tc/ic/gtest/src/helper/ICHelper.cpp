/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include "ICHelper.h"

std::mutex ICHelper::s_Mutex;
ICHelper* ICHelper::s_ICHelper = NULL;

string ICHelper::s_Uid = "";
string ICHelper::s_AccessToken = "";
string ICHelper::s_GroupID = "";
string ICHelper::s_GroupType = "";
string ICHelper::s_GName = "";
string ICHelper::s_Member = "";
string ICHelper::s_GroupUUID = "";
string ICHelper::s_SearchQuery = "/oic/res?di=";
bool ICHelper::s_IsGetSuccessful;
bool ICHelper::s_IsObserveSuccessful;
bool ICHelper::s_IsDeleteSuccessful;
bool ICHelper::s_IsPublishSuccessful;
bool ICHelper::s_Pub;
bool ICHelper::s_Sub;
std::vector< std::string > ICHelper::s_DeviceID =
{ };

bool ICHelper::s_IsServerResponsed = false;
bool ICHelper::s_IsSignedUp = false;
static bool ICHelper::s_IsPostRequestSuccess = false;

ResourceHandles ICHelper::s_ResourceHandles;
OC::OCResource::Ptr ICHelper::s_binaryswitchResource;
OCResourceHandle m_resourceHandles2;

ICHelper::ICHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[ICHelper] IN");
}

FILE* controllerOpen(const char *path, const char *mode)
{
    if (0 == strncmp(path, OC_SECURITY_DB_DAT_FILE_NAME, strlen(OC_SECURITY_DB_DAT_FILE_NAME)))
    {
        return fopen(path, mode);
    }
    else
    {
        return fopen(CLOUD_CLIENT_DAT, mode);
    }
}

bool ICHelper::initCloudClient()
{
    __FUNC_IN__

    static OCPersistentStorage pstStr;
    pstStr.open = controllerOpen;
    pstStr.read = fread;
    pstStr.write = fwrite;
    pstStr.close = fclose;
    pstStr.unlink = unlink;

    if (OC_STACK_OK != OCRegisterPersistentStorageHandler(&pstStr))
    {
        IOTIVITYTEST_LOG(ERROR, "[ICHelper] OCRegisterPersistentStorageHandler error");
        return false;
    }

    // initialize OC stack and provisioning manager
    if (OC_STACK_OK != OCInit(NULL, 0, OC_CLIENT_SERVER))
    {
        IOTIVITYTEST_LOG(ERROR, "[ICHelper] OCStack init error");
        return false;
    }
    __FUNC_OUT__
    return true;
}

ICHelper* ICHelper::getInstance(void)
{
    if (s_ICHelper == NULL)
    {
        s_Mutex.lock();

        s_ICHelper = new ICHelper();

        s_Mutex.unlock();
    }

    return s_ICHelper;
}

ICHelper::~ICHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[ICHelper] OUT");
    if (s_ICHelper != NULL)
    {
        s_ICHelper = NULL;
    }
}

void ICHelper::waitForServerResponse()
{
    s_IsServerResponsed = false;

    void* ret = NULL;
    pthread_t serverlistiningThread;
    pthread_create(&serverlistiningThread, NULL, executeThread, NULL);
    pthread_join(serverlistiningThread, &ret);
}

void* ICHelper::executeThread(void *arg)
{
    int second = 0;

    IOTIVITYTEST_LOG(INFO,
            "\nWaiting For Server Response..........................................................\n");

    while (!s_IsServerResponsed)
    {
        sleep(1);
        if (++second == CALLBACK_TIMEOUT)
        {
            IOTIVITYTEST_LOG(INFO, "\nTimeout For Server Response!Please Try Again");
            break;
        }
    }

    pthread_exit(NULL);
}

FILE* ICHelper::readResourceDATFile(const char * /*path*/, const char *mode)
{
    return fopen(RESOURCE_SERVER_DAT, mode);
}

void ICHelper::icPrintRepresentation(OCRepresentation rep)
{
    for (auto itr = rep.begin(); itr != rep.end(); ++itr)
    {
        IOTIVITYTEST_LOG(INFO, "%s \t : %s \t", itr->attrname().c_str(),
                itr->getValueToString().c_str());
        if (itr->type() == AttributeType::Vector)
        {
            switch (itr->base_type())
            {
                case AttributeType::OCRepresentation:
                    for (auto itr2 : (*itr).getValue< vector< OCRepresentation > >())
                    {
                        icPrintRepresentation(itr2);
                    }
                    break;

                case AttributeType::Integer:
                    for (auto itr2 : (*itr).getValue< vector< int > >())
                    {
                        IOTIVITYTEST_LOG(INFO, "\t\t %d", itr2);
                    }
                    break;

                case AttributeType::String:
                    for (auto itr2 : (*itr).getValue< vector< string > >())
                    {
                        IOTIVITYTEST_LOG(INFO, "\t\t%s", itr2.c_str());
                    }
                    break;

                default:
                    IOTIVITYTEST_LOG(INFO, "Unhandled base type %d", itr->base_type());
                    break;
            }
        }
        else if (itr->type() == AttributeType::OCRepresentation)
        {
            icPrintRepresentation((*itr).getValue< OCRepresentation >());
        }
    }
}

bool ICHelper::isResourceRegistered()
{
    try
    {
        OCStackResult ocResult = OC_STACK_ERROR;

        ICAirconditionerResource airConditioner(IC_RESOURCE_URI_AIRCON,
        { IC_RESOURCE_TYPE_AIRCON },
        { DEFAULT_INTERFACE, BATCH_INTERFACE, LINK_INTERFACE });

        string uri = airConditioner.getResourceUri();
        string rt = airConditioner.getResourceType()[0];
        string itf = airConditioner.getInterfaces()[0];
        ocResult = OC_STACK_ERROR;

        ocResult = OCPlatform::registerResource(airConditioner.m_handle, uri, rt, itf,
                bind(&ICAirconditionerResource::entityHandler, &airConditioner, placeholders::_1),
                OC_DISCOVERABLE);

        if (ocResult != OC_STACK_OK)
        {
            IOTIVITYTEST_LOG(INFO, "ICAirconditionerResource registration is unsuccessful");
            return false;
        }

        IOTIVITYTEST_LOG(INFO, "Publishing resources to cloud");

        ResourceHandles resourceHandles;

        OCDeviceInfo devInfoAirConditioner;
        OCStringLL deviceType;

        m_resourceHandles2 = airConditioner.m_handle;
        resourceHandles.push_back(airConditioner.m_handle);
        ICHelper::s_ResourceHandles = resourceHandles;

        ICHelper::s_IsServerResponsed = true;
        return true;
    }
    catch (OCException &ex)
    {
        IOTIVITYTEST_LOG(INFO, "OCException result string : %s", ex.what());
        return false;
    }
}

bool ICHelper::isUnResourceRegistered()
{
    try
    {
        if (!ICHelper::s_ResourceHandles.empty())
        {

            if (OCPlatform::unregisterResource(m_resourceHandles2) == OC_STACK_OK)
            {
                IOTIVITYTEST_LOG(INFO, "resource unregistering success");
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    catch (OCException &ex)
    {
        IOTIVITYTEST_LOG(INFO, "OCException result string : %s", ex.what());
        return false;
    }

}

void ICHelper::init_string(stringstr *str)
{
    str->len = 0;
    str->ptr = malloc(str->len + 1);
    if (str->ptr == NULL)
    {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    str->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, stringstr *str)
{
    size_t new_len = str->len + size * nmemb;
    str->ptr = realloc(str->ptr, new_len + 1);
    if (str->ptr == NULL)
    {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(str->ptr + str->len, ptr, size * nmemb);
    str->ptr[new_len] = '\0';
    str->len = new_len;

    return size * nmemb;
}

char* ICHelper::get_authenticity_token(const char* resposeTxt)
{
    char* authLast = "";
    char* auth_token = strstr(resposeTxt,
            "<input name=\"authenticity_token\" type=\"hidden\" value=\"");
    auth_token = strstr(auth_token, "value=\"");
    auth_token = &auth_token[strlen("value=\"")];
    authLast = strstr(auth_token, "\" />");
    auth_token[(authLast - auth_token)] = '\0';
    return auth_token;
}

char* ICHelper::get_auth_token_code(const char* resposeTxt, char *code)
{
    char* authLast = "";
    char *authcode;
    int len;
    char* auth_token = strstr(resposeTxt, "<a href=\"http://www.example.com/oauth_callback/?code=");
    authLast = strstr(auth_token, "\">");
    auth_token[(authLast - auth_token)] = '\0';
    auth_token = strstr(auth_token, "code=");
    auth_token = strstr(auth_token, "=");
    len = strlen(auth_token);
    memset(code, 0, len);
    strncpy(code, auth_token + 1, len - 1);
    return code;
}

void ICHelper::cloudConnectGetHandler(const HeaderOptions & /*headerOptions*/,
        const OCRepresentation &rep, const int ecode)
{
    IOTIVITYTEST_LOG(INFO, "GET callback is invoked.");
    IOTIVITYTEST_LOG(INFO, " ecode is %d", ecode);
    ICHelper::icPrintRepresentation(rep);
    ICHelper::s_GroupID = rep.getValueToString("gid");
    if (ecode == OC_STACK_OK || ecode == OC_STACK_RESOURCE_CHANGED)
    {
        IOTIVITYTEST_LOG(INFO, "ecode = %d", ecode);
        ICHelper::s_IsGetSuccessful = true;

        ICHelper::s_GroupType = rep.getValueToString("gtype");
        ICHelper::s_GroupID = rep.getValueToString("gid");
        ICHelper::s_GName = rep.getValueToString("gname");
        ICHelper::s_GroupUUID = rep.getValueToString("owner");
        ICHelper::s_Member = rep.getValueToString("members");
    }
    else
    {
        ICHelper::s_IsGetSuccessful = false;
        IOTIVITYTEST_LOG(INFO, "GET callback is Fail. ecode = %d", ecode);
    }

}

void ICHelper::cloudConnectObserveHandler(const HeaderOptions /*headerOptions*/,
        const OCRepresentation &rep, const int &eCode, const int &sequenceNumber)
{

    if (eCode == OC_STACK_OK && sequenceNumber != maxSequenceNumber + 1)
    {
        if (sequenceNumber == OC_OBSERVE_REGISTER)
        {
            ICHelper::s_IsObserveSuccessful = true;
            IOTIVITYTEST_LOG(INFO, "Observe registration action is successful");
        }

        IOTIVITYTEST_LOG(INFO, "OBSERVE RESULT:");
        ICHelper::icPrintRepresentation(rep);
    }
    else
    {
        if (eCode == OC_STACK_OK)
        {
            ICHelper::s_IsObserveSuccessful = true;

            IOTIVITYTEST_LOG(INFO, "Observe callback is Successful.");
        }
        else
        {
            IOTIVITYTEST_LOG(INFO, "Observe callback is Fail.");
        }
    }
}

void ICHelper::onDeleteHandler(const HeaderOptions & /*headerOptions*/, const int eCode)
{
    if (eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_DELETED)
    {
        IOTIVITYTEST_LOG(INFO, "Delete was successful:");
        ICHelper::s_IsDeleteSuccessful = true;
    }
    else
    {
        IOTIVITYTEST_LOG(INFO, "ecode = %d", eCode);
        IOTIVITYTEST_LOG(INFO, "Delete Response :");
        ICHelper::s_IsDeleteSuccessful = true;
    }
}

void ICHelper::createTopicCB(const int ecode, const string &originUri,
        shared_ptr< OC::OCResource > topic)
{
    IOTIVITYTEST_LOG(INFO, "Create topic response received, code:  %d", ecode);

    if (ecode == OCStackResult::OC_STACK_RESOURCE_CREATED)
    {
        IOTIVITYTEST_LOG(INFO, "Created topic : %s", topic->uri().c_str());
    }
    else
    {
        IOTIVITYTEST_LOG(INFO, "Topic creation failed : %s", originUri.c_str());
    }
}

void ICHelper::onPublish(const OCRepresentation &, const int &eCode)
{
    ICHelper::s_IsPublishSuccessful = true;
    if (eCode == 4)
    {
        IOTIVITYTEST_LOG(INFO, "onPublish callback is invoked with received code : %d", eCode);
    }

}

void ICHelper::onDelete(const int& eCode)
{
    ICHelper::s_IsDeleteSuccessful = true;
    IOTIVITYTEST_LOG(INFO, "onDelete callback is invoked with received code : %d", eCode);
}

void ICHelper::foundDevice(shared_ptr< OC::OCResource > resource)
{
    IOTIVITYTEST_LOG(INFO, "Found device called!");

    vector< string > rt = resource->getResourceTypes();

    IOTIVITYTEST_LOG(INFO, "Device found: %s", resource->uri().c_str());
    IOTIVITYTEST_LOG(INFO, "DI: %s", resource->sid().c_str());

    for (auto it = rt.begin(); it != rt.end(); it++)
    {
        if (it->compare("oic.d.airconditioner") == 0)
        {
            s_SearchQuery += resource->sid();
            IOTIVITYTEST_LOG(INFO, "Airconditioner found");
            OCPlatform::findResource(COAP_HOST_ADDRESS, s_SearchQuery,
                    static_cast< OCConnectivityType >(CT_ADAPTER_TCP | CT_IP_USE_V4),
                    &ICAirconditionerResource::foundAirconditionerResource);

            OCPlatform::OCPresenceHandle handle;
            if (OCPlatform::subscribeDevicePresence(handle, COAP_HOST_ADDRESS,
            { resource->sid() }, static_cast< OCConnectivityType >(CT_ADAPTER_TCP | CT_IP_USE_V4),
                    &ICAirconditionerResource::onObserve) != OC_STACK_OK)
            {
                IOTIVITYTEST_LOG(INFO, "Device presence failed");
            }
            if(OCPlatform::unsubscribePresence(handle)!= OC_STACK_OK)
            {
                ICHelper::s_Pub = true;
                IOTIVITYTEST_LOG(INFO, "Device unsubscribe presence failed");
            }
        }
    }
}

void ICHelper::errorFoundDevice(const std::string &uri, const int ecode)
{
    IOTIVITYTEST_LOG(INFO, "Found device error on %s  code %d ", uri.c_str(), ecode);
}

void ICHelper::deleteResponse(const HeaderOptions&, const int)
{
}

