/*
 * BinarySwitchResource.cpp
 */
#include <iostream>
#include <vector>
#include <thread>

#include "BinarySwitchResource.h"
#include "OCPlatform.h"

#define INTERFACE_KEY "if"

using namespace OC;
namespace PH = std::placeholders;

BinarySwitchResource::BinarySwitchResource():
        m_notify_thread_active{true},
        m_notify_thread{nullptr},
        m_send_notification_flag{false},
        m_cv_mutex{},
        m_cv{},
        m_nr_resource_types{1},
        m_nr_resource_interfaces{2},
        m_interestedObservers{},
        m_var_value_rt{},
        m_var_value_n{},
        m_var_value_if{},
        m_var_value_value{}
{
    std::cout << "- Running: BinarySwitchResource constructor" << std::endl;
    std::string resourceURI = "/binaryswitch";

    // VS2013 will fail list initialization so use array initialization
    m_RESOURCE_TYPE[0] = "oic.r.switch.binary";
    m_RESOURCE_INTERFACE[0] = "oic.if.baseline";
    m_RESOURCE_INTERFACE[1] = "oic.if.a";
    m_IF_UPDATE[0] = "oic.if.a";
    m_IF_UPDATE[1] = "oic.if.rw";
    m_IF_UPDATE[2] = "oic.if.baseline";
    m_var_name_rt = "rt";
    m_var_name_n = "n";
    m_var_name_if = "if";
    m_var_name_value = "value";

    // initialize member variables /binaryswitch
    // initialize vector rt
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_n = "";  // current value of property "n"
    // initialize vector if
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");

    m_var_value_value = true; // current value of property "value"

    EntityHandler cb = std::bind(&BinarySwitchResource::entityHandler, this,PH::_1);
    //uint8_t resourceProperty = 0;
    OCStackResult result = OCPlatform::registerResource(m_resourceHandle,
            resourceURI,
            m_RESOURCE_TYPE[0],
            m_RESOURCE_INTERFACE[0],
            cb,
            OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE );

    // add the additional interfaces
    std::cout << "\t" << "# resource interfaces: " << m_nr_resource_interfaces << std::endl;
    std::cout << "\t" << "# resource types     : " << m_nr_resource_types << std::endl;
    for( int a = 1; a < m_nr_resource_interfaces; a++)
    {
        OCStackResult result1 = OCBindResourceInterfaceToResource(m_resourceHandle, m_RESOURCE_INTERFACE[a].c_str());
        if (result1 != OC_STACK_OK)
            std::cerr << "Could not bind interface:" << m_RESOURCE_INTERFACE[a] << std::endl;
    }
    // add the additional resource types
    for( int a = 1; a < m_nr_resource_types; a++ )
    {
        OCStackResult result2 = OCBindResourceTypeToResource(m_resourceHandle, m_RESOURCE_TYPE[a].c_str());
        if (result2 != OC_STACK_OK)
            std::cerr << "Could not bind resource type:" << m_RESOURCE_INTERFACE[a] << std::endl;
    }

    if(OC_STACK_OK != result)
    {
        throw std::runtime_error(
                std::string("BinarySwitchResource failed to start")+std::to_string(result));
    }

    m_send_notification_flag = false;
    m_notify_thread_active = true;
    m_notify_thread = new std::thread(&BinarySwitchResource::notifyObservers, this);

}

BinarySwitchResource::~BinarySwitchResource()
{
    m_notify_thread_active = false;
    m_cv.notify_all();
    if(m_notify_thread->joinable())
    {
        m_notify_thread->join();
    }
}

OC::OCRepresentation BinarySwitchResource::get(OC::QueryParamsMap queries)
{
    OC_UNUSED(queries);

    m_rep.setValue(m_var_name_rt,  m_var_value_rt );
    m_rep.setValue(m_var_name_n, m_var_value_n );
    m_rep.setValue(m_var_name_if,  m_var_value_if );
    m_rep.setValue(m_var_name_value, m_var_value_value );

    return m_rep;
}

OCEntityHandlerResult BinarySwitchResource::post(OC::QueryParamsMap queries, const OC::OCRepresentation& rep)
{
    OCEntityHandlerResult ehResult = OC_EH_OK;
    OC_UNUSED(queries);

    try {
        if (rep.hasAttribute(m_var_name_value))
        {
            // value exist in payload

        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    // TODO add check on array contents out of range, etc..

    if (ehResult == OC_EH_OK)
    {
        // no error: assign the variables

        // array only works for integer, boolean, numbers and strings
        // TODO: make it also work with array of objects
        try {
            if (rep.hasAttribute(m_var_name_rt))
            {
                rep.getValue(m_var_name_rt, m_var_value_rt);
                int first = 1;
                std::cout << "\t\t" << "property 'rt' : " ;
                for(auto myvar: m_var_value_rt)
                {
                    if(first)
                    {
                        std::cout << myvar;
                        first = 0;
                    }
                    else
                    {
                        std::cout << "," << myvar;
                    }
                }
                std::cout <<  std::endl;
                m_send_notification_flag = true;
            }
            else
            {
                std::cout << "\t\t" << "property 'rt' not found in the representation" << std::endl;
            }
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }

        try {
            if (rep.getValue(m_var_name_n, m_var_value_n ))
            {
                std::cout << "\t\t" << "property 'n' : " << m_var_value_n << std::endl;
                m_send_notification_flag = true;
            }
            else
            {
                std::cout << "\t\t" << "property 'n' not found in the representation" << std::endl;
            }
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }

        // array only works for integer, boolean, numbers and strings
        // TODO: make it also work with array of objects
        try {
            if (rep.hasAttribute(m_var_name_if))
            {
                rep.getValue(m_var_name_if, m_var_value_if);
                int first = 1;
                std::cout << "\t\t" << "property 'if' : " ;
                for(auto myvar: m_var_value_if)
                {
                    if(first)
                    {
                        std::cout << myvar;
                        first = 0;
                    }
                    else
                    {
                        std::cout << "," << myvar;
                    }
                }
                std::cout <<  std::endl;
                m_send_notification_flag = true;
            }
            else
            {
                std::cout << "\t\t" << "property 'if' not found in the representation" << std::endl;
            }
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }

        try {
            if (rep.getValue(m_var_name_value, m_var_value_value ))
            {
                std::cout << "\t\t" << "property 'value': " << m_var_value_value << std::endl;
                m_send_notification_flag = true;
            }
            else
            {
                std::cout << "\t\t" << "property 'value' not found in the representation" << std::endl;
            }
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
        if (m_send_notification_flag)
        {
            m_cv.notify_all();
        }
    }
    return ehResult;
}

void BinarySwitchResource::notifyObservers(void) {
    // TODO add code to handle the other resource properties.
    //std::vector<std::string> prev_rt = m_var_value_rt;
    //std::string prev_n = m_var_value_n;
    //std::vector<std::string> prev_if = m_var_value_if;
    bool prev_value = this->m_var_value_value;
    std::unique_lock<std::mutex> cv_lock(m_cv_mutex);
    while (m_notify_thread_active)
    {
        m_cv.wait(cv_lock);
        if(m_send_notification_flag)
        {
            m_send_notification_flag = false;
            std::cout << this->m_var_value_n << " Changed:" << std::endl;
            std::cout << "\t Switch value - from " << (prev_value ? "ON" : "OFF") << " to " << (this->m_var_value_value ? "ON" : "OFF") << std::endl;

            prev_value = this->m_var_value_value;

            OCPlatform::notifyAllObservers(this->m_resourceHandle);
        }
    }
}

OCEntityHandlerResult BinarySwitchResource::entityHandler(std::shared_ptr<OC::OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for BinarySwitchResource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for BinarySwitchResource, URI is : "
                << request->getResourceUri() << std::endl;

        // Check for query params (if any)
        QueryParamsMap queries = request->getQueryParameters();
        if (!queries.empty())
        {
            std::cout << "\nQuery processing up to entityHandler" << std::endl;
        }
        for (auto it : queries)
        {
            std::cout << "Query key: " << it.first << " value : " << it.second
                    << std::endl;
        }
        // get the value, so that we can AND it to check which flags are set
        int requestFlag = request->getRequestHandlerFlag();

        if(requestFlag & RequestHandlerFlag::RequestFlag)
        {
            // request flag is set
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(request->getRequestHandle());
            pResponse->setResourceHandle(request->getResourceHandle());

            if(request->getRequestType() == "GET")
            {
                std::cout<<"BinarySwitchResource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }

            else if(request->getRequestType() == "POST")
            {
                std::cout <<"BinarySwitchResource Post Request"<<std::endl;
                bool  handle_post = true;

                if (queries.size() > 0)
                {
                    for (const auto &eachQuery : queries)
                    {
                        std::string key = eachQuery.first;
                        if (key.compare(INTERFACE_KEY) == 0)
                        {
                            std::string value = eachQuery.second;
                            if (in_updatable_interfaces(value) == false)
                            {
                                std::cout << "Update request received via interface: " << value
                                        << " . This interface is not authorized to update resource!!" << std::endl;
                                pResponse->setResponseResult(OCEntityHandlerResult::OC_EH_FORBIDDEN);
                                handle_post = false;
                                ehResult = OC_EH_ERROR;
                                break;
                            }
                        }
                    }
                }
                if (handle_post)
                {
                    ehResult = post(queries, request->getResourceRepresentation());
                    if (ehResult == OC_EH_OK)
                    {
                        pResponse->setResourceRepresentation(get(queries), "");
                    }
                    else
                    {
                        pResponse->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
                    }
                    if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                    {
                        // TODO: if there are observers inform the observers
                        //OCStackResult sResult;
                        // update all observers with the new value
                        // not sure if this is an blocking call
                        //sResult = OCPlatform::notifyListOfObservers(   m_resourceHandle,
                        //                                               m_interestedObservers,
                        //                                               pResponse);
                    }
                }
            }
            else
            {
                std::cout << "BinarySwitchResource unsupported request type (delete,put,..)"
                        << request->getRequestType() << std::endl;
                pResponse->setResponseResult(OC_EH_ERROR);
                OCPlatform::sendResponse(pResponse);
                ehResult = OC_EH_ERROR;
            }
        }

        if(requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            // observe flag is set
            std::cout << "\t\trequestFlag : Observer\n" << std::endl;
            ObservationInfo observationInfo = request->getObservationInfo();
            if(ObserveAction::ObserveRegister == observationInfo.action)
            {
                // add observer
                m_interestedObservers.push_back(observationInfo.obsId);
            }
            else if(ObserveAction::ObserveUnregister == observationInfo.action)
            {
                // delete observer
                m_interestedObservers.erase(std::remove(
                        m_interestedObservers.begin(),
                        m_interestedObservers.end(),
                        observationInfo.obsId),
                        m_interestedObservers.end());
            }
            ehResult = OC_EH_OK;
        }
    }
    return ehResult;
}
