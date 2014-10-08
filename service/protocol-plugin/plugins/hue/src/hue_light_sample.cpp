//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

/// @file hue_light_sample.cpp

/// @brief


#include <boost/algorithm/string.hpp>
#include <signal.h>

#include "PluginProvider.h"
#include "HuePlugin.h"
#include "HueLightHandler.h"
#include "json_parse.h"
#include "http_curl.h"



/**
 *  If you set ENV variable "CONF_PATH", then it will be the default path of configure file, otherwise you can set the default path here
 *
 */

using namespace OIC;
using namespace boost::property_tree;
using namespace boost;
using namespace std;
using namespace OC;


pthread_t tid ;

typedef struct classifier_t classifier_t;
struct classifier_t
{
    /** Classifier specific runtime data */
    cp_context_t *data;

    /** The classifying args */
    char *path;
};



/**
*handler of pluginmanager as a resource
*handle find resource or plugin managerment operations
*it would be trigger by Iotivity core
**/

int gObservation = 0;
void entityHandler(std::shared_ptr<OCResourceRequest> request,
                   std::shared_ptr<OCResourceResponse> response);


void *ChangeLightRepresentation (void *param)
{
    std::cout << "Enter ChangeLightRepresentation" << std::endl;
    return NULL;
}

std::string light_url;


string c_light_url = "/a/container";
string c_resType = "core.container";
string c_resInterface = OC::DEFAULT_INTERFACE;

//LightContainer myLightContainer(c_light_url,c_resType,c_resInterface);
LightContainer *myLightContainer = new LightContainer(c_light_url, c_resType, c_resInterface);
OC::OCPlatform *platform_s;


void entityHandler(std::shared_ptr<OC::OCResourceRequest> request,
                   std::shared_ptr<OC::OCResourceResponse> response)
{
    std::cout << "\tIn Server CPP entity handler:\n";
    if (request)
    {
        // Get the request type and request flag
        std::string requestType = request->getRequestType();
        OC::RequestHandlerFlag requestFlag = request->getRequestHandlerFlag();
        if (requestFlag == OC::RequestHandlerFlag::InitFlag)
        {
            std::cout << "\t\trequestFlag : Init\n";

            // entity handler to perform resource initialization operations
        }
        else if (requestFlag == OC::RequestHandlerFlag::RequestFlag)
        {
            std::cout << "\t\trequestFlag : Request\n";

            CHttpClient *client = new CHttpClient();
//            std::shared_ptr<CHttpClient> client;
            std::string resp;

            // If the request type is GET
            if (requestType == "GET")
            {
                std::cout << "\t\t\trequestType : GET\n";

                // Check for query params (if any)
                OC::QueryParamsMap queryParamsMap = request->getQueryParameters();
                cout << "\t\t\tquery params: \n";
                for (OC::QueryParamsMap::iterator it = queryParamsMap.begin(); it != queryParamsMap.end(); it++)
                {
                    cout << "\t\t\t\t" << it->first << ":" << it->second << endl;
                }

                // Process query params and do required operations ..

                // Get the representation of this resource at this point and send it as response
                // ShortCut need all resource in the same state !!!
                OC::OCRepresentation rep;
                rep = myLightContainer->getRepresentation();

                OC::AttributeMap attributeMap;
                std::cout << "..... container has " << myLightContainer->m_container.size() << std::endl;
                if (myLightContainer->m_container.empty())
                {
                    cout << "no lights to get" << endl;
                    delete client;
                    return;
                }
                vector<LightResource>::iterator iter;
                iter = myLightContainer->m_container.begin();
                light_url = iter->ext_url;
                size_t index = light_url.find_last_of("/");
                string light_get_url = light_url.substr(0, index);

                if (response)
                {
                    response->setErrorCode(200);
                    //TODO restful with hue light

                    std::cout << "ready to send rest api to light" << std::endl;
                    OC::AttributeValues stateVal;
                    OC::AttributeValues colorVal;
                    OC::AttributeValues brightVal;

                    std::cout << "ready to connect url ---- light_get_url  " << light_get_url << std::endl;
                    client->Get((string &)light_get_url, (string &)resp);
//                    std::cout<<"<<<<<<<<<response ----"<<resp<<std::endl;

                    if (resp.size())
                    {
                        stringstream stream;
                        ptree pt;
                        replace_all(resp, "u\'", "\"");
                        replace_all(resp, "\'", "\"");
                        if (resp.find("True") != string::npos)
                            replace_all(resp, "True", "\"true\"");
                        else replace_all(resp, "true", "\"true\"");
                        if (resp.find("False") != string::npos)
                            replace_all(resp, "False", "\"false\"");
                        else replace_all(resp, "false", "\"false\"");

                        stream << resp;
                        read_json<ptree>(stream, pt);
                        string lig_status = pt.get<string>("state.on");
                        int lig_color = pt.get<int>("state.hue");
                        int lig_bri = pt.get<int>("state.bri");

                        if (lig_status == "true")  stateVal.push_back("true");
                        else stateVal.push_back("false");
                        colorVal.push_back(to_string(lig_color));
                        brightVal.push_back(to_string(lig_bri));
                    }
                    attributeMap["state"] = stateVal;
                    attributeMap["color"] = colorVal;
                    attributeMap["bright"] = brightVal;

                    rep.setAttributeMap(attributeMap);
//                    std::cout<<"<<<<<<<<<after rep.setAttributeMap ----"<<std::endl;
                    auto findRes = queryParamsMap.find("if");
                    if (findRes != queryParamsMap.end())
                    {
//                        std::cout<<"<<<<<<<<<Enter setResourceRepresentation found if ----"<<std::endl;
                        response->setResourceRepresentation(rep, findRes->second);
                    }
                    else
                    {
//                        std::cout<<"<<<<<<<<<Enter setResourceRepresentation found no if,using default interface ----"<<std::endl;
                        response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
                    }
                }
            }
            else if (requestType == "PUT")
            {
                std::cout << "\t\t\trequestType : PUT\n";
                std::cout << "..... container has " << myLightContainer->m_container.size() << std::endl;
                if (myLightContainer->m_container.empty())
                {
                    cout << "no lights to put" << endl;
                    delete client;
                    return;
                }

                // Check for query params (if any)
                OC::QueryParamsMap queryParamsMap = request->getQueryParameters();
                cout << "\t\t\tquery params: \n";
                for (auto it = queryParamsMap.begin(); it != queryParamsMap.end(); it++)
                {
                    cout << "\t\t\t\t" << it->first << ":" << it->second << endl;
                }

                // Check queryParamsMap and do required operations ..

                // Get the representation from the request
                OC::OCRepresentation rep = request->getResourceRepresentation();
                OC::AttributeMap attributeMap = rep.getAttributeMap();

                vector<LightResource>::iterator iter;
                for (iter = myLightContainer->m_container.begin(); iter != myLightContainer->m_container.end();
                     iter++)
                {
                    iter->setRepresentation(rep);
                    light_url = iter->ext_url;
                    // Do related operations related to PUT request
                    //TODO restful with hue light
                    std::cout << "ready to send rest api to light" << std::endl;
                    bool m_state;
                    int m_color;
                    int m_bright;

                    m_state = attributeMap["state"][0].compare("true") == 0;
                    m_color = std::stoi(attributeMap["color"][0]);
                    m_bright = std::stoi(attributeMap["bright"][0]);

                    if (m_state)
                    {
                        string putbody = "{\"hue\": " + to_string(m_color) + ", \"on\": true, \"bri\": " + to_string(
                                             m_bright) + "}";
                        client->Put((string &)light_url,  (string &)putbody, (string &)resp);
                    }
                    else
                    {
                        string putbody = "{\"hue\": 0, \"on\": false, \"bri\": 0}";
                        client->Put((string &)light_url,  (string &)putbody, (string &)resp);
                    }
                    rep = iter->getRepresentation();
                }

                if (response)
                {
                    response->setErrorCode(200);
                    auto findRes = queryParamsMap.find("if");

                    if (findRes != queryParamsMap.end())
                    {
                        response->setResourceRepresentation(rep, findRes->second);
                    }
                    else
                    {
                        std::cout <<
                                  "<<<<<<<<<PUT ...Enter setResourceRepresentation found no if,using default interface ----" <<
                                  std::endl;
                        response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
                    }
                    //TODO restful with hue light
                }
            }
            else if (requestType == "POST")
            {
                // POST request operations
            }
            else if (requestType == "DELETE")
            {
                // DELETE request operations
            }
            delete client;
        }
        else if (requestFlag == OC::RequestHandlerFlag::ObserverFlag)
        {
            pthread_t threadId;

            std::cout << "\t\trequestFlag : Observer\n";
            gObservation = 1;

            static int startedThread = 0;

            // Observation happens on a different thread in ChangeLightRepresentation function.
            // If we have not created the thread already, we will create one here.
            if (!startedThread)
            {
                pthread_create (&threadId, NULL, ChangeLightRepresentation, (void *)NULL);
                startedThread = 1;
            }

        }
    }
    else
    {
        std::cout << "Request invalid" << std::endl;
    }
}



/**
 * Handler for hue light resources to intel core:
 *
 * register new resource and unregister lost resource
 *
 */

void *hue_light_handler(void *args)
{
    cout << "Enter the new hue_light_handler thread" << endl;
    HuePlugin *device = new HuePlugin();
    vector<string> res_pool;

  /*  OC::PlatformConfig cfg;
//        cfg.ipAddress = "134.134.161.33";
    cfg.ipAddress = "192.168.27.124";
    cfg.port = 56832;
    cfg.mode = OC::ModeType::Server;
    cfg.serviceType = OC::ServiceType::InProc;*/

    PlatformConfig cfg {
        OC::ServiceType::InProc,
        OC::ModeType::Server,
        "192.168.2.5",
        56832,
        OC::QualityOfService::NonConfirmable
    };


    platform_s = new OC::OCPlatform(cfg);
    cout << "Create OCPlatform OK" << endl;
    myLightContainer->registerResource(*platform_s, &entityHandler);

    string resURL = "/a/light";              //should /**/**/ type
    while (1)
    {

        vector<OCResourceDesc> ret = device->SearchDeviceResources(res_pool);
        cout << "Search resources OK" << endl;
        vector<OCResourceDesc>::iterator iter;

        if (ret.empty())
        {
            cout << "Not found new lights" << endl;
            sleep(10);
            continue;
        }
        else
        {
            cout << "ready to get all lights info" << endl;

            // TO CONTROL ALL Lights
            for (iter = ret.begin(); iter != ret.end(); iter++)
            {
                std::string res_id = iter->GetResource_id();
                std::string url = iter->GetResource_url();
                std::string state = iter->GetResource_state();

                cout << "res_id = " << res_id << endl;
                cout << "url = " << url << endl;
                cout << "status = " << state << endl;

                //TO do  register and unregister with Intel API
                // adding new resource
                if (state == "join")
                {
                    light_url = url;
                    string resType = "core.light";
                    string resInterface = OC::DEFAULT_INTERFACE;
                    LightResource *myLightResource = new LightResource(resURL, resType, resInterface, light_url);

                    int abc;
                    do
                    {
//                             abc = myLightResource->registerResource(platform_s, &entityHandler);
                        abc = myLightResource->registerResource(*platform_s, NULL);
//                             myLightResource->addType(platform_s, "core.brightlight");
//                             myLightResource->addInterface(platform_s, "oc.mi.ll");
                        std::cout << "register result = " << abc << std::endl;
                        sleep(2);
                    }
                    while (abc != 0);
                    cout << "Notifying observers with resource handle: " << myLightResource->getHandle() << endl;

                    myLightContainer->m_container.push_back(*myLightResource);
                    // modify the next light url
                    char *data;
                    int len = resURL.length();
                    data = (char *)malloc((len + 1) * sizeof(char));
                    if (!data)
                    {
                        delete myLightResource;
                        break;
                    }
                    resURL.copy(data, len, 0);
                    data[1]++;
                    resURL = data;
                    free(data);
                    delete myLightResource;
                }
                // remove leaving resource
                else if (state == "leave")
                {
                    light_url = url;
                    vector<LightResource>::iterator l_iter;
                    for (l_iter = myLightContainer->m_container.begin(); l_iter != myLightContainer->m_container.end();)
                    {
                        if (l_iter->ext_url == light_url)
                        {
                            myLightContainer->m_container.erase(l_iter);
                            break;
                        }
                        else l_iter++;
                    }
                }
            }
        }
        sleep(10);
    }
    delete device;
}



/* ------------------------------------------------------------------------
 * Exported classifier information
 * ----------------------------------------------------------------------*/

/**
 * Plug-in runtime information for the framework. The name of this symbol
 * is stored in the plug-in descriptor.
 */
static void *mycreate(cp_context_t *ctx)
{
    cout << "Test create!" << endl;
    classifier_t *cl;
    cl = (classifier_t *)malloc(sizeof(classifier_t));
    if (cl != NULL)
    {
        cl->data = ctx;
    }
    else
    {
        printf("Insufficient memory for plug-in data.\n");
    }
    return cl;
}

static int mystart(void *d)
{
    classifier_t *cl = (classifier_t *)d;
    cp_context_t *data = cl->data;
    cout << "Test started!" << endl;


    int ret;
    ret = pthread_create(&tid, NULL, hue_light_handler, NULL);
    if (ret != 0)
    {
        cout << "Create Hue Light Handler Thread failed!" << endl;
        return -1;
    }
    cout << "after create a thread....." << endl;
    return 0;
}

static void mydestroy(void *d)
{
}


static void mystop(void *d)
{
//    int kill_ret = pthread_kill(tid, SIGKILL);
	std::cout<<"test"<<std::endl;
    int kill_ret = pthread_cancel(tid);

	std::cout<<"test"<<std::endl;
    void *p;
    pthread_join(tid, &p);
    std::cout << "ready to delete platform resource,Lightcontainer and stop plugin" << std::endl;
	std::cout<<"test"<<std::endl;
//    delete platform_s;
    delete myLightContainer;

    classifier_t *cl = (classifier_t *)d;
    if (cl->data != NULL)
    {
        free(cl->data);
    }
}


CP_EXPORT cp_plugin_runtime_t hue_light =
{
    mycreate,
    mystart,
    mystop,
    mydestroy
};


/*
int main(int argc, char* argv[])
{
     std::cout<<"hello"<<std::endl;
     hue_light_handler(NULL);
     return 0;
}*/
