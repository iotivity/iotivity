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

#ifndef __CLOUD_COMMON_UTIL_H__
#define __CLOUD_COMMON_UTIL_H__

#include <stddef.h>
#include <malloc.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string>
#include <fstream>
#ifndef __ANDROID__
#include <ifaddrs.h>
#include <arpa/inet.h>
#endif
#include <ctime>
#include <iostream>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS
#include "boost/assign.hpp"
#include "octypes.h"
#include <curl/curl.h>
#include "logger.h"
#include "ocpayload.h"
#include "ocstack.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "OCAccountManager.h"
#include "OCApi.h"
#include "OCPlatform.h"
#include "OCPlatform_impl.h"
#include "payload_logging.h"
#include "rd_client.h"
#include "OCRepresentation.h"

#ifdef __SECURED__
#include "ca_adapter_net_ssl.h"
#include "CAManager.h"
#include "casecurityinterface.h"
#include "cathreadpool.h"
#include "casecurityinterface.h"
#include "cathreadpool.h"
#include "occloudprovisioning.h"
#include "ocprovisioningmanager.h"
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "pmtypes.h"
#include "srmutility.h"
#include "ssl_ciphersuites.h"
#include "utils.h"
#include "utlist.h"
#include "auth.h"
#endif

#include "Configuration.h"
#include "CommonUtil.h"
#include "CommonProperties.h"
#include "IotivityTest_Logger.h"

using namespace std;
using namespace OC;

#define INIT_GET_LOGIN_URL "https://github.com/login"
#define LOGIN_URL "https://github.com/session"
#define IOTIVITY_CLOUD_URL "https://github.com/login?return_to=%2Flogin%2Foauth%2Fauthorize%3Fclient_id%3Dea9c18f540323b0213d0%26redirect_uri%3Dhttp%253A%252F%252Fwww.example.com%252Foauth_callback%252F"
#define PAR_POST_VAL "commit=Sign+in&utf8=%E2%9C%93"//"commit=Sign+in&utf8=✓";//"commit=Sign+in&utf8=%E2%9C%93";
#define AUTHENTICITY_TOKEN "&authenticity_token="

typedef struct Stringstr
{
    char *ptr;
    size_t len;
} Stringstr;

const std::string CONFIG_PROP_FILE = "config.properties";
const std::string CLOUD_PROP_FILE = "cloud.properties";
const std::string CLOUD_ACCESSTOKEN_KEY = "accesstoken";
const std::string CLOUD_UID_KEY ="uid";
const std::string CI_SERVER_ETHERNET_IP_KEY = "ci_server_ethernet_ip";
const std::string CI_SERVER_WIFI_IP_KEY = "ci_server_wifi_ip";
const std::string GITHUB_USER_KEY = "github_userid";
const std::string GITHUB_PASSWORD_KEY = "github_password";

class CloudCommonUtil
{
    static long s_setUpDynamicMemoryUsage;
    static long s_tearDownDynamicMemoryUsage;
    static long s_memoryDiffSum;

public:
    static std::string g_uid;
    static std::string g_accessToken;
    static std::string g_refreshToken;
    static std::string g_authCode;
    static std::string g_authprovider;

    static void init_string(Stringstr *str);
    static std::string getDefaultIp();
    static std::string getDefaultHostAddess();
    static OCDevAddr getDefaultEndPoint();
#ifdef __SECURED__
    static OicCloud_t* getCloudServer();
#endif
    static char* get_authenticity_token(const char* resposeTxt);
    static void create_file(string filename, string data);
    static char* get_auth_token_code(const char* responseTxt, char *code);
    static char const* getgithubcode(const char gitlogin[], const char gitpassword[], char *code);
    static void printRepresentation(OCRepresentation rep);
    static OCRepresentation parseOCClientResponse(OCClientResponse* clientResponse);
    static std::string readfile(std::string filename);
    static char* getGitLoginAuthCodeMain();
    static bool signUp(OCAccountManager::Ptr accountMgr);
    static bool signUp(OCAccountManager::Ptr accountMgr, std::string authCode);
    static bool signIn(OCAccountManager::Ptr accountMgr);
    static bool signOut(OCAccountManager::Ptr accountMgr);
};
#endif
