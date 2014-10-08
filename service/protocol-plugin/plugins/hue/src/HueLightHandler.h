#include <functional>

#include <pthread.h>

#include "OCPlatform.h"
#include "OCApi.h"


namespace OIC
{
    void resourceHandler(std::shared_ptr<OC::OCResource> resource);
    class LightResource
    {
        public:
            bool m_state;
            int m_color;
            int m_bright;
            OCResourceHandle m_resourceHandle;

            std::string resourceURL;  // URI of the resource
            std::string resourceTypeName; // resource type name. In this case, it is light
            std::string resourceInterface; // resource interface.
            std::string ext_url;

            // OCResourceProperty is defined ocstack.h
            uint8_t resourceProperty;

        public:
            /// Constructor
            LightResource(string &resUrl, string &resType, string &resInterface, string e_url);
            LightResource();
            virtual ~LightResource();

            /* Note that this does not need to be a member function: for classes you do not have
            access to, you can accomplish this with a free function: */

            // This function internally calls registerResource API.
            OCStackResult findResource(OC::OCPlatform &platform);
            OCStackResult registerResource(OC::OCPlatform &platform,
                                           std::function<void(const OC::OCResourceRequest::Ptr, const OC::OCResourceResponse::Ptr)>
                                           entityHandler);
//            void unregisterResource(OC::OCPlatform& platform);
//
            OCResourceHandle getHandle();
            void setRepresentation(OC::OCRepresentation &light);
            OC::OCRepresentation getRepresentation();
            OCStackResult addType(const OC::OCPlatform &platform, const std::string &type);
            OCStackResult addInterface(const OC::OCPlatform &platform, const std::string &interface);
    };

    class LightContainer: public LightResource
    {
        public:
            vector<LightResource> m_container;
            LightContainer(string &resUrl, string &resType, string &resInterface);
            virtual ~LightContainer();
    };

}
