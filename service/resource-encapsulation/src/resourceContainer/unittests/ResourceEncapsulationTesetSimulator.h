//ResourceEncapsulationTestSimulator.h
#include <memory>
#include <mutex>
#include <atomic>

#include "UnitTestHelper.h"

#include "RCSDiscoveryManager.h"
#include "RCSRemoteResourceObject.h"
#include "RCSResourceAttributes.h"
#include "RCSAddress.h"

//#include "RequestObject.h"

using namespace testing;
using namespace OIC::Service;

class ResourceEncapsulationTestSimulator
    : public std::enable_shared_from_this<ResourceEncapsulationTestSimulator>
{
    public:
        typedef std::shared_ptr<ResourceEncapsulationTestSimulator> Ptr;

        RCSResourceObject::Ptr server;
        RCSRemoteResourceObject::Ptr remoteResource;

    private:
        std::mutex mutexForDiscovery;

        std::string MULTICASTURI;
        std::string RESOURCEURI;
        std::string RESOURCETYPE;
        std::string RESOURCEINTERFACE;
        std::string ATTR_KEY;
        int ATTR_VALUE;

    public:
        ResourceEncapsulationTestSimulator()
            : server(nullptr), remoteResource(nullptr),
              mutexForDiscovery(),
              MULTICASTURI("/oic/res"),
              RESOURCEURI("/a/TempHumSensor/Container"),
              RESOURCETYPE("Resource.Container"),
              RESOURCEINTERFACE("oic.if.baseline"),
              ATTR_KEY("Temperature"),
              ATTR_VALUE(0)
        { }

        ~ResourceEncapsulationTestSimulator()
        {
            if (remoteResource != nullptr && remoteResource->isCaching())
            {
                remoteResource->stopCaching();
            }
            if (remoteResource != nullptr && remoteResource->isMonitoring())
            {
                remoteResource->stopMonitoring();
            }
        }

    private:
        void onDiscoveryResource_Impl(RCSRemoteResourceObject::Ptr resourceObject)
        {
            if (remoteResource != nullptr)
            {
                return;
            }

            if (RESOURCEURI.compare(resourceObject->getUri()) != 0)
            {
                return;
            }

            remoteResource = resourceObject;
            mutexForDiscovery.unlock();
        }

        static void onDiscoveryResource(RCSRemoteResourceObject::Ptr resourceObject,
                                        std::weak_ptr<ResourceEncapsulationTestSimulator> rPtr)
        {
            std::shared_ptr<ResourceEncapsulationTestSimulator> ptr = rPtr.lock();
            if (ptr != nullptr)
            {
                ptr->onDiscoveryResource_Impl(resourceObject);
            }
            else
            {
                std::cout << "Aleady delete simulator\n";
            }
        }
        void waitForDiscovery()
        {
            std::chrono::milliseconds interval(100);
            while (true)
            {
                if (mutexForDiscovery.try_lock())
                {
                    mutexForDiscovery.unlock();
                    return;
                }
                std::this_thread::sleep_for(interval);
            }
        }

    public:
        void defaultRunSimulator()
        {
            createResource();
            discoveryResource();
            waitForDiscovery();
        }

        void createResource()
        {
            server = RCSResourceObject::Builder(RESOURCEURI, RESOURCETYPE, RESOURCEINTERFACE)
                     .setDiscoverable(true).setObservable(true).build();
            server->setAttribute(ATTR_KEY, ATTR_VALUE);
        }

        void discoveryResource()
        {
            discoveryResource(RESOURCETYPE);
        }

        void discoveryResource(std::string &resourceType)
        {
            try
            {
                RCSDiscoveryManager::getInstance()->discoverResourceByType(RCSAddress::multicast(),
                        MULTICASTURI, resourceType,
                        std::bind(onDiscoveryResource,
                                  std::placeholders::_1,
                                  std::weak_ptr<ResourceEncapsulationTestSimulator>(shared_from_this())));
                mutexForDiscovery.lock();
            }
            catch (std::exception &e)
            {
                std::cout << "exception : " << e.what() << std::endl;
            }
        }

        std::string getServerUri() const
        {
            return RESOURCEURI;
        }

        RCSResourceObject::Ptr getResourceServer() const
        {
            return server;
        }

        RCSRemoteResourceObject::Ptr getRemoteResource() const
        {
            return remoteResource;
        }

        void ChangeAttributeValue()
        {
            std::chrono::milliseconds interval(100);
            if (server != nullptr)
                server->setAttribute(ATTR_KEY, ATTR_VALUE + 10);
            std::this_thread::sleep_for(interval);
        }

        void ChangeResourceState()
        {
            std::chrono::milliseconds interval(400);
            if (server != nullptr)
                server = nullptr;
            std::this_thread::sleep_for(interval);
        }

};
