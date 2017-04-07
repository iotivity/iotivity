#include <memory>
#include <iostream>
#include <stdexcept>
#include <condition_variable>
#include <map>
#include <vector>
#include <string>
#include <unistd.h>

#include "ocstack.h"
#include "ocpayload.h"
#include "RDClient.h"

#include <OCApi.h>
#include <OCPlatform.h>

#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
#include "ocprovisioningmanager.h"
#include "mbedtls/ssl_ciphersuites.h"
#include <ca_adapter_net_ssl.h>
#endif // WITH_DTLS__ or __WITH_TLS__

#define maxSequenceNumber 0xFFFFFF

using namespace OC;
using namespace std;

string g_host;
string g_invitedGroup;
condition_variable g_callbackLock;

class LightResource
{
public:
    /// Access this property from a TB client
    std::string m_power;
    std::string m_lightUri;
    OCResourceHandle m_resourceHandle;
    OCRepresentation m_lightRep;

    /// Constructor
    LightResource() :
            m_power(""), m_lightUri("/a/light")
    {
        // Initialize representation
        m_lightRep.setUri(m_lightUri);
        m_lightRep.setValue("power", m_power);
    }

    /// This function internally calls registerResource API.
    void createResource()
    {
        std::string resourceURI = m_lightUri; //URI of the resource
        std::string resourceTypeName = "core.light"; //resource type name. In this case, it is light
        std::string resourceInterface = DEFAULT_INTERFACE; // resource interface.

        EntityHandler cb = std::bind(&LightResource::entityHandler, this, std::placeholders::_1);

        // This will internally create and register the resource.
        OCStackResult result = OCPlatform::registerResource(m_resourceHandle, resourceURI,
                resourceTypeName, resourceInterface, cb, OC_DISCOVERABLE | OC_OBSERVABLE);

        if (OC_STACK_OK != result)
        {
            cout << "Resource creation was unsuccessful\n";
        }
    }

    OCRepresentation post(OCRepresentation &rep)
    {
        m_power = rep.getValueToString("power");
        return get();
    }

    // gets the updated representation.
    // Updates the representation with latest internal state before
    // sending out.
    OCRepresentation get()
    {
        m_lightRep.setValue("power", m_power);

        return m_lightRep;
    }

private:
    // This is just a sample implementation of entity handler.
    // Entity handler can be implemented in several ways by the manufacturer
    OCEntityHandlerResult entityHandler(std::shared_ptr< OCResourceRequest > request)
    {
        cout << "\tIn Server CPP entity handler:\n";
        OCEntityHandlerResult ehResult = OC_EH_ERROR;
        if (request)
        {
            // Get the request type and request flag
            std::string requestType = request->getRequestType();
            int requestFlag = request->getRequestHandlerFlag();

            if (requestFlag & RequestHandlerFlag::RequestFlag)
            {
                cout << "\t\trequestFlag : Request\n";
                auto pResponse = std::make_shared< OC::OCResourceResponse >();
                pResponse->setRequestHandle(request->getRequestHandle());
                pResponse->setResourceHandle(request->getResourceHandle());

                // If the request type is GET
                if (requestType == "GET")
                {
                    cout << "\t\t\trequestType : GET\n";
                    pResponse->setResponseResult(OC_EH_OK);
                    pResponse->setResourceRepresentation(get());
                    if (OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                    {
                        ehResult = OC_EH_OK;
                    }
                }
                else if (requestType == "POST")
                {
                    cout << "\t\t\trequestType : POST\n";

                    OCRepresentation rep = request->getResourceRepresentation();

                    // Do related operations related to POST request
                    OCRepresentation rep_post = post(rep);
                    pResponse->setResourceRepresentation(rep_post);

                    if (OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                    {
                        ehResult = OC_EH_OK;
                    }
                }
                else
                {
                    std::cout << "Unhandled request type" << std::endl;
                }
            }
        }
        else
        {
            std::cout << "Request invalid" << std::endl;
        }

        return ehResult;
    }
};

void printRepresentation(OCRepresentation rep)
{
    for (auto itr = rep.begin(); itr != rep.end(); ++itr)
    {
        cout << "\t" << itr->attrname() << ":\t" << itr->getValueToString() << endl;
		if (!strcmp(itr->attrname().c_str(), "gid")) {
			g_invitedGroup = itr->getValueToString();
		}
        if (itr->type() == AttributeType::Vector)
        {
            switch (itr->base_type())
            {
                case AttributeType::OCRepresentation:
                    for (auto itr2 : (*itr).getValue< vector< OCRepresentation > >())
                    {
                        printRepresentation(itr2);
                    }
                    break;

                case AttributeType::Integer:
                    for (auto itr2 : (*itr).getValue< vector< int > >())
                    {
                        cout << "\t\t" << itr2 << endl;
                    }
                    break;

                case AttributeType::String:
                    for (auto itr2 : (*itr).getValue< vector< string > >())
                    {
                        cout << "\t\t" << itr2 << endl;
                    }
                    break;

                default:
                    cout << "Unhandled base type " << itr->base_type() << endl;
                    break;
            }
        }
        else if (itr->type() == AttributeType::OCRepresentation)
        {
            printRepresentation((*itr).getValue< OCRepresentation >());
        }
    }
}

void getResource(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    cout << "Resource get: " << ecode << endl;

    printRepresentation(rep);
}

void foundMyDevice(shared_ptr< OC::OCResource > resource)
{
    cout << "Device found: " << resource->uri() << endl;
    cout << "DI: " << resource->sid() << endl;

    g_callbackLock.notify_all();
}

void foundDevice(shared_ptr< OC::OCResource > resource)
{
    vector < string > rt = resource->getResourceTypes();

    cout << "Device found: " << resource->uri() << endl;
    cout << "DI: " << resource->sid() << endl;

    QueryParamsMap query;
    resource->get(query, &getResource);
}

void onObserveGroup(const HeaderOptions /*headerOptions*/, const OCRepresentation &rep,
        const int &eCode, const int /*&sequenceNumber*/)
{
    cout << "onObserveGroup response received code: " << eCode << endl;

    if (eCode == OC_STACK_OK)
    {
        printRepresentation(rep);

        vector < string > dilist = rep.getValue < vector< string > > ("dilist");

        for (auto itr = dilist.begin(); itr != dilist.end(); ++itr)
        {
            cout << (*itr) << " discovered" << endl;
            if ((*itr) != OCGetServerInstanceIDString())
            {
                cout << "New device joined" << endl;
                string query = "/oic/res?di=";
                query += (*itr);
                OCStackResult result = OC_STACK_ERROR;

                cout << "find my resource : " << *itr << endl;
                result = OCPlatform::findResource(g_host, query,
                        static_cast< OCConnectivityType >(CT_ADAPTER_TCP | CT_IP_USE_V4),
                        &foundDevice);
                cout << " result: " << result << endl;
                break;
            }
        }
    }
    g_callbackLock.notify_all();
}

void onInvite(const HeaderOptions /*headerOptions*/, const OCRepresentation &rep, const int &eCode,
        const int &sequenceNumber)
{
    cout << "onInvite response received code: " << eCode << endl;

	if (eCode == OC_STACK_OK && sequenceNumber <= MAX_SEQUENCE_NUMBER)
	{
		if (sequenceNumber == OC_OBSERVE_REGISTER)
		{
			cout << "Observe registration action is successful" << endl;
		}

		cout << "OBSERVE RESULT:" << endl;
		printRepresentation(rep);

	}
	else
	{
		if (eCode == OC_STACK_OK)
		{
			cout << "Observe registration failed or de-registration action failed/succeeded"
				<< endl;
		}
		else
		{
			cout << "onObserve Response error: " << eCode << endl;
			exit(-1);
		}
	}

	g_callbackLock.notify_all();
}

string g_gid;
void onCreateGroup(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    cout << "onCreateGroup response received code: " << ecode << endl;

    if (ecode == 4)
    {
        printRepresentation(rep);
        g_gid = rep.getValueToString("gid");
    }

    g_callbackLock.notify_all();
}

void onPublish(const OCRepresentation &, const int &eCode)
{
    cout << "Publish resource response received, code: " << eCode << endl;
    g_callbackLock.notify_all();
}

void onPost(const HeaderOptions & /*headerOptions*/, const OCRepresentation &rep, const int eCode)
{
    if (eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_CHANGED)
    {
        cout << "\tRequest was successful: " << eCode << endl;

        printRepresentation(rep);
    }
    else
    {
        cout << "\tResponse error: " << eCode << endl;
    }

    g_callbackLock.notify_all();
}

void onDelete(const HeaderOptions & /*headerOptions*/, const int eCode)
{
	if (eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_DELETED)
	{
		cout << "\tDelete was successful" << endl;
	}
	else
	{
		cout << "\tDelete Response error: " << eCode << endl;
	}
}

string g_uid;
string g_accesstoken;

void handleLoginoutCB(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    cout << "Auth response received code: " << ecode << endl;

    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep);
    }

    if (ecode == 4)
    {
        g_accesstoken = rep.getValueToString("accesstoken");

        g_uid = rep.getValueToString("uid");
    }

    g_callbackLock.notify_all();
}

string g_option;

#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
int saveTrustCert(void)
{
	OCStackResult res = OC_STACK_ERROR;
	uint16_t g_credId = 0;

	cout << "Save Trust Cert. Chain into Cred of SVR" << endl;

	ByteArray trustCertChainArray = { 0, 0 };

	FILE *fp = fopen("rootca.crt", "rb+");

	if (fp)
	{
		size_t fsize;
		if (fseeko(fp, 0, SEEK_END) == 0 && (fsize = ftello(fp)) > 0)
		{
			trustCertChainArray.data = (uint8_t *)malloc(fsize);
			trustCertChainArray.len = fsize;
			if (NULL == trustCertChainArray.data)
			{
				cout << "Failed to allocate memory" << endl;
				fclose(fp);
				return res;
			}
			rewind(fp);
			if (fsize != fread(trustCertChainArray.data, 1, fsize, fp))
			{
				cout << "Certiface not read completely" << endl;
			}
			fclose(fp);
		}
	}

	res = OCSaveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len, OIC_ENCODING_PEM,
		&g_credId);

	if (OC_STACK_OK != res)
	{
		cout << "OCSaveTrustCertChainBin API error" << endl;
		return res;
	}
	cout << "CredId of Saved Trust Cert. Chain into Cred of SVR : " << g_credId << endl;

	return res;
}
#endif

static FILE *client_open(const char *path, const char *mode)
{
	if (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
	{
		return fopen("./group_light_share.dat", mode);
	}
	else
	{
		return fopen(path, mode);
	}
}

int main(int argc, char **argv)
{
    if (argc != 5)
    {
		cout
			<< "Put \"[host-ipaddress:port] [uid] [accessToken] [\'owner\'|\'member\']\" for sign-up and sign-in"
			<< endl;
        return 0;
    }

	g_option = argv[argc-1];

//   OCPersistentStorage ps
//    { client_open, fread, fwrite, fclose, unlink };

    PlatformConfig cfg
    { ServiceType::InProc, ModeType::Both, "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
            0, // Uses randomly available port
            QualityOfService::LowQos, 0 };

    OCPlatform::Configure(cfg);

    OCStackResult result = OC_STACK_ERROR;

	g_host = "coap+tcp://";


#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
	g_host = "coaps+tcp://";
#endif

	g_host += argv[1];

	OCAccountManager::Ptr accountMgr = OCPlatform::constructAccountManagerObject(g_host,
		CT_ADAPTER_TCP);


#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
	cout << "Security Mode" << endl;
	if (CA_STATUS_OK != saveTrustCert())
	{
		cout << "saveTrustCert returned an error" << endl;
	}

	uint16_t cipher = MBEDTLS_TLS_RSA_WITH_AES_128_GCM_SHA256;
	if (CA_STATUS_OK != CASelectCipherSuite(cipher, CA_ADAPTER_TCP))
	{
		cout << "CASelectCipherSuite returned an error" << endl;
	}
#endif

    mutex blocker;
    unique_lock < mutex > lock(blocker);

	if (strlen(argv[2]) > 35)
    {
        accountMgr->signIn(argv[2], argv[3], &handleLoginoutCB);
        g_callbackLock.wait(lock);
    }
    else
    {
        accountMgr->signUp(argv[2], argv[3], &handleLoginoutCB);
        g_callbackLock.wait(lock);
        accountMgr->signIn(g_uid, g_accesstoken, &handleLoginoutCB);
        g_callbackLock.wait(lock);
    }

    string cmd;

    LightResource lightResource;
    lightResource.createResource();

    ResourceHandles resourceHandles;
    resourceHandles.push_back(lightResource.m_resourceHandle);

	result = RDClient::Instance().publishResourceToRD(g_host, OCConnectivityType::CT_ADAPTER_TCP, resourceHandles,
            &onPublish);

	cout << " result: " << result << " Waiting Publish user resource response from cloud" << endl;

    g_callbackLock.wait(lock);
// TODO: need to modify the below according to the OCAccountManager API changed.
    if (g_option == "owner")
    {
        cout << "Creating group" << endl;
		QueryParamsMap queryParam =
		{};
		queryParam.insert(pair< string, string >("gname", "test"));
		accountMgr->createGroup(queryParam, &onCreateGroup);
        g_callbackLock.wait(lock);
        cout << "Adding device " << OCGetServerInstanceIDString() << " to group " << g_gid << endl;

//		accountMgr->addDeviceToGroup(g_gid,
//        { OCGetServerInstanceIDString() }, &onPost);
		OCRepresentation propertyValue;
		vector< string > values;
		values.push_back(OCGetServerInstanceIDString());

		propertyValue.setValue < vector < string >>("devices", values);

		accountMgr->addPropertyValueToGroup(g_gid,
			propertyValue, &onPost);
        g_callbackLock.wait(lock);


        accountMgr->observeGroup(&onObserveGroup);
        g_callbackLock.wait(lock);
        cout << "Put userUUID to send invitation" << endl;
        cin >> cmd;
        cout << "Group id : " << g_gid << " send invitation to " << cmd << endl;
        accountMgr->sendInvitation(g_gid, cmd, &onPost);
        g_callbackLock.wait(lock);

		cout << "Success to group invite...press any key to quit" << endl;
        cin >> cmd;
    }
    else if (g_option == "member")
    {
        cout << "Observing invitation" << endl;
        accountMgr->observeInvitation(&onInvite);
        g_callbackLock.wait(lock);
        cout << "Waiting invitation" << endl;
        g_callbackLock.wait(lock);
        cout << "Joining group " << g_invitedGroup << endl;
 //       accountMgr->joinGroup(g_invitedGroup, &onPost);
		accountMgr->replyToInvitation(g_invitedGroup, true, &onDelete);
        g_callbackLock.wait(lock);

        cout << "find my resource " << cmd << endl;
        result = OCPlatform::findResource(g_host, "/oic/res",
                static_cast< OCConnectivityType >(CT_ADAPTER_TCP | CT_IP_USE_V4), &foundMyDevice);
        g_callbackLock.wait(lock);

        accountMgr->observeGroup(&onObserveGroup);
        g_callbackLock.wait(lock);

		cout << "Success to group invite...press any key to quit" << endl;
        cin >> cmd;
    }
	
    return 0;
}
