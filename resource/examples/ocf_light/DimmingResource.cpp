/*
 * DimmingResource.cpp
 */
#include <iostream>

#include "DimmingResource.h"
#include "OCPlatform.h"

#define INTERFACE_KEY "if"

using namespace OC;
namespace PH = std::placeholders;

DimmingResource::DimmingResource():
        m_notify_thread_active{true},
        m_notify_thread{nullptr},
        m_send_notification_flag{false},
        m_cv_mutex{},
        m_cv{},
        m_nr_resource_types{1},
        m_nr_resource_interfaces{2},
        m_interestedObservers{},
        m_var_value_dimmingSetting{100},
        m_var_value_n{},
        m_var_value_if{},
        m_var_value_rt{}
{
    std::cout << "- Running: DimmingResource constructor" << std::endl;
    std::string resourceURI = "/dimming";

    // VS2013 will fail list initialization so use array initialization
    m_RESOURCE_TYPE[0] = "oic.r.light.dimming";
    m_RESOURCE_INTERFACE[0] = "oic.if.baseline";
    m_RESOURCE_INTERFACE[1] = "oic.if.a";
    m_IF_UPDATE[0] = "oic.if.a";
    m_IF_UPDATE[1] = "oic.if.rw";
    m_IF_UPDATE[2] = "oic.if.baseline";
    m_var_name_dimmingSetting = "dimmingSetting";
    m_var_name_n = "n";
    m_var_name_if = "if";
    m_var_name_rt = "rt";

    // initialize member variables /dimming
    m_var_value_dimmingSetting = 0; // current value of property "dimmingSetting"
    m_var_value_n = "";  // current value of property "n"
    // initialize vector if
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    // initialize vector rt
    m_var_value_rt.push_back("oic.r.light.dimming");

    EntityHandler cb = std::bind(&DimmingResource::entityHandler, this,PH::_1);
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
                std::string("DimmingResource failed to start")+std::to_string(result));
    }

    m_send_notification_flag = false;
    m_notify_thread_active = true;
    m_notify_thread = new std::thread(&DimmingResource::notifyObservers, this);
}

DimmingResource::~DimmingResource()
{
    m_notify_thread_active = false;
    m_cv.notify_all();
    if(m_notify_thread->joinable())
    {
        m_notify_thread->join();
    }
}

OC::OCRepresentation DimmingResource::get(OC::QueryParamsMap queries)
{
    OC_UNUSED(queries);

    m_rep.setValue(m_var_name_dimmingSetting, m_var_value_dimmingSetting );
    m_rep.setValue(m_var_name_n, m_var_value_n );
    m_rep.setValue(m_var_name_if,  m_var_value_if );
    m_rep.setValue(m_var_name_rt,  m_var_value_rt );

    return m_rep;
}

OCEntityHandlerResult DimmingResource::post(OC::QueryParamsMap queries, const OC::OCRepresentation& rep)
{
    OCEntityHandlerResult ehResult = OC_EH_OK;
    OC_UNUSED(queries);

    try {
        if (rep.hasAttribute(m_var_name_dimmingSetting))
        {
            // allocate the variable
            int value;
            // get the actual value from the payload
            rep.getValue(m_var_name_dimmingSetting, value);

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

        try {
            // value exist in payload
            if (rep.getValue(m_var_name_dimmingSetting, m_var_value_dimmingSetting ))
            {
                std::cout << "\t\t" << "property 'dimmingSetting': " << m_var_value_dimmingSetting << std::endl;
                m_send_notification_flag = true;
            }
            else
            {
                std::cout << "\t\t" << "property 'dimmingSetting' not found in the representation" << std::endl;
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
        if (m_send_notification_flag)
        {
            m_cv.notify_all();
        }
    }
    return ehResult;
}

void DimmingResource::notifyObservers(void) {
    // TODO add code to handle the other resource properties.
    //std::vector<std::string> prev_rt = m_var_value_rt;
    //std::string prev_n = m_var_value_n;
    //std::vector<std::string> prev_if = m_var_value_if;
    int prev_dimmingSetting = this->m_var_value_dimmingSetting;
    std::unique_lock<std::mutex> cv_lock(m_cv_mutex);
    while (m_notify_thread_active)
    {
        m_cv.wait(cv_lock);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if(m_send_notification_flag)
        {
            m_send_notification_flag = false;
            std::cout << this->m_var_value_n << " Changed:" << std::endl;
            std::cout << "\t Switch value - from " << prev_dimmingSetting << " to " << this->m_var_value_dimmingSetting << std::endl;

            prev_dimmingSetting = this->m_var_value_dimmingSetting;

            OCPlatform::notifyAllObservers(this->m_resourceHandle);
        }
    }
}

OCEntityHandlerResult DimmingResource::entityHandler(std::shared_ptr<OC::OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for DimmingResource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for DimmingResource, URI is : "
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
                std::cout<<"DimmingResource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }

            else if(request->getRequestType() == "POST")
            {
                std::cout <<"DimmingResource Post Request"<<std::endl;
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
                std::cout << "DimmingResource unsupported request type (delete,put,..)"
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
