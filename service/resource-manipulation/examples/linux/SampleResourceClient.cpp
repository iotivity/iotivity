#include<iostream>

#include "ResourceClient.h"
#include "ResourceAttributes.h"
#include "OCPlatform.h"

using namespace std;
using namespace OC;
using namespace OIC::Service;

std::shared_ptr<RemoteResourceObject>  resource;
ResourceAttributes resourceAttributes;
bool startCachingFlag;
bool isReady;

//callback function for discoverResource()
void OnResourceDiscovered(std::shared_ptr<RemoteResourceObject> foundResource)
{

    cout << "\nOnResourceDiscovered callback" << std::endl;

    std::string resourceURI = foundResource->getUri();
    std::string hostAddress = foundResource->getAddress();

    cout << "\tResource URI : " << resourceURI << std::endl;
    cout << "\tResource Host : " << hostAddress << std::endl;

    resource = foundResource;
    isReady = true;
}

//callback for StartWatching()
void OnResourceStateChanged(ResourceState resourceState)
{

    cout << "\nOnResourceStateChanged callback" << std::endl;

    if (resourceState == ResourceState::NOT_WATCHING)
        cout << "State changed to : NOT_WATCHING" << std::endl;
    else if (resourceState == ResourceState::ALIVE)
        cout << "State changed to : ALIVE" << std::endl;
    else if (resourceState == ResourceState::REQUESTED)
        cout << "State changed to : REQUESTED" << std::endl;
    else if (resourceState == ResourceState::LOST_SIGNAL)
        cout << "State changed to : LOST_SIGNAL" << std::endl;
    else if (resourceState == ResourceState::DESTROYED)
        cout << "State changed to : DESTROYED" << std::endl;
}

//callback for startCaching() [uptodate]
void OnCacheUpdated(const ResourceAttributes atttribute )
{
    cout << "\nOnCacheUpdated callback" << std::endl;
    if (atttribute.empty())
    {
        std::cout << "Attribute is Empty" << std::endl;
    }
    else
    {
        ResourceAttributes::const_iterator iter = atttribute.begin();
        for (unsigned int i = 0; i < atttribute.size(); ++i)
        {
            std::cout << "key : " << iter->key() << "\nvalue : " << iter->value().toString() << std::endl;
            ++iter;
        }
    }

}
//callback for getRemoteAttributes()
void OnRemoteAttributesReceivedCallback(const ResourceAttributes &atttribute)
{

    std::cout << "\nOnRemoteAttributesReceivedCallback callback" << std::endl;
    if (atttribute.empty())
    {
        std::cout << "Got empty attribute " << std::endl;
    }
    else
    {
        resourceAttributes = atttribute;
        ResourceAttributes::const_iterator iter = atttribute.begin();
        for (unsigned int i = 0; i < atttribute.size(); ++i)
        {
            std::cout << "key : " << iter->key() << "\nvalue : " << iter->value().toString() << std::endl;
            ++iter;
        }
    }
}

//callback for setRemoteAttributes()
void OnRemoteAttributesSetCallback(const ResourceAttributes &atttribute)
{

    std::cout << "\nOnRemoteAttributesSetCallback callback" << std::endl;
    if (atttribute.empty())
    {
        std::cout << "Got empty attribute " << std::endl;
    }
    else
    {
        resourceAttributes = atttribute;
        ResourceAttributes::const_iterator iter = atttribute.begin();
        for (unsigned int i = 0; i < atttribute.size(); ++i)
        {
            std::cout << "key : " << iter->key() << "\nvalue : " << iter->value().toString() << std::endl;
            ++iter;
        }
    }
}

int main()
{

    DiscoveryManager *discoveryManagerInstance =  DiscoveryManager::getInstance();
    bool cachingFlag = false;

    //configuring the platform
    PlatformConfig config
    {
        OC::ServiceType::InProc, ModeType::Client, "0.0.0.0", 0, OC::QualityOfService::LowQos
    };
    OCPlatform::Configure(config);

    std::cout << "\nPlatform configured successfully" << std::endl;
    std::string uri = "";
    std::string address = "";
    std::string rt = "core.TemperatureSensor";

    try
    {
        uri = OC_MULTICAST_DISCOVERY_URI + uri + "?rt=" + rt;
        //discover the resource in the network
        discoveryManagerInstance->discoverResource(address, uri , CT_DEFAULT, &OnResourceDiscovered);
    }
    catch (InvalidParameterException e)
    {
        cout << "Exeception in discoverResource" << e.what() << std::endl;
    }

    bool isRun = true;
    int userInput;
    while (isRun)
    {
        while (isReady)
        {
            cout << endl;
            cout << "1 :: Start Hosting" << endl;
            cout << "2 :: Stop Hosting" << endl;
            cout << "3 :: Get Attribute" << endl;
            cout << "4 :: Set Attribute" << endl;
            cout << "5 :: Start caching (No update to Application)" << endl;
            cout <<  "6 :: Start caching (Update the application when data change)" <<
                 endl; //look for the datachange on server
            cout << "7 :: Get Resource cache State" << endl;
            cout << "8 :: Get Cached Attributes" << endl;
            cout << "9 :: Get Cached Attribute"  << endl;
            cout << "10 :: Stop caching" << endl;
            cout << "11 :: QUIT" << endl;

            cin >> userInput;

            if (userInput == 1)
            {
                try
                {
                    resource->startWatching(&OnResourceStateChanged);
                    cout << "\n\n**********  Hosting Started ***********" << std::endl;
                }
                catch (InvalidParameterException e)
                {
                    cout << "Exeception in startWatching :: " << e.what() << std::endl;
                }
            }
            else if (userInput == 2)
            {
                resource->stopWatching();
                cout << "\n\n******  Hosting stopped******" << std::endl;
            }
            else if (userInput == 3)
            {
                resource->getRemoteAttributes(&OnRemoteAttributesReceivedCallback);
            }
            else if (userInput == 4)
            {
                int temperatureValue;
                if (0 == resourceAttributes.size())
                {
                    cout << "\n***First Get the Attributes from Remote Device : press 3 to get attributes***" <<
                         std::endl;
                }
                else
                {
                    ResourceAttributes::const_iterator iter = resourceAttributes.begin();
                    for (unsigned int i = 0; i < resourceAttributes.size(); ++i)
                    {
                        if ( iter->key() == "Temperature")
                        {
                            cout << "Enter the value you want to set :";
                            cin >> temperatureValue;
                            resourceAttributes["Temperature"]  = temperatureValue;
                            resource->setRemoteAttributes(resourceAttributes, &OnRemoteAttributesSetCallback);
                        }
                        ++iter;
                    }
                }
            }
            else if (userInput == 5)
            {
                if (false == cachingFlag)
                {
                    resource->startCaching();
                    cout << "**********  caching Started ***********" << std::endl;
                    cachingFlag = true;
                }
                else
                {
                    cout << "***  Already Started... To start it again first stop it : press 10 ***" << std::endl;
                }
            }
            else if (userInput == 6)
            {
                try
                {
                    if (false == cachingFlag)
                    {
                        resource->startCaching(&OnCacheUpdated);
                        cout << "**********  caching Started ***********" << std::endl;
                    }
                    else
                    {
                        cout << "***  Already Started... To start it again first stop it : press 10 ***" << std::endl;
                    }
                }
                catch (InvalidParameterException e)
                {
                    cout << "Exeception in startCaching :: " << e.what() << std::endl;
                }
            }
            else if (userInput == 7)
            {

                CacheState state = resource->getResourceCacheState();
                if (state == CacheState ::READY)
                    cout << "Current Cache State : " << "CACHE_STATE ::READY" << std::endl;
                else if (state == CacheState ::READY_YET)
                    cout << "Current Cache State : " << "CACHE_STATE ::READY_YET" << std::endl;
                else if (state == CacheState ::LOST_SIGNAL)
                    cout << "Current Cache State : " << "CACHE_STATE ::LOST_SIGNAL" << std::endl;
                else if (state == CacheState ::DESTROYED)
                    cout << "Current Cache State : " << "CACHE_STATE ::DESTROYED" << std::endl;
                else if (state == CacheState ::UPDATING)
                    cout << "Current Cache State : " << "CACHE_STATE ::UPDATING" << std::endl;
                else if (state == CacheState ::NONE)
                    cout << "Current Cache State : " << "CACHE_STATE ::NONE" << std::endl;
            }
            else if (userInput == 8)
            {
                try
                {
                    ResourceAttributes atttribute = resource->getCachedAttributes();
                    if (atttribute.empty())
                    {
                        cout << "Received cached attribute is empty" << std::endl;
                    }
                    else
                    {
                        ResourceAttributes::const_iterator iter = atttribute.begin();
                        for (unsigned int i = 0; i < atttribute.size(); ++i)
                        {
                            std::cout << "\nkey : " << iter->key() << "\nvalue : " << iter->value().toString() << std::endl;
                            ++iter;
                        }
                    }
                }
                catch (BadRequestException e)
                {
                    cout << "getCachedAttributes exception : " << e.what() << std::endl;
                }
            }
            else if (userInput == 9)
            {
                std::string key = "Temperature";
                try
                {
                    ResourceAttributes::Value valueObj = resource->getCachedAttribute(key);
                    int value = valueObj.get< int >();
                    cout << "\nkey : " << key << "\nValue : " << value << std::endl;
                }
                catch (BadRequestException e)
                {
                    cout << "getCachedAttribute exception : " << e.what() << std::endl;
                }
                catch (BadGetException e)
                {
                    cout << "Exeception in getCachedAttribute  BadGetException:: " << e.what() << std::endl;
                }
            }
            else if (userInput == 10)
            {
                resource->stopCaching();
                cachingFlag = false;
                cout << "****** Caching stopped ******" << std::endl;
            }
            else if (userInput == 11)
            {
                isReady = false;
                isRun = false;
            }
            else
            {
                cout << "***   Please enter the number between 1-11  ***" << std::endl;
            }
        }
    }
    return 0;
}

