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

#include "ICHelper.h"

using namespace std;
using namespace OC;
using namespace OC::OCPlatform;

std::mutex ICHelper::s_mutex;
ICHelper* ICHelper::s_ICHelper = NULL;

string ICHelper::UID = "";
string ICHelper::ACCESS_TOKEN = "";
string ICHelper::GROUP_ID="";
string ICHelper::GROUP_TYPE="";
std::vector<std::string> ICHelper::DEVICE_ID={};

bool ICHelper::isServerResponsed = false;
bool ICHelper::isSignedUp = false;
static bool ICHelper::isPostRequestSuccess = false;

ResourceHandles ICHelper::g_ResourceHandles;
OC::OCResource::Ptr ICHelper::m_binaryswitchResource;

ICHelper::ICHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[ICHelper] IN");
}

ICHelper* ICHelper::getInstance(void)
{
    if (s_ICHelper == NULL)
    {
        s_mutex.lock();

        s_ICHelper = new ICHelper();

        s_mutex.unlock();
    }

    return s_ICHelper;
}

ICHelper::~ICHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[ICHelper] OUT");
    if (s_ICHelper != NULL)
    {
        delete s_ICHelper;

        s_ICHelper = NULL;
    }
}

void ICHelper::waitForServerResponse()
{
    isServerResponsed = false;

    void* ret = NULL;
    pthread_t serverlistiningThread;
    pthread_create(&serverlistiningThread, NULL, executeThread, NULL);
    pthread_join(serverlistiningThread, &ret);
}

void* ICHelper::executeThread(void *arg)
{
    int second = 0;

    cout
            << "\nWaiting For Server Response..........................................................\n";

    while (!isServerResponsed)
    {
        sleep(1);
        if (++second == CALLBACK_TIMEOUT)
        {
            cout << "\nTimeout For Server Response!Please Try Again\n\n";
            break;
        }
    }

    pthread_exit (NULL);
}

FILE* ICHelper::readResourceDATFile(const char * /*path*/, const char *mode)
{
    return fopen(RESOURCE_SERVER_DAT, mode);
}

void ICHelper::icPrintRepresentation(OCRepresentation rep)
{
    for (auto itr = rep.begin(); itr != rep.end(); ++itr)
    {
        cout << "\t" << itr->attrname() << ":\t" << itr->getValueToString() << endl;
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
                        cout << "\t\t" << itr2 << endl;
                    }
                    break;

                case AttributeType::String:
                    for (auto itr2 : (*itr).getValue< vector< string > >())
                    {
                        cout << "\t\t" << itr2 << endl;
                    }
                    break;

                default:
                    cout << "Unhandled base type " << itr->base_type() << endl;
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

        ICBinarySwitchResource binarySwitch(IC_RESOURCE_URI_BSWITCH,
        { IC_RESOURCE_TYPE_BSWITCH },
        { DEFAULT_INTERFACE });

        ICTemperatureResource temperature(IC_RESOURCE_URI_TEMP,
        { IC_RESOURCE_TYPE_TEMP },
        { DEFAULT_INTERFACE });

        string uri = airConditioner.getResourceUri();
        string rt = airConditioner.getResourceType()[0];
        string itf = airConditioner.getInterfaces()[0];
        ocResult = OC_STACK_ERROR;
        ocResult = OCPlatform::registerResource(airConditioner.m_handle, uri, rt, itf,
                bind(&ICAirconditionerResource::entityHandler, &airConditioner, placeholders::_1),
                OC_DISCOVERABLE);
        if (ocResult != OC_STACK_OK)
        {
            cout << "ICAirconditionerResource registration is unsuccessful" << endl;
            return false;
        }

        itf = airConditioner.getInterfaces()[1];
        ocResult = OC_STACK_ERROR;
        ocResult = OCPlatform::bindInterfaceToResource(airConditioner.m_handle, itf);
        if (ocResult != OC_STACK_OK)
        {
            cout << "Binding second interface for airConditioner is unsuccessful" << endl;
            return false;
        }

        itf = airConditioner.getInterfaces()[2];
        ocResult = OC_STACK_ERROR;
        ocResult = OCPlatform::bindInterfaceToResource(airConditioner.m_handle, itf);
        if (ocResult != OC_STACK_OK)
        {
            cout << "Binding third interface for airConditioner is unsuccessful" << endl;
            return false;
        }

        uri = binarySwitch.getResourceUri();
        rt = binarySwitch.getResourceType()[0];
        itf = binarySwitch.getInterfaces()[0];
        ocResult = OC_STACK_ERROR;
        ocResult = OCPlatform::registerResource(binarySwitch.m_handle, uri, rt, itf,
                bind(&ICBinarySwitchResource::entityHandler, &binarySwitch, placeholders::_1),OC_OBSERVABLE);
        if (ocResult != OC_STACK_OK)
        {
            cout << "ICBinarySwitchResource registration is unsuccessful" << endl;
            return false;
        }

        uri = temperature.getResourceUri();
        rt = temperature.getResourceType()[0];
        itf = temperature.getInterfaces()[0];
        ocResult = OC_STACK_ERROR;
        ocResult = OCPlatform::registerResource(temperature.m_handle, uri, rt, itf,
                bind(&ICTemperatureResource::entityHandler, &temperature, placeholders::_1),
                OC_OBSERVABLE);
        if (ocResult != OC_STACK_OK)
        {
            cout << "ICTemperatureResource registration is unsuccessful" << endl;
            return false;
        }

        ocResult = OC_STACK_ERROR;
        ocResult = airConditioner.addChildResource(&binarySwitch);
        if (ocResult != OC_STACK_OK)
        {
            cout << "Add ICBinarySwitchResource as a child resource of Air conditioner is unsuccessful" << endl;
            return false;
        }

        ocResult = OC_STACK_ERROR;
        ocResult = airConditioner.addChildResource(&temperature);
        if (ocResult != OC_STACK_OK)
        {
            cout << "Add ICTemperatureResource as a child resource of Air conditioner is unsuccessful" << endl;
            return false;
        }

        cout << "Publishing resources to cloud ";

        ResourceHandles resourceHandles;

        OCDeviceInfo devInfoAirConditioner;
        OCStringLL deviceType;

        deviceType.value = "oic.d.airconditioner";
        deviceType.next = NULL;
        devInfoAirConditioner.deviceName = "FAC_2016";
        devInfoAirConditioner.types = &deviceType;
        devInfoAirConditioner.specVersion = NULL;
        devInfoAirConditioner.dataModelVersions = NULL;
        ocResult = OC_STACK_ERROR;
        ocResult = OCPlatform::registerDeviceInfo(devInfoAirConditioner);
        if (ocResult != OC_STACK_OK)
        {
            cout << "RegisterDeviceInfo is unsuccessful" << endl;
            return false;
        }

        resourceHandles.push_back(airConditioner.m_handle);
        g_ResourceHandles = resourceHandles;
        ICHelper::isServerResponsed = true;
        return true;
    }
    catch(OCException &ex)
    {
        cout << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
        return false;
    }
}

void ICHelper::init_string(stringstr *str) {
    str->len = 0;
    str->ptr = malloc(str->len+1);
  if (str->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  str->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, stringstr *str)
{
  size_t new_len = str->len + size*nmemb;
  str->ptr = realloc(str->ptr, new_len+1);
  if (str->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(str->ptr+str->len, ptr, size*nmemb);
  str->ptr[new_len] = '\0';
  str->len = new_len;

  return size*nmemb;
}

char* ICHelper::get_authenticity_token(const char* resposeTxt)
{
	char* authLast="";
	char* auth_token=strstr(resposeTxt,"<input name=\"authenticity_token\" type=\"hidden\" value=\"");
	auth_token=strstr(auth_token,"value=\"");
	auth_token=&auth_token[strlen("value=\"")];
	authLast=strstr(auth_token,"\" />");
	auth_token[(authLast-auth_token)]='\0';
	return auth_token;
}

char* ICHelper::get_auth_token_code(const char* resposeTxt,char *code)
{
    char* authLast="";
    char *authcode;
    int len;
    char* auth_token=strstr(resposeTxt,"<a href=\"http://www.example.com/oauth_callback/?code=");
    authLast=strstr(auth_token,"\">");
    auth_token[(authLast-auth_token)]='\0';
    auth_token=strstr(auth_token,"code=");
    auth_token=strstr(auth_token,"=");
    len = strlen(auth_token);
    memset(code,0,len);
    strncpy(code,auth_token+1,len-1);
    return code;
}

char* ICHelper::getgithubcode(char gitlogin[],char gitpassword[], char *code)
{
    char demoPost[1000]={0};
    char *auth_text="";
    char *auth_url_text="";
    char *code_text = "";
    CURL *curl;
    CURLcode res;

    struct curl_slist *cookies;
    struct curl_slist *nc;
    struct curl_slist *header;
    struct curl_slist *rheader;

    int i;
    int http_code=0;
    stringstr str;
    init_string(&str);

    curl_global_init(CURL_GLOBAL_ALL);

    //get login page
    curl = curl_easy_init();

    if(curl)
    {
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        //Get Github login page
        curl_easy_setopt(curl, CURLOPT_URL, INIT_GET_LOGIN_URL);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); /* start cookie engine */
        curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1L);

        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL"); // clear all cookies
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
          fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
          return 1;
        }

        auth_text=get_authenticity_token(str.ptr);
        auth_url_text=curl_easy_escape(curl, auth_text, strlen(auth_text));
        sprintf(demoPost,"%s%s%s%s%s",PAR_POST_VAL,AUTHENTICITY_TOKEN,auth_url_text,gitlogin,gitpassword);
        free(str.ptr);

        init_string(&str);
        res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
          if(res != CURLE_OK) {
            fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n",
                    curl_easy_strerror(res));
            exit(1);
          }

          //github login
          curl_easy_setopt(curl, CURLOPT_URL, LOGIN_URL);
          curl_easy_setopt(curl, CURLOPT_POST, 1L); /* start cookie engine */
          curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
          curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);
          curl_easy_setopt(curl,CURLOPT_POSTFIELDS,demoPost);


          nc = cookies, i = 1;
              while(nc) {
                curl_easy_setopt(curl, CURLOPT_COOKIELIST, nc->data);
                nc = nc->next;
                i++;
              }

          res = curl_easy_perform(curl);
          res = curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);

          printf("\n\nResposecode: %d\n\n\n",http_code);

          res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
              if(res != CURLE_OK) {
                fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n",
                        curl_easy_strerror(res));
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
                  while(nc) {
                    curl_easy_setopt(curl, CURLOPT_COOKIELIST, nc->data);
                    nc = nc->next;
                    i++;
                  }
            res = curl_easy_perform(curl);
            if(res != CURLE_OK) {
                  fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
                  return 1;
                }

            code_text = get_auth_token_code(str.ptr, code);

            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();
        free(str.ptr);

        return code_text;
}

char* ICHelper::getGitLoginAuthCodeMain()
{
	char gitlogin[]="&login=iotsrbd@gmail.com";
	char gitpassword[]="&password=Iotivity@123";
	char *code = new char[30];
	code=getgithubcode(gitlogin,gitpassword,code);
	printf("%s\n",code);

	return code;
}
