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

#include "CloudCommonUtil.h"

string CloudCommonUtil::g_authprovider = "github";
string CloudCommonUtil::g_authCode = "";
string CloudCommonUtil::g_uid = "";
string CloudCommonUtil::g_accessToken = "";
Configuration g_cloudPropFile(CLOUD_PROP_FILE);
Configuration g_configPropFile(CONFIG_PROP_FILE);

static int g_isCbInvoked = CALLBACK_NOT_INVOKED;
OCAccountManager::Ptr accountMgr = nullptr;

std::string CloudCommonUtil::getDefaultIp()
{
    string ci_server_ip = "";
#ifdef __LINUX__
    ci_server_ip = g_configPropFile.getProperties(CI_SERVER_ETHERNET_IP_KEY);
#elif __TIZEN__
    ci_server_ip = g_configPropFile.getProperties(CI_SERVER_WIFI_IP_KEY);
#endif

    return ci_server_ip;
}

std::string CloudCommonUtil::getDefaultHostAddess()
{
    string hostAddress = "";

#ifdef __TLS_ON__
    hostAddress = COAP_TLSON_PREFIX;
#else
    hostAddress = COAP_TLSOFF_PREFIX;
#endif

#ifdef __LINUX__
    string ci_server_ip = g_configPropFile.getProperties(CI_SERVER_ETHERNET_IP_KEY);
#elif __TIZEN__
    string ci_server_ip = g_configPropFile.getProperties(CI_SERVER_WIFI_IP_KEY);
#endif

    string local_host = ci_server_ip + ":" + to_string(REMOTE_CI_SERVER_PORT);
    hostAddress += local_host;
    hostAddress.erase(std::remove(hostAddress.begin(), hostAddress.end(), '\n'), hostAddress.end());

    return hostAddress;
}

OCDevAddr CloudCommonUtil::getDefaultEndPoint()
{
    OCDevAddr endPoint;

#ifdef __LINUX__
    string ci_server_ip = g_configPropFile.getProperties(CI_SERVER_ETHERNET_IP_KEY);
#elif __TIZEN__
    string ci_server_ip = g_configPropFile.getProperties(CI_SERVER_WIFI_IP_KEY);
#endif

    memset(&endPoint, 0, sizeof(endPoint));
    strncpy(endPoint.addr, ci_server_ip.c_str(), sizeof(endPoint.addr));
    endPoint.port = OC_MULTICAST_PORT;

    return endPoint;
}

#ifdef __SECURED__
OicCloud_t* CloudCommonUtil::getCloudServer()
{
    OicCloud_t *cloud = (OicCloud_t *)OICCalloc(1,sizeof(OicCloud_t));
    if (NULL == cloud)
    {
        IOTIVITYTEST_LOG(ERROR, "[CSC Helper] cloud calloc failed");
        return NULL;
    }
    cloud->cis = (char *)OICCalloc(1,1024 * 4);
    if (NULL == cloud)
    {
        IOTIVITYTEST_LOG(ERROR, "[CSC Helper] cloud->cis calloc failed");
        OICFree(cloud);
        return NULL;
    }
    cloud->apn = (char *)OICCalloc(1,512);
    if (NULL == cloud->apn)
    {
        IOTIVITYTEST_LOG(ERROR, "[CSC Helper] cloud->apn calloc failed");
        OICFree(cloud->cis);
        OICFree(cloud);
        return NULL;
    }
    cloud->at = (char *)OICCalloc(1,1024);
    if (NULL == cloud->at)
    {
        IOTIVITYTEST_LOG(ERROR, "[CSC Helper] cloud->at calloc failed");
        OICFree(cloud->apn);
        OICFree(cloud->cis);
        OICFree(cloud);
        return NULL;
    }

    string ci_server_ip;

#ifdef __LINUX__
    ci_server_ip = g_configPropFile.getProperties(CI_SERVER_ETHERNET_IP_KEY);
#elif __TIZEN__
    ci_server_ip = g_configPropFile.getProperties(CI_SERVER_WIFI_IP_KEY);
#endif

    ci_server_ip = ci_server_ip + ":" + std::to_string(OC_MULTICAST_PORT);
    strncpy(cloud->cis, ci_server_ip.c_str(), sizeof(cloud->cis));


    return cloud;
}
#endif

std::string CloudCommonUtil::readfile(std::string filename)
{
    __FUNC_IN__
    FILE *fp;
    char buff[100];

    fp = fopen(filename.c_str(), "r");

    if (NULL == fp)
    {
        IOTIVITYTEST_LOG(ERROR, "[CSC Helper] ERROR Opening File : %s", filename.c_str());
    }

    if (NULL == fgets(buff, 100, (FILE*) fp))
    {
        IOTIVITYTEST_LOG(ERROR, "[CSC Helper] Unable to Get input from File: %s", filename.c_str());
    }

    fclose(fp);

    IOTIVITYTEST_LOG(DEBUG, "[CSC Helper] Recieved Data : %s", buff);

    __FUNC_OUT__
    return std::string(buff);
}

void CloudCommonUtil::create_file(string filename, string data)
{
    FILE *fp;
    fp = fopen(filename.c_str(), "w+");
    fprintf(fp, "%s", data.c_str());
    fclose(fp);
}

static int waitCallbackRet()
{
    IOTIVITYTEST_LOG(DEBUG, "Waiting for Callback to be invoked");

    for (int i = 0; CALLBACK_TIMEOUT > i; ++i)
    {
        if (CALLBACK_INVOKED == g_isCbInvoked)
        {
            return CALLBACK_INVOKED;
        }

        if (CALLBACK_CODE_MISMATCH == g_isCbInvoked)
        {
            return CALLBACK_CODE_MISMATCH;
        }

        sleep(DELAY_SHORT);
        printf("Second Elapsed : %d seconds\n", i);

        if (OC_STACK_OK != OCProcess())
        {
            printf("OCStack process error\n");
            return CALLBACK_NOT_INVOKED;
        }
    }

    return CALLBACK_NOT_INVOKED;
}

void CloudCommonUtil::init_string(Stringstr *str)
{
    str->len = 0;
    str->ptr = (char*) malloc(str->len + 1);
    if (str->ptr == NULL)
    {
        fprintf(stderr, "malloc() failed\n");
        exit (EXIT_FAILURE);
    }
    str->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, Stringstr *str)
{
    size_t new_len = str->len + size * nmemb;
    str->ptr = (char*) realloc(str->ptr, new_len + 1);
    if (str->ptr == NULL)
    {
        fprintf(stderr, "realloc() failed\n");
        exit (EXIT_FAILURE);
    }
    memcpy(str->ptr + str->len, ptr, size * nmemb);
    str->ptr[new_len] = '\0';
    str->len = new_len;

    return size * nmemb;
}

char* CloudCommonUtil::get_authenticity_token(const char* responseText)
{
    char* authLast = NULL;
    char* auth_token = (char*) strstr(responseText,
            "<input type=\"hidden\" name=\"authenticity_token\" value=\"");
    auth_token = strstr(auth_token, "value=\"");
    auth_token = &auth_token[strlen("value=\"")];
    authLast = strstr(auth_token, "\" />");
    auth_token[(authLast - auth_token)] = '\0';
    return auth_token;
}

char* CloudCommonUtil::get_auth_token_code(const char* resposeTxt, char *code)
{
    char* authLast = NULL;

    int len;
    char* auth_token = (char*) strstr(resposeTxt,
            "<a href=\"http://www.example.com/oauth_callback/?code=");

    authLast = strstr(auth_token, "\">");
    auth_token[(authLast - auth_token)] = '\0';

    auth_token = strstr(auth_token, "code=");
    auth_token = strstr(auth_token, "=");

    len = strlen(auth_token);
    memset(code, 0, len);
    strncpy(code, auth_token + 1, len - 1);
    return code;
}

char const* CloudCommonUtil::getgithubcode(const char *gitlogin, const char *gitpassword,
        char *code)
{
    char demoPost[1000] =
    { 0 };
    char *auth_text = NULL;
    char *auth_url_text = NULL;
    char *code_text = NULL;
    CURL *curl;
    CURLcode res;

    struct curl_slist *cookies;
    struct curl_slist *nc;

    int i;
    int http_code = 0;
    Stringstr str;
    init_string(&str);

    curl_global_init(CURL_GLOBAL_ALL);

    //get login page
    curl = curl_easy_init();

    if (curl)
    {
        //Get Github login page
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_URL, INIT_GET_LOGIN_URL);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); /* start cookie engine */
        curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1L);

        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL"); // clear all cookies
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
            return "";
        }

        auth_text = get_authenticity_token(str.ptr);
        auth_url_text = curl_easy_escape(curl, auth_text, strlen(auth_text));

        snprintf(demoPost, 1000, "%s%s%s%s%s%s%s", PAR_POST_VAL, AUTHENTICITY_TOKEN, auth_url_text,
                "&login=", gitlogin, "&password=", gitpassword);

        free(str.ptr);

        init_string(&str);
        res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n", curl_easy_strerror(res));
            exit(1);
        }

        //github login
        curl_easy_setopt(curl, CURLOPT_URL, LOGIN_URL);
        curl_easy_setopt(curl, CURLOPT_POST, 1L); /* start cookie engine */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, demoPost);

        nc = cookies, i = 1;
        while (nc)
        {
            curl_easy_setopt(curl, CURLOPT_COOKIELIST, nc->data);
            nc = nc->next;
            i++;
        }

        res = curl_easy_perform(curl);
        res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        IOTIVITYTEST_LOG(INFO, "Resposecode from Github: %d", http_code);
        res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);

        if (res != CURLE_OK)
        {
            fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n", curl_easy_strerror(res));
            exit(1);
        }
        free(str.ptr);

        //request for cloud code
        init_string(&str);
        curl_easy_setopt(curl, CURLOPT_URL, IOTIVITY_CLOUD_URL);
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);
        nc = cookies, i = 1;
        while (nc)
        {
            curl_easy_setopt(curl, CURLOPT_COOKIELIST, nc->data);
            nc = nc->next;
            i++;
        }
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
            return (char*) "";
        }

        code_text = get_auth_token_code(str.ptr, code);

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    free(str.ptr);

    return code_text;
}

/*
 * Get Auth Code From  Github
 */
char* CloudCommonUtil::getGitLoginAuthCodeMain()
{
    __FUNC_IN__
    char *code = (char*) calloc(30, sizeof(char));
    string github_user_id = g_configPropFile.getProperties(GITHUB_USER_KEY);
    string github_password = g_configPropFile.getProperties(GITHUB_PASSWORD_KEY);
    code = (char*) getgithubcode(github_user_id.c_str(), github_password.c_str(), code);
    IOTIVITYTEST_LOG(INFO, "Auth Code: %s", code);
    __FUNC_OUT__
    return code;
}

void CloudCommonUtil::printRepresentation(OCRepresentation rep)
{
    for (auto itr = rep.begin(); itr != rep.end(); ++itr)
    {
        cout << "\t" << itr->attrname() << ":\t" << itr->getValueToString() << endl;

        if (itr->attrname().compare("accesstoken") == 0)
        {
            CloudCommonUtil::g_accessToken = itr->getValueToString();
            g_cloudPropFile.setProperties(CLOUD_ACCESSTOKEN_KEY, CloudCommonUtil::g_accessToken);

        }

        if (itr->attrname().compare("uid") == 0)
        {
            CloudCommonUtil::g_uid = itr->getValueToString();
            g_cloudPropFile.setProperties(CLOUD_UID_KEY, CloudCommonUtil::g_uid);
        }

        if (itr->attrname().compare("aclid") == 0)
        {
            IOTIVITYTEST_LOG(INFO, "ACL ID : %s", itr->getValueToString().c_str());

        }

        if (itr->attrname().compare("aceid") == 0)
        {
            IOTIVITYTEST_LOG(INFO, "ACE ID : %s", itr->getValueToString().c_str());
        }
    }
}

OCRepresentation CloudCommonUtil::parseOCClientResponse(OCClientResponse* clientResponse)
{
    __FUNC_OUT__
    if (nullptr == clientResponse)
    {
        IOTIVITYTEST_LOG(ERROR, "clientResponse is NULL");
        return OCRepresentation();
    }

    if (nullptr == clientResponse->payload
            || PAYLOAD_TYPE_REPRESENTATION != clientResponse->payload->type)
    {
        IOTIVITYTEST_LOG(ERROR, "clientResponse->payload is NULL");
        return OCRepresentation();
    }

    if (PAYLOAD_TYPE_REPRESENTATION != clientResponse->payload->type)
    {
        IOTIVITYTEST_LOG(ERROR, "clientResponse is not of PAYLOAD_TYPE_REPRESENTATION");
        return OCRepresentation();
    }

    MessageContainer oc;
    oc.setPayload(clientResponse->payload);

    std::vector< OCRepresentation >::const_iterator it = oc.representations().begin();
    if (it == oc.representations().end())
    {
        return OCRepresentation();
    }

    // first one is considered the root, everything else is considered a child of this one.
    OCRepresentation root = *it;
    root.setDevAddr(clientResponse->devAddr);
    root.setUri(clientResponse->resourceUri);
    ++it;

    std::for_each(it, oc.representations().end(), [&root](const OCRepresentation& repItr)
    {   root.addChild(repItr);});

    __FUNC_OUT__
    return root;
}

static void handleLoginoutCB(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    IOTIVITYTEST_LOG(INFO, "Auth response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        CloudCommonUtil::printRepresentation(rep);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}

static void signInOutCB(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    IOTIVITYTEST_LOG(INFO, "Auth response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        CloudCommonUtil::printRepresentation(rep);
    }

    g_isCbInvoked =
            (OC_STACK_RESOURCE_CHANGED == ecode) ? CALLBACK_INVOKED : CALLBACK_CODE_MISMATCH;
}

/*
 * Sign Up in Cloud
 */
bool CloudCommonUtil::signUp(OCAccountManager::Ptr accountMgr)
{
    __FUNC_IN__

    char* g_chAuthCode = CloudCommonUtil::getGitLoginAuthCodeMain();
    CloudCommonUtil::g_authCode = string(g_chAuthCode);
    IOTIVITYTEST_LOG(INFO, "[Cloud Common] AuthCode for SignUp %s",
            CloudCommonUtil::g_authCode.c_str());
    OCStackResult result = accountMgr->signUp(CloudCommonUtil::g_authprovider,
            CloudCommonUtil::g_authCode, handleLoginoutCB);

    IOTIVITYTEST_LOG(INFO, "[Cloud Common] signUp returns %s",
            CommonUtil::getOCStackResult(result));

    if (OC_STACK_OK != result)
    {
        IOTIVITYTEST_LOG(ERROR, "[Cloud Common] signUp returns %s",
                CommonUtil::getOCStackResult(result));
        free(g_chAuthCode);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_INVOKED != waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud Common] Callback Not Invoked");
            free(g_chAuthCode);
            return false;
        }
    }
    g_cloudPropFile.save();
    __FUNC_OUT__
    return true;
}

/*
 * Sign Up in Cloud
 */
bool CloudCommonUtil::signUp(OCAccountManager::Ptr accountMgr, std::string authCode)
{
    __FUNC_IN__

    OCStackResult result = accountMgr->signUp(CloudCommonUtil::g_authprovider, authCode.c_str(),
            handleLoginoutCB);

    IOTIVITYTEST_LOG(INFO, "[Cloud Common] signUp returns %s",
            CommonUtil::getOCStackResult(result));

    if (OC_STACK_OK != result)
    {
        IOTIVITYTEST_LOG(ERROR, "[Cloud Common] signUp returns %s",
                CommonUtil::getOCStackResult(result));
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_INVOKED != waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud Common] Callback Not Invoked");
            return false;
        }
    }
    g_cloudPropFile.save();
    __FUNC_OUT__
    return true;
}

/*
 * Sign In in Cloud
 */
bool CloudCommonUtil::signIn(OCAccountManager::Ptr accountMgr)
{
    __FUNC_IN__
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    CloudCommonUtil::g_uid = g_cloudPropFile.getProperties(CLOUD_UID_KEY);
    CloudCommonUtil::g_accessToken = g_cloudPropFile.getProperties(CLOUD_ACCESSTOKEN_KEY);

    OCStackResult result = accountMgr->signIn(CloudCommonUtil::g_uid,
            CloudCommonUtil::g_accessToken, signInOutCB);
    IOTIVITYTEST_LOG(INFO, "[Cloud Common] signIn returns %s",
            CommonUtil::getOCStackResult(result));
    IOTIVITYTEST_LOG(INFO, "[Cloud Common] UUID = %s   ACCESSTOKE = %s ", g_uid.c_str(),
            g_accessToken.c_str());

    if (OC_STACK_OK != result)
    {
        IOTIVITYTEST_LOG(ERROR, "[Cloud Common] signIn returns %s",
                CommonUtil::getOCStackResult(result));
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_INVOKED != waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud Common] Callback Error : %d", g_isCbInvoked);

            return false;
        }
    }

    return true;
    __FUNC_OUT__
}

bool CloudCommonUtil::signOut(OCAccountManager::Ptr accountMgr)
{
    __FUNC_IN__
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    CloudCommonUtil::g_accessToken = g_cloudPropFile.getProperties(CLOUD_ACCESSTOKEN_KEY);

    OCStackResult result = accountMgr->signOut(CloudCommonUtil::g_accessToken, signInOutCB);
    IOTIVITYTEST_LOG(INFO, "[Cloud Common] signOut returns %s",
            CommonUtil::getOCStackResult(result));

    if (OC_STACK_OK != result)
    {
        IOTIVITYTEST_LOG(ERROR, "[Cloud Common] signOut returns %s",
                CommonUtil::getOCStackResult(result));
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_INVOKED != waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud Common] Callback Error : %d", g_isCbInvoked);
            return false;
        }
    }
    __FUNC_OUT__
    return true;
}
