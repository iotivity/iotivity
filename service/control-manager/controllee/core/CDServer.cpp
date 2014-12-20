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

#include "CDServer.h"
#include "OCPlatform.h"
#include "CDContext.h"
#include "CDDeserializer.h"
#include "CDSerializer.h"
#include "CDCoreResourceHandlerFactory.h"
#include "CDSerializableDataFactory.h"
#include "ISubscriptionManager.h"
#include "ISerializable.h"
#include "ISerializer.h"
#include "IDeserializer.h"
#include "CDErrorMessage.h"
#include "JsonDeserializer.h"
#include "JsonSerializer.h"
#include "OCHeaderOption.h"
#include "log.h"
#include <string>

using namespace std;

namespace OC
{

    string cdescapeString(const string &value);
    CDServerListener *CDServer::_listener = NULL;
    OC::Cm::IContext *CDServer::_context = NULL;
    typedef shared_ptr<OC::OCResourceRequest> OCResourceReq;
    typedef shared_ptr<OC::OCResourceResponse> OCResourceRes;

    CDServer::CDServer(OC::Cm::IContext *p_context)
    {
        // m_ocPlatform = p_ocPlatform;
        _context = p_context;
        _listener = this;
        _serializableDataFactory = NULL;
        _resourceHandlerFactory = NULL;
    }

    CDServer::~CDServer()
    {
        this->deinit();
    }

    bool CDServer::initialize(OC::Cm::Configuration *configuration)
    {

        OC::CDContext *_cdContext = (OC::CDContext *) _context;
        if (registerOCResources() == false)
        {
            return false;
        }
        _resourceHandlerFactory = new CDCoreResourceHandlerFactory();
        _serializableDataFactory = new CDSerializableDataFactory();
        _resourceHandlerFactory->pApplicationFactory =
            configuration->getResourceHandlerFactory();
        _serializableDataFactory->pApplicationFactory =
            configuration->getSerializableDataFactory();
        if (_cdContext->p_subscriptionManager != NULL)
        {
            _serializableDataFactory->pSubscriptionFactory =
                _cdContext->p_subscriptionManager->getSerializationFactory();
            _resourceHandlerFactory->pSubscriptionFactory =
                _cdContext->p_subscriptionManager->getResourceHandlerFactory();
        }
        _resourceHandlerFactory->setContext(_context);
        OC::OCPlatform::setDefaultDeviceEntityHandler(&CDServer::handleOCRequest);
        return true;
    }

    bool CDServer::deinit()
    {
        if (_resourceHandlerFactory)
        {
            _resourceHandlerFactory->pApplicationFactory = NULL;
            delete (_resourceHandlerFactory);
            _resourceHandlerFactory = NULL;
        }
        if (_serializableDataFactory)
        {
            _serializableDataFactory->pApplicationFactory = NULL;
            delete (_serializableDataFactory);
            _serializableDataFactory = NULL;
        }
        if (_context)
        {
            _context = NULL;
        }
        return true;
    }

    bool CDServer::start()
    {
        return true;
    }

    bool CDServer::stop()
    {
        return true;
    }

    OC::Cm::Serialization::SerializableDataFactory *CDServer::getSerializableDataFactory()
    {
        return _serializableDataFactory;
    }

    bool CDServer::registerOCResource(string resourceURI, string resourceName,
                                      bool isDiscoverable)
    {
        CE(cout << __PRETTY_FUNCTION__ << std::endl);
        bool bRet = false;
        OCResourceHandle resourceHandle;
        string resourceInterface = OC::DEFAULT_INTERFACE;
        uint8_t resourceProperty = OCResourceProperty::OC_ACTIVE;
        string resourceTypeName = resourceName;

        if (isDiscoverable == true)
        {
            resourceProperty = OCResourceProperty::OC_DISCOVERABLE;
        }

        OCStackResult result = OC::OCPlatform::registerResource(resourceHandle,
                               resourceURI, resourceTypeName, resourceInterface,
                               &CDServer::handleOCRequest, resourceProperty);

        if (result == OCStackResult::OC_STACK_OK)
        {

            CE(cout << "CDServer::registerOCResource=> Registered Resource " << resourceTypeName << endl);

            bRet = true;
        }
        else
        {

            CE(cout << "CDServer::registerOCResource => Failed to Register Resource " << resourceTypeName << endl);

            bRet = false;
        }

        return bRet;
    }

    bool CDServer::registerOCResources()
    {

        CE(cout << __PRETTY_FUNCTION__ << std::endl);
        bool bRet = true;
        bRet = registerOCResource("/capability", "capability", true); // Registering capability resource
        bRet = bRet && registerOCResource("/devices", "devices", false); // Registering devices resource
        bRet = bRet && registerOCResource("/devices/{deviceId}", "device", false); // Registering device resource
        return bRet;
    }

    OCEntityHandlerResult CDServer::handleOCRequest(
        const shared_ptr<OC::OCResourceRequest> ocRequest /**,
        shared_ptr<OC::OCResourceResponse> ocResponse **/)
    {

        CE(printf("CDServer::handleOCRequest => Entering handleOCRequest Callback \n"));
        auto ocResponse = std::make_shared<OC::OCResourceResponse>();

        if (_listener != NULL)
        {
            ocResponse->setRequestHandle(ocRequest->getRequestHandle());
            ocResponse->setResourceHandle(ocRequest->getResourceHandle());
            _listener->handleRequest(ocRequest, ocResponse);
        }
        else
        {

            CE(cout << "Listener not set in " << __PRETTY_FUNCTION__ << endl);

            return OC_EH_ERROR;
        }

	// Request that the OC Platform sends the response on the wire
	//
	ocResponse->setResponseResult(OC_EH_OK);
        CE(cout << "\nResponse set : " << "\n");

	// Comment out for now as stack has issues if the JSON representation is not actually set
	//
	//std::string m_payload = ocResponse->getResourceRepresentation().getJSONRepresentation();
        //CE(cout << "\nFinal Payload : " << m_payload << "\n");

	OCStackResult sr = OCPlatform::sendResponse(ocResponse);
	
	if (sr != OC_STACK_OK) {
                CE(printf("CDServer::error in sending ocResponse \n"));
		return OC_EH_ERROR;
	}
	return OC_EH_OK;
    }

    int CDServer::simple_deserialize(const OCResourceReq &ocRequest,
                                     OC::Cm::Serialization::ISerializable *m_pRequestPayloadOut)
    {

        OC::Cm::Serialization::ISerializable *m_pRequestPayload = NULL;
        OC::Cm::Serialization::ISerializable *m_pResponsePayload = NULL;
        string payLoad = "", errMsg = "", uri = "", method = "";
        int responseStatus = -1, resourceType, statusCode = 400;
        map<string, string> m_template_parameters;
        if (!ocRequest)
        {
            //TODO: Error Control

            CE(printf("CDServer::simple_deserialize => handleOCRequest null ocRequest Error \n"));

            return 00;
        }
        uri = ocRequest->getResourceUri();

        CE(cout << "CDServer::simple_deserialize=> Resource URI from OCRequest " << uri << endl);

        if (uri.empty())
        {
            uri = "/capability";
        }
        method = ocRequest->getRequestType();
        resourceType = _resourceHandlerFactory->getResourceType(uri,
                       m_template_parameters);
        _serializableDataFactory->createMethodSerializables(resourceType, method,
                &m_pRequestPayload, &m_pResponsePayload);
        string m_payloadType = OIC_PAYLOAD_TYPE, reqType = "";
        int payloadSize = 0;
        map<string, string> m_queryParams;
        OCRepresentation ocRep;
        ocRep = ocRequest->getResourceRepresentation();
        payLoad = ocRep.getValue<string>("json");
        payloadSize = payLoad.size();

        CE(cout << "RequestType " << method << endl);

        if (NULL != m_pRequestPayload)
        {
            string errMsg;

            CE(printf("handleRequest payload non-NULL, de-serialize \n"));

            statusCode = deSerialize(m_payloadType, payLoad, payloadSize, method,
                                     m_queryParams, m_pRequestPayload, errMsg);
            m_pRequestPayloadOut = m_pRequestPayload;
            delete m_pResponsePayload;
        }
        else
        {
            CE(printf("NULL IN simple deserialize\n"));
        }

        return statusCode;
    }

    void CDServer::handleRequest(const OCResourceReq ocRequest,
                                 OCResourceRes ocResponse)
    {

        CE(printf("CDServer::handleRequest=> Entering handleOCRequest Callback \n"));

        OC::Cm::Serialization::ISerializable *m_pRequestPayload = NULL,
                                              *m_pResponsePayload = NULL;
        map<string, string> m_template_parameters;
        string payLoad = "", errMsg = "", uri = "", method = "";
        int responseStatus = -1, resourceType;

        if (!ocRequest || !ocResponse)
        {
            ocResponse->setErrorCode(OC_STACK_ERROR);

            CE(cout << "handleOCRequest null ocRequest/O Error in " << __PRETTY_FUNCTION__ << endl);

            return;
        }
        uri = ocRequest->getResourceUri();

        CE(cout << "Resource URI from OCRequest " << uri << endl);

        if (uri.empty())
        {
            uri = "/capability";
        }
        method = ocRequest->getRequestType();

        if (_resourceHandlerFactory != NULL)
        {
            resourceType = _resourceHandlerFactory->getResourceType(uri,
                           m_template_parameters);
        }
        else
        {
            ocResponse->setErrorCode(OC_STACK_ERROR);

            CE(cout << "Resource Handler Factory not set in " << __PRETTY_FUNCTION__ << endl);

            return;
        }

        if (_serializableDataFactory != NULL)
        {
            _serializableDataFactory->createMethodSerializables(resourceType,
                    method, &m_pRequestPayload, &m_pResponsePayload);
        }
        else
        {
            ocResponse->setErrorCode(OC_STACK_ERROR);

            CE(cout << "Serializable Data factory not set in " << __PRETTY_FUNCTION__ << endl);

            return;
        }

        string m_payloadType = OIC_PAYLOAD_TYPE, reqType = "";
        int payloadSize = 0;
        map<string, string> m_queryParams;
        OCRepresentation ocRep;
        ocRep = ocRequest->getResourceRepresentation();
        payLoad = ocRep.getValue<string>("json");
        payloadSize = payLoad.size();

        CE(cout << "RequestType " << method << endl);

        if (NULL != m_pRequestPayload)
        {
            string errMsg;

            CE(printf("handleRequest payload non-NULL, de-serialize \n"));

            int statusCode = deSerialize(m_payloadType, payLoad, payloadSize,
                                         method, m_queryParams, m_pRequestPayload, errMsg);
        }
        //int statusCode = simple_deserialize(ocRequest, m_pRequestPayload);
        OC::Cm::Server::ResourceHandler::IResourceHandler *p_resourceHandler =
            _resourceHandlerFactory->createResourceHandler(resourceType);
        if (p_resourceHandler != NULL)
        {
            p_resourceHandler->handleResource(ocRequest->getRequestType(),
                                              m_pRequestPayload, m_pResponsePayload, responseStatus);
        }
        else
        {
            ocResponse->setErrorCode(OC_STACK_ERROR);

            CE(cout << "Resource Handler not set available in " << __PRETTY_FUNCTION__ << " for " << resourceType << endl);
            if (_serializableDataFactory != NULL)
            {
                _serializableDataFactory->deleteMethodSerializables(resourceType,
                        m_pRequestPayload, m_pResponsePayload);
            }
            return;
        }
        if (m_pResponsePayload != NULL)
        {

            CE(printf("handleOCRequest responsePayload non-NULL \n"));

            string m_payloadType = OIC_PAYLOAD_TYPE;
            string m_pretty_print_payload = "", errorMessage = "";
            map<string, string> m_queryParams;
            string replyOC;
            OCRepresentation ocRep;
            int payLoadSize = 0;
            string reqType = ocRequest->getRequestType();
            if (responseStatus != 201)
            {
                if (false
                    == CDServer::serialize(m_pResponsePayload, reqType,
                                           &(m_queryParams), m_payloadType, replyOC,
                                           payLoadSize, m_pretty_print_payload,
                                           errorMessage))
                {
                    if (m_pResponsePayload->getElementName().compare("error")
                        != 0)
                    {
                        ocResponse->setErrorCode(OC_STACK_ERROR);

                        CE(printf("handleOCRequest Serialize Error Class 1 \n")); CE(cout << errorMessage << endl);

                    }
                    else
                    {

                        CE(printf("handleOCRequest Serialize Error Class 2 \n"));

                        payLoadSize = 0;
                        responseStatus = 500;
                    }
                }

            }

            string loc;
            string knownHeader = "2048";
            if ((m_pResponsePayload->resp.m_headers.getHeader(knownHeader, loc)))
            {

                CE(printf("Adding header option 2048 \n"));

                CE(cout << "Contents: " << loc << endl);

                OC::HeaderOption::OCHeaderOption header(2048, loc);
                OC::HeaderOption::OCHeaderOption header_version(2050, "v1.0.0");
                OC::HeaderOptions headerOptions;
                headerOptions.push_back(header);
                headerOptions.push_back(header_version);
                ocResponse->setHeaderOptions(headerOptions);
                CE(printf("Header options set \n"));
                ocResponse->setErrorCode(201);
            }
            else
            {
                OC::HeaderOption::OCHeaderOption header_version(2050, "v1.0.0");
                OC::HeaderOptions headerOptions;
                headerOptions.push_back(header_version);
                ocResponse->setHeaderOptions(headerOptions);
                ocRep.setValue("json", replyOC);
                ocResponse->setErrorCode(responseStatus);
                ocResponse->setResourceRepresentation(ocRep, DEFAULT_INTERFACE);
            }
        }
        else
        {
            //TODO:set an error ocResponse

            CE(printf("handleOCRequest response payload NULL \n"));

            ocResponse->setErrorCode(responseStatus);

        }
        if (_serializableDataFactory != NULL)
        {
            _serializableDataFactory->deleteMethodSerializables(resourceType,
                    m_pRequestPayload, m_pResponsePayload);
        }
        if ((p_resourceHandler != NULL) && (_resourceHandlerFactory != NULL))
        {

            _resourceHandlerFactory->deleteResourceHandler(p_resourceHandler,
                    resourceType);
        }

    }

    int CDServer::deSerialize(string &contentType, string &payLoad, int payloadLen,
                              string &method, map<string, string> &queryParams,
                              OC::Cm::Serialization::ISerializable *data, string &errorMessage)
    {

        if ((payLoad.empty()) || (payloadLen == 0) || (NULL == data)
            || contentType.empty())
        {
            return 400;
        }

        OC::Cm::Serialization::IDeserializer *p_deserializer = NULL;
        p_deserializer = new OC::Cm::Core::Serialization::Json::JsonDeserializer(
            _context);

        if (p_deserializer == NULL)
        {
            return 415;
        }

        if (queryParams.find("filter") != queryParams.end())
        {
            string filter = queryParams["filter"];

            if (filter.size() > 0)
            {
                p_deserializer->setFilter(filter);
            }
        }

        if (queryParams.find("recursive") != queryParams.end())
        {
            string recursive = queryParams["recursive"];

            if (0 == recursive.compare("false"))
            {
                p_deserializer->setRecursive(false);
            }
            else
            {
                p_deserializer->setRecursive(true);
            }
        }

        if (0 == strcasecmp(method.c_str(), "PUT"))
        {
            p_deserializer->setValidateMandatory(false);
        }
        else if (0 == strcasecmp(method.c_str(), "POST"))
        {
            p_deserializer->setIDExcemption(true);
        }

        if (false == p_deserializer->parse(payLoad, payloadLen))
        {

            errorMessage = p_deserializer->getErrorMessage();

            delete p_deserializer;
            return 400;
        }

        if (false == data->deSerialize(*p_deserializer))
        {
            OC::Cm::Serialization::ISerializable *errorData =
                new OC::Cm::Serialization::Xsd::CDErrorMessage();
            if (errorData->deSerialize(*p_deserializer)) // Check for CMErrorMessage DataModel
            {
                data = errorData;
                delete p_deserializer;
                return 200;
            }

            errorMessage = p_deserializer->getErrorMessage();

            delete p_deserializer;
            return 400;
        }

        delete p_deserializer;

        return 200;
    }

    bool CDServer::serialize(OC::Cm::Serialization::ISerializable *data,
                             string &method, map<string, string> *queryParams, string &contentType,
                             string &payLoad, int &payloadLen, string &prettyPrintOutput,
                             string &errorMessage)
    {
        if (data == NULL)
        {
            return false;
        }
        /* OIC : Using OCSerializer */
        OC::Cm::Serialization::ISerializer *p_serializer =
            new OC::Cm::Core::Serialization::Json::JsonSerializer();
        if (p_serializer == NULL)
        {
            return false;
        }
        if (NULL != queryParams)
        {
            if (queryParams->find("filter") != queryParams->end())
            {
                string filter = (*queryParams)["filter"];

                if (filter.size() > 0)
                {
                    p_serializer->setFilter(filter);
                }
            }
            if (queryParams->find("recursive") != queryParams->end())
            {
                string recursive = (*queryParams)["recursive"];

                if (0 == recursive.compare("false"))
                {
                    p_serializer->setRecursive(false);
                }
                else
                {
                    p_serializer->setRecursive(true);
                }
            }
        }
        if (0 == strcasecmp(method.c_str(), "PUT"))
        {
            p_serializer->setValidateMandatory(false);
        }
        else if (0 == strcasecmp(method.c_str(), "POST"))
        {
            p_serializer->setIDExcemption(true);
        }
        if (false == data->serialize(*p_serializer))
        {
            errorMessage = p_serializer->getErrorMessage();

            delete p_serializer;
            return false;
        }
        if (false == p_serializer->getOutput(payLoad, payloadLen))
        {
            errorMessage = p_serializer->getErrorMessage();

            delete p_serializer;
            return false;
        }

        CE(cout << "\nPayload : " << payLoad << "\n");

        if (false == p_serializer->getPrettyPrintOutput(prettyPrintOutput))
        {

            CE(cout << "something might be wrong in the serialize" << "\n");

        }

        contentType = p_serializer->getContentType();
        delete p_serializer;

        return true;
    }

// Helper function to escape special character.
    string cdescapeString(const string &value)
    {
        ostringstream stringStream;
for (const char & c : value)
        {
            switch (c)
            {
                case '\\':
                    stringStream << "\\\\";
                    break;
                case '"':
                    stringStream << "\\\"";
                    break;
                case '/':
                    stringStream << "\\/";
                    break;
                case '\b':
                    stringStream << "\\b";
                    break;
                case '\f':
                    stringStream << "\\f";
                    break;
                case '\n':
                    stringStream << "\\n";
                    break;
                case '\r':
                    stringStream << "\\r";
                    break;
                case '\t':
                    stringStream << "\\t";
                    break;
                default:
                    stringStream << c;
                    break;
            }
        }
        return stringStream.str();
    }

    void CDServer::sendClientRequest(string method, string deviceAddress,
                                     string relativeURI, OC::Cm::Serialization::ISerializable *data)
    {

        CE(cout << "CDServer::sendClientRequest() => " << "called!" << endl);

        OC::CDContext *pCDContext = (OC::CDContext *) _context;

        if ((deviceAddress.empty() == true) || (relativeURI.empty() == true)
            || (method.empty() == true))
        {
            return;
        }

        // Start to construct request
        if (data != NULL)
        {
            string m_payloadType = "application/json; charset=utf-8"; //TODO MACRO this
            string m_pretty_print_payload = "", errorMessage = "", replyOC;
            map<string, string> m_queryParams;
            OCRepresentation ocRep;
            int payLoadSize = 0;
            if (false
                == CDServer::serialize(data, method, &(m_queryParams),
                                       m_payloadType, replyOC, payLoadSize,
                                       m_pretty_print_payload, errorMessage))
            {

                return; //return error
            }

            CE(cout << "sendRequest() payload" << endl); CE(cout << replyOC << endl);

            ocRep.setValue("json", replyOC);
            OC::HeaderOption::OCHeaderOption header_version(2050, "v1.0.0");
            OC::HeaderOptions headerOptions;
            headerOptions.push_back(header_version);

            // Request constructed

            // Construct the resource types and interfaces properly.
            const vector<string> resourceTypes = { "clientresource" };
            const vector<string> interfaces = { DEFAULT_INTERFACE };
            string resourceURI = "coap://" + deviceAddress;
            // interfaces constructed properly.

            CE(cout << "sendClientRequest() resourceURI: " << resourceURI << endl);

            string resourcePath = relativeURI;

            CE(cout << "sendClientRequest() resourcePath: " << resourcePath << endl);

            shared_ptr<OC::OCResource> resource =
                OC::OCPlatform::constructResourceObject(resourceURI, resourcePath,
                        false, resourceTypes, interfaces);

            if (resource == NULL)
            {
                return; // OCStackResult::OC_STACK_ERROR;
            }

            OCStackResult result;
            resource->setHeaderOptions(headerOptions);
            if (method.compare("GET") == 0)
            {
                result = resource->get(m_queryParams,
                                       GetCallback(
                                           bind(&CDServer::onResponse, placeholders::_1,
                                                placeholders::_2, placeholders::_3)));
            }
            else if (method.compare("PUT") == 0)
            {
                result = resource->put(ocRep, m_queryParams,
                                       PutCallback(
                                           bind(&CDServer::onResponse, placeholders::_1,
                                                placeholders::_2, placeholders::_3)));
            }
            else if (method.compare("POST") == 0)
            {

                CE(cout << "sendRequest().OC POST => " << "called!" << endl);

                result = resource->post(ocRep, m_queryParams,
                                        PostCallback(
                                            bind(&CDServer::onResponse, placeholders::_1,
                                                 placeholders::_2, placeholders::_3)));

            }
            else if (method.compare("DELETE") == 0)
            {
                result = resource->deleteResource(
                             DeleteCallback(
                                 bind(&CDServer::onOCDelete, placeholders::_1,
                                      placeholders::_2)));
            }
        }

    }

    void CDServer::onResponse(const HeaderOptions &headerOptions,
                              const OC::OCRepresentation &ocResource, const int eCode)
    {
    }

    void CDServer::onOCDelete(const HeaderOptions &headerOptions, const int eCode)
    {
    }
}
