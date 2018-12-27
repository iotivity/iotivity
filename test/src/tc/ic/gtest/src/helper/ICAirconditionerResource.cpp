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

#include "ICAirconditionerResource.h"

OC::OCResource::Ptr g_binaryswitchResource = nullptr;

ICAirconditionerResource::ICAirconditionerResource(string uri, vector< string > rt,
        vector< string > itf) :
        ICResource(uri, rt, itf)
{
}

OCEntityHandlerResult ICAirconditionerResource::entityHandler(
        shared_ptr< OCResourceRequest > request)
{
    IOTIVITYTEST_LOG(INFO, "\tIn Server Airconditioner entity handler:\n");
    OCEntityHandlerResult ehResult = OC_EH_ERROR;

    if (request)
    {
        // Get the request type and request flag
        string requestType = request->getRequestType();
        int requestFlag = request->getRequestHandlerFlag();

        if (requestFlag & RequestHandlerFlag::RequestFlag)
        {
            IOTIVITYTEST_LOG(INFO, "\t\trequestFlag : Request\n");

            // If the request type is GET
            if (requestType == "GET")
            {
                IOTIVITYTEST_LOG(INFO, "\t\t\trequestType : GET\n");
                string findRes = request->getQueryParameters().find("if")->second;
                if (findRes.compare(LINK_INTERFACE) == 0)
                {
                    if (OC_STACK_OK == sendRepresentation(request))
                    {
                        ehResult = OC_EH_OK;
                    }
                }
                else
                {
                    ehResult = OC_EH_FORBIDDEN;
                }
            }
            else if (requestType == "PUT")
            {
                IOTIVITYTEST_LOG(INFO, "\t\t\trequestType : PUT\n");
            }
            else if (requestType == "POST")
            {
                // POST request operations
            }
            else if (requestType == "DELETE")
            {
                // DELETE request operations
            }
        }

        if (requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            IOTIVITYTEST_LOG(INFO, "\t\trequestFlag : Observer\n");
        }
    }
    else
    {
        IOTIVITYTEST_LOG(INFO, "Request invalid");
    }

    return ehResult;
}

void ICAirconditionerResource::onObserve(const HeaderOptions /*headerOptions*/,
        const OCRepresentation &rep, const int &eCode, const int &sequenceNumber)
{
    try
    {
        if (eCode == OC_STACK_OK && sequenceNumber <= MAX_SEQUENCE_NUMBER)
        {
            if (sequenceNumber == OC_OBSERVE_REGISTER)
            {
                IOTIVITYTEST_LOG(INFO, "Observe registration action is successful for airconditon");
            }

            IOTIVITYTEST_LOG(INFO, "OBSERVE RESULT:");
        }
        else
        {
            if (eCode == OC_STACK_OK)
            {
                IOTIVITYTEST_LOG(INFO,
                        "Observe registration failed or de-registration action failed/succeeded");
            }
            else
            {
                IOTIVITYTEST_LOG(INFO, "onObserve Response error: %d\n", eCode);
            }
        }
    }
    catch (exception &e)
    {
        cout << "Exception: " << e.what() << " in onObserve" << endl;
    }
}

void getCollectionResource(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    IOTIVITYTEST_LOG(INFO, "Resource get: %d\n", ecode);

    vector< OCRepresentation > children = rep.getChildren();

    IOTIVITYTEST_LOG(INFO, "Constructing binary switch\n");

    for (auto it = children.begin(); it != children.end(); it++)
    {
        cout << "RT: " << it->getResourceTypes().at(0) << endl;

        if (it->getResourceTypes().at(0).compare("oic.r.switch.binary") == 0)
        {
            cout << "Observing " << it->getUri() << endl;
            g_binaryswitchResource = OCPlatform::constructResourceObject(COAP_HOST_ADDRESS,
                    it->getUri(), static_cast< OCConnectivityType >(CT_ADAPTER_TCP | CT_IP_USE_V4),
                    true,
                    { string("oic.r.switch.binary") },
                    { string(DEFAULT_INTERFACE) });

            QueryParamsMap query;
            g_binaryswitchResource->observe(ObserveType::Observe, query,
                    &ICAirconditionerResource::onObserve);
        }
    }
}

void ICAirconditionerResource::foundAirconditionerResource(shared_ptr< OC::OCResource > resource)
{
    vector< string > rt = resource->getResourceTypes();

    cout << "Aircondition resource found: " << resource->uri() << endl;

    for (auto it = rt.begin(); it != rt.end(); it++)
    {
        cout << "RT: " << *it << endl;

        if (it->compare("x.com.samsung.da.device") == 0)
        {
            cout << "Found Samsung Airconditioner" << endl;

            QueryParamsMap query;
            query["if"] = string(LINK_INTERFACE);
            //Request to collection resource
            resource->get(query, &getCollectionResource);
        }
    }
}
