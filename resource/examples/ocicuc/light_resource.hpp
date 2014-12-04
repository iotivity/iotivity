#ifndef __LIGHT_RESOURCE_HPP
 #define __LIGHT_RESOURCE_HPP

#include <map>
#include <atomic>
#include <thread>
#include <string>
#include <ostream>
#include <sstream>
#include <iostream>
#include <functional>

#include "OCApi.h"
#include "OCResource.h"
#include "OCPlatform.h"

/* An example of a server-side resource: */
namespace Intel { namespace OCDemo {

using namespace OC;
using namespace std;

/// This class represents a single resource named 'lightResource'. This resource has
/// two simple properties named 'state' and 'power'
class LightResource
{
 public:
    bool m_state;       // off or on?
    int m_power;        // power level
    OCRepresentation m_rep;

    private:
    atomic<bool> m_observation; // are we under observation?

    private:
    static atomic<bool> shutdown_flag;
    static thread observe_thread;

    private:
    OCResourceHandle m_resourceHandle;

    public:
    LightResource()
     : m_state(false),
       m_power(0),
       m_observation(false)
    {}

    ~LightResource()
    {
        shutdown_flag = true;

        if(observe_thread.joinable())
         observe_thread.join();
    }

    private:
    inline std::string make_URI(const unsigned int resource_number)
    {
        std::string uri = std::string("/a/light") + "_" + std::to_string(resource_number);
        m_rep.setUri(uri);
        return uri;
    }

    public:
    // This function internally calls registerResource API.
    void createResource(const unsigned int resource_number);
    void unregisterResource();
    OCResourceHandle getHandle() const { return m_resourceHandle; }

    void setRepresentation(const OCRepresentation& rep);
    OCRepresentation getRepresentation(void);

    void addType(const std::string& type) const;
    void addInterface(const std::string& interface) const;

    private:
    OCEntityHandlerResult entityHandler(std::shared_ptr<OCResourceRequest> request);

    private:
    void observe_function();

    // Request handlers:
    private:
    void dispatch_request(const std::string& request_type, std::shared_ptr<OCResourceRequest> request);
    void handle_get_request(std::shared_ptr<OCResourceRequest> request);
    void handle_put_request(std::shared_ptr<OCResourceRequest> request);
    void handle_post_request(std::shared_ptr<OCResourceRequest> request);
    void handle_delete_request(std::shared_ptr<OCResourceRequest> request);
    void handle_observe_event(std::shared_ptr<OCResourceRequest> request);
};

}} // namespace Intel::OCDemo

#endif
