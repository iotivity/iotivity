/*
 * ResourceBroker.cpp
 *
 */

#include "ResourceBroker.h"

#define OIC_COAP "coap://"
#define DEFAULT_CONTEXT_VALUE 0x99

typedef std::function<OCStackApplicationResult(void *, OCDoHandle, OCClientResponse *)> FindCB;

ResourceBroker * ResourceBroker::s_instance = null;

class DiscoverResource: public PrimitiveResource {
public:
    DiscoverResource();
    DiscoverResource(PrimitiveResource & pResource, DiscoverCB _cb) :
            Uri(pResource.getUri()), Address(pResource.getAddress()), discovercb(_cb) {
    pFindCB = (FindCB)(std::bind(&DiscoverResource::findCB,this,std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        }
    DiscoverResource(std::string _Uri, std::string _Address, DiscoverCB _cb) :
            Uri(_Uri), Address(_Address), discovercb(_cb) {
    pFindCB = (FindCB)(std::bind(&DiscoverResource::findCB, this,std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        }
    ~DiscoverResource();
    const DiscoverCB discovercb;
    const FindCB pFindCB;

private:
    OCStackApplicationResult findCB(void * ctx, OCDoHandle handle,OCClientResponse * clientResponse)
    {
        OCStackApplicationResult ret = OC_STACK_DELETE_TRANSACTION;
        // parse about resource info

        // create primitiveResource
        PrimitiveResource * retResource = new PrimitiveResource(this->Uri, this->Address);
        // callback
        this->discovercb(retResource);
        //why using auto_ptr?
        std::auto_ptr bye_bye(this);
        /////////////////////////////////////////////////////////////////////////////
        std::cout << "Is Possible?? : " << this->getAddress() << std::endl;
        /////////////////////////////////////////////////////////////////////////////
        return ret;
     }
};

        ResourceBroker::ResourceBroker() {
            // TODO Auto-generated constructor stub

        }

        ResourceBroker::~ResourceBroker() {
            // TODO Auto-generated destructor stub
        }

        ResourceBroker * ResourceBroker::getInstance() {
            if (!s_instance) {
                s_mutexForCreation.lock();
                if (!s_instance) {
                    s_instance = new ResourceBroker();
                }
                s_mutexForCreation.unlock();
            }
            return s_instance;
        }

        OCStackResult ResourceBroker::discoverResource(const std::string _uri,
                const std::string _address, DiscoverCB _cb) {
            OCStackResult ret = OC_STACK_INVALID_PARAM;

            if (_uri.empty() || _address.empty()) {
                return ret;
            }
            if (!_cb) {
                ret = OC_STACK_INVALID_CALLBACK;
                return ret;
            }

            DiscoverResource * dResource = new DiscoverResource(_uri, _address, _cb);
            OCDoHandle handle;
            OCCallbackData cbData;
            std::string fullAddress = OIC_COAP + _address;

            cbData.cb = dResource->pFindCB;
            cbData.context = (void *) DEFAULT_CONTEXT_VALUE;
            cbData.cd = NULL;

            return OCDoResource(&handle, OC_REST_GET, fullAddress.c_str(), _uri.c_str(), 0, OC_ALL,
                    OC_LOW_QOS, &cbData, NULL, 0);
        }

        OCStackResult ResourceBroker::discoverResource(PrimitiveResource & pResource,
                DiscoverCB _cb) {
            return this->discoverResource(pResource.getUri(), pResource.getAddress(), _cb);
        }

        OCStackResult ResourceBroker::hostResource(const std::string _uri,const std::string _address, BrokerCB _cb)
        {
            OCStackResult ret = OC_STACK_INVALID_PARAM;
            if (uri.empty() || address.empty()) {
                return ret;
            }
            if (!cb) {
                ret = OC_STACK_INVALID_CALLBACK;
                return ret;
            }

            return ret;
        }
        OCStackResult ResourceBroker::hostResource(PrimitiveResource & pResource, BrokerCB cb)
        {
            OCStackResult ret = OC_STACK_INVALID_PARAM;
            if (pResource.getUri().empty() || pResource.getAddress().empty()) {
                return ret;
            }
            if (!cb) {
                ret = OC_STACK_INVALID_CALLBACK;
                return ret;
            }

            return ret;
        }
        bool ResourceBroker::isPrimitiveResource(PrimitiveResource& pResource, BrokerCB cb)
        {
            if(s_presenceInfo.empty())
                return false;
            else
            {
                std::list<PresenceInfo*>::iterator it;
                PresenceInfo item = new PresenceInfo(pResource.getUri(), pResource.getAddress(), cb);
                item.PrimitiveResource(pResource.getUri(),pResource.getAddress());
                it = std::find(s_presenceInfo.begin(),s_presenceInfo.end(),item);
                if(it==s_presenceInfo.end())
                    return false;
                else
                    return true;
            }
        }
