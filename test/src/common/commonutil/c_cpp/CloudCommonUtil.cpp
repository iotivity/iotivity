/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

static int g_isCbInvoked = CALLBACK_NOT_INVOKED;
OCAccountManager::Ptr accountMgr = nullptr;

std::string CloudCommonUtil::getDefaultHostAddess()
{
    string hostAddress = "";
#ifdef __TLS_ON__
    hostAddress = COAP_TLSON_PREFIX;
#else
    hostAddress = COAP_TLSOFF_PREFIX;
#endif
    string ci_server_ip = CloudCommonUtil::readfile(CI_SERVER_IP_TXT);
    string local_host = ci_server_ip + ":" + to_string(REMOTE_CI_SERVER_PORT);
    hostAddress += local_host;
    hostAddress.erase(std::remove(hostAddress.begin(), hostAddress.end(), '\n'), hostAddress.end());

    return hostAddress;
}

std::string CloudCommonUtil::readfile(std::string filename)
{
    __FUNC_IN__
    FILE *fp;
    char buff[100];

    fp = fopen(filename.c_str(), "r");

    if (NULL == fp)
    {
        IOTIVITYTEST_LOG(ERROR, "[CSC Helper] ERROR Opening File");
    }

    if (NULL == fgets(buff, 100, (FILE*) fp))
    {
        IOTIVITYTEST_LOG(ERROR, "[CSC Helper] Unable to Get input from File");
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
        exit(EXIT_FAILURE);
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
        exit(EXIT_FAILURE);
    }
    memcpy(str->ptr + str->len, ptr, size * nmemb);
    str->ptr[new_len] = '\0';
    str->len = new_len;

    return size * nmemb;
}

char* CloudCommonUtil::get_authenticity_token(const char* responseText)
{
    char* authLast = "";
    char* auth_token = (char*) strstr(responseText,
            "<input name=\"authenticity_token\" type=\"hidden\" value=\"");
    auth_token = strstr(auth_token, "value=\"");
    auth_token = &auth_token[strlen("value=\"")];
    authLast = strstr(auth_token, "\" />");
    auth_token[(authLast - auth_token)] = '\0';
    return auth_token;
}

char* CloudCommonUtil::get_auth_token_code(const char* resposeTxt, char *code)
{
    char* authLast = "";
    char *authcode;

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

char* CloudCommonUtil::getgithubcode(char *gitlogin, char *gitpassword, char *code)
{
    char demoPost[1000] =
    { 0 };
    char *auth_text = "";
    char *auth_url_text = "";
    char *code_text = "";
    CURL *curl;
    CURLcode res;

    struct curl_slist *cookies;
    struct curl_slist *nc;
    struct curl_slist *header;
    struct curl_slist *rheader;

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

        sprintf(demoPost, "%s%s%s%s%s%s%s", PAR_POST_VAL, AUTHENTICITY_TOKEN, auth_url_text,
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
            return "";
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
    char *code = new char[30];
    code = getgithubcode(GITHUB_USER_ID, GITHUB_USER_PASSOWRD, code);
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
            CloudCommonUtil::create_file(CLOUD_ACCESSTOKEN_TXT, CloudCommonUtil::g_accessToken);

        }

        if (itr->attrname().compare("uid") == 0)
        {
            CloudCommonUtil::g_uid = itr->getValueToString();
            CloudCommonUtil::create_file(CLOUD_UUID_TXT, CloudCommonUtil::g_uid);
        }
    }
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

    OCStackResult result = accountMgr->signUp(CloudCommonUtil::g_authprovider,
            CloudCommonUtil::g_authCode, handleLoginoutCB);

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

    CloudCommonUtil::g_uid = readfile(CLOUD_UUID_TXT);
    CloudCommonUtil::g_accessToken = readfile(CLOUD_ACCESSTOKEN_TXT);

    OCStackResult result = accountMgr->signIn(CloudCommonUtil::g_uid,
            CloudCommonUtil::g_accessToken, signInOutCB);
    IOTIVITYTEST_LOG(INFO, "[Cloud Common] signIn returns %s",
            CommonUtil::getOCStackResult(result));

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

    CloudCommonUtil::g_accessToken = readfile(CLOUD_ACCESSTOKEN_TXT);

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
