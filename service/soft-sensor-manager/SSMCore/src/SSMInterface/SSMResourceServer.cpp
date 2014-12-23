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

#include "SSMResourceServer.h"
#include "SSMCore.h"
#include "Common/PlatformLayer.h"
#include "Common/InternalInterface.h"
using namespace OC;

static std::vector< std::map<std::string, std::string> > g_vecQueryEventResults;

class CQueryEngineEvent: public IQueryEngineEvent
{
    private:
        std::string m_queryEngineId;
        OCResourceHandle m_hSSMResource;

    public:
        /* Constructor */
        CQueryEngineEvent(std::string queryEngineId, OCResourceHandle resourceHandle)
        {
            m_queryEngineId = queryEngineId;
            m_hSSMResource = resourceHandle;
        }

        SSMRESULT onQueryEngineEvent(IN int cqid, IN IDataReader *pResult)
        {
            int dataCount = 0;
            IModelData *pModelData = NULL;
            std::vector < std::string > affectedModels;

            std::map<std::string, std::string> queryEventResult;

            std::stringstream sstream;

            // QueryEngine Id
            queryEventResult["queryEngineId"] = m_queryEngineId;

            // CQID
            sstream << cqid;
            queryEventResult["CQID"] = sstream.str();
            sstream.str("");

            pResult->getAffectedModels(&affectedModels);

            // Affected Model Count
            sstream << affectedModels.size();
            queryEventResult["modelCount"] = sstream.str();
            sstream.str("");

            //TODO: we assume that contains only one model at time
            for (std::vector< std::string >::iterator itor = affectedModels.begin();
                 itor != affectedModels.end(); ++itor)
            {
                // Model Name
                sstream << (*itor);
                queryEventResult["modelName"] = sstream.str();
                sstream.str("");

                pResult->getModelDataCount(*itor, &dataCount);

                // Data Count
                sstream << dataCount;
                queryEventResult["dataCount"] = sstream.str();
                sstream.str("");

                //FixME: we have to support multiple data count
                for (int i = 0; i < dataCount; i++)
                {
                    pResult->getModelData(*itor, i, &pModelData);

                    // Data Id
                    sstream << pModelData->getDataId();
                    queryEventResult["dataId"] = sstream.str();
                    sstream.str("");

                    // Property Count
                    sstream << pModelData->getPropertyCount();
                    queryEventResult["propertyCount"] = sstream.str();
                    sstream.str("");

                    for (int j = 0; j < pModelData->getPropertyCount(); j++)
                    {
                        // Property Name & Value
                        sstream << pModelData->getPropertyValue(j).c_str();
                        queryEventResult[pModelData->getPropertyName(j).c_str()] = sstream.str();
                        sstream.str("");
                    }
                }
            }

            g_vecQueryEventResults.push_back(queryEventResult);

            //TODO: need to modify for notifying proper clients
            OCPlatform::notifyAllObservers(m_hSSMResource);

            return SSM_S_OK;
        }
};

SSMResourceServer::SSMResourceServer()
{
    m_hSSMResource = NULL;
}

SSMResourceServer::~SSMResourceServer()
{
}

int SSMResourceServer::initializeManager(std::string &xmlDescription)
{
    SSMRESULT res = SSM_E_FAIL;

    SSM_CLEANUP_ASSERT(InitializeSSMCore(xmlDescription));
    SSM_CLEANUP_ASSERT(StartSSMCore());

    if (createResource() != 0)
    {
        SSM_CLEANUP_ASSERT (SSM_E_FAIL);
    }

CLEANUP:
    if (res != SSM_S_OK)
        return -1;

    return 0;
}

int SSMResourceServer::terminateManager()
{
    SSMRESULT res = SSM_E_FAIL;

    SSM_CLEANUP_ASSERT(StopSSMCore());
    SSM_CLEANUP_ASSERT(TerminateSSMCore());

CLEANUP:
    if (res != SSM_S_OK)
        return -1;

    return 0;
}

int SSMResourceServer::createResource()
{
    std::string resourceURI = "/service/SoftSensorManager"; // URI of the resource
    std::string resourceTypeName = "core.SoftSensorManager"; // resource type name.
    std::string resourceInterface = DEFAULT_INTERFACE; // resource interface.

    // OCResourceProperty is defined ocstack.h
    uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

    // This will internally create and register the resource.
    OCStackResult result = OCPlatform::registerResource(m_hSSMResource, resourceURI,
                           resourceTypeName, resourceInterface,
                           std::bind(&SSMResourceServer::entityHandler, this, std::placeholders::_1
                                    ), resourceProperty);

    if (OC_STACK_OK != result)
    {
        return -1;
    }

    return 0;
}

OCEntityHandlerResult SSMResourceServer::entityHandler(std::shared_ptr< OCResourceRequest > request)
{
    SSMRESULT res = SSM_E_FAIL;

    auto response = std::make_shared<OC::OCResourceResponse>();

    if (request)
    {
        // Get the request type and request flag
        std::string requestType = request->getRequestType();
        int requestFlag = request->getRequestHandlerFlag();

        response->setRequestHandle(request->getRequestHandle());
        response->setResourceHandle(request->getResourceHandle());

        if (requestFlag & RequestHandlerFlag::InitFlag)
        {
            // entity handler to perform resource initialization operations
        }

        if (requestFlag & RequestHandlerFlag::RequestFlag)
        {
            cout << "\t\trequestFlag : Request\n";

            // If the request type is GET
            if (requestType == "GET")
            {
                std::map<std::string, std::string> responseAttributeMap;

                OCRepresentation rep = request->getResourceRepresentation();

                if (g_vecQueryEventResults.size() > 0)
                {
                    responseAttributeMap = g_vecQueryEventResults.front();
                    g_vecQueryEventResults.erase(g_vecQueryEventResults.begin());
                }

                if (response)
                {
                    for (std::map<std::string, std::string>::iterator itor =
                             responseAttributeMap.begin(); itor != responseAttributeMap.end();
                         ++itor)
                    {
                        rep.setValue(itor->first, itor->second);
                    }

                    response->setErrorCode(200);
                    response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
                }
            }
            else if (requestType == "PUT")
            {
                OCRepresentation rep = request->getResourceRepresentation();

                IQueryEngine *pQueryEngine = NULL;

                std::stringstream sstream;

                // Process query params and do required operations ..
                if (rep.getValue<std::string>("command") == "CreateQueryEngine")
                {
                    CQueryEngineEvent *queryEngineEvent = NULL;

                    res = CreateQueryEngine(&pQueryEngine);

                    if (res != SSM_S_OK)
                    {
                        rep.setValue("error", "CreateQueryEngine failed");
                        goto CLEANUP;
                    }

                    sstream << reinterpret_cast<intptr_t>(pQueryEngine);

                    // Register QueryEngineEvent
                    queryEngineEvent = new CQueryEngineEvent(sstream.str(), m_hSSMResource);

                    if (queryEngineEvent == NULL)
                    {
                        rep.setValue("error", "QueryEngineEvent create failed");
                        goto CLEANUP;
                    }

                    res = pQueryEngine->registerQueryEvent(queryEngineEvent);

                    if (res != SSM_S_OK)
                    {
                        rep.setValue("error", "RegisterQueryEvent failed");
                        goto CLEANUP;
                    }

                    rep.setValue("queryEngineId", sstream.str());
                }
                else if (rep.getValue<std::string>("command") == "ReleaseQueryEngine")
                {
                    pQueryEngine = (IQueryEngine *) std::stoi(
                                       rep.getValue<std::string>("queryEngineId"));

                    ReleaseQueryEngine(pQueryEngine);
                }
                else if (rep.getValue<std::string>("command") == "ExecuteContextQuery")
                {
                    int CQID = 0;

                    pQueryEngine = (IQueryEngine *) std::stoi(
                                       rep.getValue<std::string>("queryEngineId"));

                    res = pQueryEngine->executeContextQuery(
                              rep.getValue<std::string>("contextQuery"), &CQID);

                    if (res != SSM_S_OK)
                    {
                        rep.setValue("error", "ExecuteContextQuery failed");
                        goto CLEANUP;
                    }

                    sstream << CQID;

                    rep.setValue("CQID", sstream.str());
                }
                else if (rep.getValue<std::string>("command") == "KillContextQuery")
                {
                    pQueryEngine = (IQueryEngine *) std::stoi(
                                       rep.getValue<std::string>("queryEngineId"));

                    res = pQueryEngine->killContextQuery(std::stoi(rep.getValue<std::string>("CQID")));

                    if (res != SSM_S_OK)
                    {
                        rep.setValue("error", "KillContextQuery failed");
                        goto CLEANUP;
                    }
                }

CLEANUP:
                if (response)
                {
                    response->setErrorCode(200);
                    response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
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
        }

        if (requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            // perform observe related operations on the resource.
        }
    }

    return OCPlatform::sendResponse(response) == OC_STACK_OK ? OC_EH_OK : OC_EH_ERROR;
}
