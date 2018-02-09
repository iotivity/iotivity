/*
 * DimmingResource.h
 */

#ifndef EXAMPLE_OCF_LIGHT_DIMMINGRESOURCE_H_
#define EXAMPLE_OCF_LIGHT_DIMMINGRESOURCE_H_

#include <vector>
#include <atomic>
#include <thread>
#include <condition_variable>

#include "Resource.h"

#include "OCApi.h"
#include "OCRepresentation.h"

class DimmingResource : public Resource
{
public:
    /*
     * constructor
     */
    DimmingResource();
    virtual ~DimmingResource();
private:

    /*
     * function to make the payload for the retrieve function (e.g. GET) /dimming
     * @param queries  the query parameters for this call
     */
    OC::OCRepresentation get(OC::QueryParamsMap queries);

    /*
     * function to parse the payload for the update function (e.g. POST) /dimming
     * @param queries  the query parameters for this call
     * @param rep  the response to get the property values from
     * @return OCEntityHandlerResult ok or not ok indication
     */
    OCEntityHandlerResult post(OC::QueryParamsMap queries, const OC::OCRepresentation& rep);


    std::atomic<bool> m_notify_thread_active;
    std::thread* m_notify_thread;

    std::atomic<bool> m_send_notification_flag;
    std::mutex m_cv_mutex;
    std::condition_variable m_cv;
    void notifyObservers(void);

    // resource types and interfaces as array..
    std::string m_RESOURCE_TYPE[1]; // rt value (as an array)
    std::string m_RESOURCE_INTERFACE[2]; // interface if (as an array)
    std::string m_IF_UPDATE[3]; // updateble interfaces
    int m_nr_resource_types;
    int m_nr_resource_interfaces;
    OC::ObservationIds m_interestedObservers;

    // member variables for path: /dimming
    int m_var_value_dimmingSetting; // the value for the attribute
    std::string m_var_name_dimmingSetting; // the name for the attribute

    std::string m_var_value_n; // the value for the attribute
    std::string m_var_name_n; // the name for the attribute


    std::vector<std::string>  m_var_value_if;
    std::string m_var_name_if; // the name for the attribute


    std::vector<std::string>  m_var_value_rt;
    std::string m_var_name_rt; // the name for the attribute
protected:
    /*
     * function to check if the interface is
     * @param  interface_name the interface name used during the request
     * @return true: updatable interface
     */
    bool in_updatable_interfaces(std::string interface_name)
    {
        for (int i=0; i<3; i++)
        {
            if (m_IF_UPDATE[i].compare(interface_name) == 0)
                return true;
        }
        return false;
    }

    /*
     * the entity handler for this resource
     * @param request the incoming request to handle
     * @return OCEntityHandlerResult ok or not ok indication
     */
    virtual OCEntityHandlerResult entityHandler(std::shared_ptr<OC::OCResourceRequest> request);
};

#endif /* EXAMPLE_OCF_LIGHT_DIMMINGRESOURCE_H_ */
