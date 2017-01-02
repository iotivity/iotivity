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

using namespace OC;
using namespace std;

string              g_host;

class Resource
{
    public:
        OCResourceHandle m_handle;
        Resource(string uri, vector<string> rt, vector<string> itf)
        {
            m_representation.setUri(uri);
            m_representation.setResourceTypes(rt);
            m_representation.setResourceInterfaces(itf);
        }

        string getResourceUri()
        {
            return m_representation.getUri();
        }

        vector<string> getResourceType()
        {
            return m_representation.getResourceTypes();
        }

        vector<string> getInterfaces()
        {
            return m_representation.getResourceInterfaces();
        }

        OCRepresentation getRepresentation(void)
        {
            m_representation.clearChildren();
            for (auto it = m_childResources.begin(); it != m_childResources.end(); it++)
            {
                m_representation.addChild((*it)->getRepresentation());
            }
            return m_representation;
        }

        OCStackResult addChildResource(Resource  *childResource)
        {
            m_childResources.push_back(childResource);
            return OCPlatform::bindResource(m_handle, childResource->m_handle);
        }

        OCStackResult sendRepresentation(shared_ptr<OCResourceRequest> pRequest)
        {
            auto pResponse = make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(pRequest->getRequestHandle());
            pResponse->setResourceHandle(pRequest->getResourceHandle());

            // Check for query params (if any)
            QueryParamsMap queryParamsMap = pRequest->getQueryParameters();

            cout << "\t\t\tquery params: \n";
            for (auto it = queryParamsMap.begin(); it != queryParamsMap.end(); it++)
            {
                cout << "\t\t\t\t" << it->first << ":" << it->second << endl;
            }

            auto findRes = queryParamsMap.find("if");

            if (findRes != queryParamsMap.end())
            {
                pResponse->setResourceRepresentation(getRepresentation(), findRes->second);
            }
            else
            {
                pResponse->setResourceRepresentation(getRepresentation(), DEFAULT_INTERFACE);
            }

            pResponse->setErrorCode(200);
            pResponse->setResponseResult(OC_EH_OK);

            return OCPlatform::sendResponse(pResponse);
        }

        OCStackResult propagate()
        {
            if (m_interestedObservers.size() > 0)
            {
                shared_ptr<OCResourceResponse> resourceResponse =
                { make_shared<OCResourceResponse>() };

                resourceResponse->setErrorCode(200);
                resourceResponse->setResourceRepresentation(getRepresentation(), DEFAULT_INTERFACE);

                return OCPlatform::notifyListOfObservers(m_handle,
                        m_interestedObservers,
                        resourceResponse);
            }

            return OC_STACK_OK;
        }

        virtual OCEntityHandlerResult entityHandler(shared_ptr<OCResourceRequest> request) = 0;

    protected:
        OCRepresentation    m_representation;
        vector<Resource *>  m_childResources;
        ObservationIds      m_interestedObservers;
};

class BinarySwitchResource : public Resource //oic.r.switch.binary
{
    private:
        bool m_value;

    public:
        BinarySwitchResource(string uri, vector<string> rt, vector<string> itf)
            : Resource(uri, rt, itf)
        {
            m_value = false;
            m_representation.setValue("value", m_value);
        }

        void setBinarySwitchRepresentation(OCRepresentation &rep)
        {
            bool value;
            if (rep.getValue("value", value))
            {
                m_value = value;
                m_representation.setValue("value", m_value);
                cout << "\t\t\t\t" << "value: " << m_value << endl;

                propagate();
            }
        }

        OCEntityHandlerResult entityHandler(shared_ptr<OCResourceRequest> request)
        {
            cout << "\tIn Server Binaryswitch entity handler:\n";
            OCEntityHandlerResult ehResult = OC_EH_ERROR;

            if (request)
            {
                // Get the request type and request flag
                string requestType = request->getRequestType();
                int requestFlag = request->getRequestHandlerFlag();

                if (requestFlag & RequestHandlerFlag::RequestFlag)
                {
                    cout << "\t\trequestFlag : Request\n";

                    // If the request type is GET
                    if (requestType == "GET")
                    {
                        cout << "\t\t\trequestType : GET\n";
                        if (OC_STACK_OK == sendRepresentation(request))
                        {
                            ehResult = OC_EH_OK;
                        }
                    }
                    else if (requestType == "PUT")
                    {
                        cout << "\t\t\trequestType : PUT\n";
                        // PUT request operations
                    }
                    else if (requestType == "POST")
                    {
                        cout << "\t\t\trequestType : POST\n";
                        // POST request operations
                        OCRepresentation    rep = request->getResourceRepresentation();
                        setBinarySwitchRepresentation(rep);

                        if (OC_STACK_OK == sendRepresentation(request))
                        {
                            ehResult = OC_EH_OK;
                        }
                    }
                    else if (requestType == "DELETE")
                    {
                        cout << "\t\t\trequestType : DELETE\n";
                        // DELETE request operations
                    }
                }

                if (requestFlag & RequestHandlerFlag::ObserverFlag)
                {
                    cout << "\t\trequestFlag : Observer\n";

                    ObservationInfo observationInfo = request->getObservationInfo();
                    if (ObserveAction::ObserveRegister == observationInfo.action)
                    {
                        m_interestedObservers.push_back(observationInfo.obsId);
                    }
                    else if (ObserveAction::ObserveUnregister == observationInfo.action)
                    {
                        m_interestedObservers.erase(remove(
                                                        m_interestedObservers.begin(),
                                                        m_interestedObservers.end(),
                                                        observationInfo.obsId),
                                                    m_interestedObservers.end());
                    }
                }
            }
            else
            {
                cout << "Request invalid" << endl;
            }

            return ehResult;
        }
};

class TemperatureResource : public Resource //oic.r.temperature
{
    private:
        int m_temperature;
        string m_range;
        string m_units;

    public:
        TemperatureResource(string uri, vector<string> rt, vector<string> itf)
            : Resource(uri, rt, itf)
        {
            m_temperature = 0;
            m_range = "";
            m_units = "";
            m_representation.setValue("temperature", m_temperature);
            m_representation.setValue("range", m_range);
            m_representation.setValue("units", m_units);
        }

        void setTemperatureRepresentation(OCRepresentation &rep)
        {
            int temperature;
            string range;
            int units;

            if (rep.getValue("temperature", temperature) &&
                rep.getValue("range", range) &&
                rep.getValue("units", units))
            {
                m_temperature = temperature;
                m_range = range;
                m_units = units;
                m_representation.setValue("temperature", m_temperature);
                m_representation.setValue("range", m_range);
                m_representation.setValue("units", m_units);
                cout << "\t\t\t\t" << "temperature: " << m_temperature << endl;
                cout << "\t\t\t\t" << "range: " << m_range << endl;
                cout << "\t\t\t\t" << "units: " << m_units << endl;

                propagate();
            }
        }

        OCEntityHandlerResult entityHandler(shared_ptr<OCResourceRequest> request)
        {
            cout << "\tIn Server Temperature entity handler:\n";
            OCEntityHandlerResult ehResult = OC_EH_ERROR;

            if (request)
            {
                // Get the request type and request flag
                string requestType = request->getRequestType();
                int requestFlag = request->getRequestHandlerFlag();

                if (requestFlag & RequestHandlerFlag::RequestFlag)
                {
                    cout << "\t\trequestFlag : Request\n";

                    // If the request type is GET
                    if (requestType == "GET")
                    {
                        cout << "\t\t\trequestType : GET\n";
                        if (OC_STACK_OK == sendRepresentation(request))
                        {
                            ehResult = OC_EH_OK;
                        }
                    }
                    else if (requestType == "PUT")
                    {
                        cout << "\t\t\trequestType : PUT\n";
                        // PUT requeist operations
                    }
                    else if (requestType == "POST")
                    {
                        cout << "\t\t\trequestType : POST\n";
                        // POST request operations
                        OCRepresentation    rep = request->getResourceRepresentation();
                        setTemperatureRepresentation(rep);

                        if (OC_STACK_OK == sendRepresentation(request))
                        {
                            ehResult = OC_EH_OK;
                        }
                    }
                    else if (requestType == "DELETE")
                    {
                        cout << "\t\t\trequestType : DELETE\n";
                        // DELETE request operations
                    }
                }

                if (requestFlag & RequestHandlerFlag::ObserverFlag)
                {
                    cout << "\t\trequestFlag : Observer\n";

                    ObservationInfo observationInfo = request->getObservationInfo();
                    if (ObserveAction::ObserveRegister == observationInfo.action)
                    {
                        m_interestedObservers.push_back(observationInfo.obsId);
                    }
                    else if (ObserveAction::ObserveUnregister == observationInfo.action)
                    {
                        m_interestedObservers.erase(remove(
                                                        m_interestedObservers.begin(),
                                                        m_interestedObservers.end(),
                                                        observationInfo.obsId),
                                                    m_interestedObservers.end());
                    }
                }
            }
            else
            {
                cout << "Request invalid" << endl;
            }

            return ehResult;
        }
};

class AirConditionerResource : public Resource // oic.d.airconditioner
{
    private:

    public:
        AirConditionerResource(string uri, vector<string> rt, vector<string> itf)
            : Resource(uri, rt, itf)
        {

        }

        OCEntityHandlerResult entityHandler(shared_ptr<OCResourceRequest> request)
        {
            cout << "\tIn Server Airconditioner entity handler:\n";
            OCEntityHandlerResult ehResult = OC_EH_ERROR;

            if (request)
            {
                // Get the request type and request flag
                string requestType = request->getRequestType();
                int requestFlag = request->getRequestHandlerFlag();

                if (requestFlag & RequestHandlerFlag::RequestFlag)
                {
                    cout << "\t\trequestFlag : Request\n";

                    // If the request type is GET
                    if (requestType == "GET")
                    {
                        cout << "\t\t\trequestType : GET\n";
                        string findRes = request->getQueryParameters().find("if")->second;
                        if (findRes.compare(LINK_INTERFACE) == 0)
                        {
                            if (OC_STACK_OK == sendRepresentation(request))
                            {
                                ehResult = OC_EH_OK;
                            }
                        }
                        else
                        {
                            ehResult = OC_EH_FORBIDDEN;
                        }
                    }
                    else if (requestType == "PUT")
                    {
                        cout << "\t\t\trequestType : PUT\n";
                        // Call these functions to prepare the response for child resources and
                        // then send the final response using sendRoomResponse function

                        /*
                        for (auto it = m_childResources.begin();
                             it != m_childResources.end(); it++)
                        {
                            (*it)->entityHandler(request);
                        }

                        if (OC_STACK_OK == sendRepresentation(request))
                        {
                            ehResult = OC_EH_OK;
                        }
                        */
                    }
                    else if (requestType == "POST")
                    {
                        // POST request operations
                    }
                    else if (requestType == "DELETE")
                    {
                        // DELETE request operations
                    }
                }

                if (requestFlag & RequestHandlerFlag::ObserverFlag)
                {
                    cout << "\t\trequestFlag : Observer\n";
                }
            }
            else
            {
                cout << "Request invalid" << endl;
            }

            return ehResult;
        }
};

condition_variable g_callbackLock;
string             g_uid;
string             g_accesstoken;

void onPublish(const OCRepresentation &, const int &eCode)
{
    cout << "Publish resource response received, code: " << eCode << endl;

    g_callbackLock.notify_all();
}

void printRepresentation(OCRepresentation rep)
{
    for (auto itr = rep.begin(); itr != rep.end(); ++itr)
    {
        cout << "\t" << itr->attrname() << ":\t" << itr->getValueToString() << endl;
        if (itr->type() == AttributeType::Vector)
        {
            switch (itr->base_type())
            {
                case AttributeType::OCRepresentation:
                    for (auto itr2 : (*itr).getValue<vector<OCRepresentation> >())
                    {
                        printRepresentation(itr2);
                    }
                    break;

                case AttributeType::Integer:
                    for (auto itr2 : (*itr).getValue<vector<int> >())
                    {
                        cout << "\t\t" << itr2 << endl;
                    }
                    break;

                case AttributeType::String:
                    for (auto itr2 : (*itr).getValue<vector<string> >())
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
            printRepresentation((*itr).getValue<OCRepresentation>());
        }
    }
}

void handleLoginoutCB(const HeaderOptions &,
                      const OCRepresentation &rep, const int ecode)
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

int saveTrustCert(void)
{
    OCStackResult res = OC_STACK_ERROR;
    uint16_t g_credId = 0;

    cout << "Save Trust Cert. Chain into Cred of SVR" <<endl;

    ByteArray trustCertChainArray = {0, 0};

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

    res = OCSaveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len, OIC_ENCODING_PEM,&g_credId);

    if(OC_STACK_OK != res)
    {
        cout << "OCSaveTrustCertChainBin API error" << endl;
        return res;
    }
    cout << "CredId of Saved Trust Cert. Chain into Cred of SVR : " << g_credId << endl;

    return res;
}

static FILE *client_open(const char * /*path*/, const char *mode)
{
    return fopen("./rootca.dat", mode);
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        cout << "Put \"[host-ipaddress:port] [authprovider] [authcode] [tls mode(0,1)]\" for sign-up and sign-in and publish resources"
             << endl;
        cout << "Put \"[host-ipaddress:port] [uid] [accessToken] [tls mode(0,1)]\" for sign-in and publish resources" <<
             endl;
        return 0;
    }

    OCPersistentStorage ps{ client_open, fread, fwrite, fclose, unlink };

    PlatformConfig cfg
    {
        ServiceType::InProc,
        ModeType::Both,
        "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
        0,         // Uses randomly available port
        QualityOfService::LowQos,
        &ps
    };

    OCPlatform::Configure(cfg);

    OCStackResult result = OC_STACK_ERROR;

	g_host = "coap+tcp://";

	if (!strcmp(argv[4],"1"))
	{
#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
		g_host = "coaps+tcp://";
#endif
	} 
	
	g_host += argv[1];
	
	OCAccountManager::Ptr accountMgr = OCPlatform::constructAccountManagerObject(g_host,
                                       CT_ADAPTER_TCP);

	if (!strcmp(argv[4], "1"))
	{
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
	}

    mutex blocker;
    unique_lock<mutex> lock(blocker);

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

    cout << "Registering resources to platform..." << endl;

    AirConditionerResource  airConditioner("/sec/aircon/0", { "x.com.samsung.da.device" }, { DEFAULT_INTERFACE, BATCH_INTERFACE, LINK_INTERFACE });

    BinarySwitchResource    binarySwitch("/power/0", { "oic.r.switch.binary" }, { DEFAULT_INTERFACE });

    TemperatureResource     temperature("/temperature/0", { "oic.r.temperature" }, { DEFAULT_INTERFACE });

    string uri = airConditioner.getResourceUri();
    string rt = airConditioner.getResourceType()[0];
    string itf = airConditioner.getInterfaces()[0];

    result = OCPlatform::registerResource(airConditioner.m_handle,
                                          uri,
                                          rt,
                                          itf,
                                          bind(&AirConditionerResource::entityHandler
                                                  , &airConditioner, placeholders::_1),
                                          OC_DISCOVERABLE);

    if (result != OC_STACK_OK)
    {
        cout << "Resource registration was unsuccessful" << endl;
    }


    itf = airConditioner.getInterfaces()[1];
    result = OCPlatform::bindInterfaceToResource(airConditioner.m_handle, itf);

    if (result != OC_STACK_OK)
    {
        cout << "Binding second interface was unsuccessful" << endl;
    }


    itf = airConditioner.getInterfaces()[2];
    result = OCPlatform::bindInterfaceToResource(airConditioner.m_handle, itf);

    if (result != OC_STACK_OK)
    {
        cout << "Binding third interface was unsuccessful" << endl;
    }


    uri = binarySwitch.getResourceUri();
    rt = binarySwitch.getResourceType()[0];
    itf = binarySwitch.getInterfaces()[0];

    result = OCPlatform::registerResource(binarySwitch.m_handle,
                                          uri,
                                          rt,
                                          itf,
                                          bind(&BinarySwitchResource::entityHandler
                                                  , &binarySwitch, placeholders::_1),
                                          OC_OBSERVABLE);

    uri = temperature.getResourceUri();
    rt = temperature.getResourceType()[0];
    itf = temperature.getInterfaces()[0];

    result = OCPlatform::registerResource(temperature.m_handle,
                                          uri,
                                          rt,
                                          itf,
                                          bind(&TemperatureResource::entityHandler
                                                  , &temperature, placeholders::_1),
                                          OC_OBSERVABLE);

    result = airConditioner.addChildResource(&binarySwitch);

    result = airConditioner.addChildResource(&temperature);

    cout << "Publishing resources to cloud ";


    ResourceHandles resourceHandles;

    OCDeviceInfo        devInfoAirConditioner;
    OCStringLL          deviceType;

    deviceType.value = "oic.d.airconditioner";
    deviceType.next = NULL;
    devInfoAirConditioner.deviceName = "FAC_2016";
    devInfoAirConditioner.types = &deviceType;
    devInfoAirConditioner.specVersion = NULL;
    devInfoAirConditioner.dataModelVersions = NULL;

    OCPlatform::registerDeviceInfo(devInfoAirConditioner);

	result = RDClient::Instance().publishResourceToRD(g_host, OCConnectivityType::CT_ADAPTER_TCP,
             resourceHandles,
             &onPublish);

    cout << " result: " << result << " Waiting Publish default resource response from cloud" << endl;

    resourceHandles.push_back(airConditioner.m_handle);

	result = RDClient::Instance().publishResourceToRD(g_host, OCConnectivityType::CT_ADAPTER_TCP,
             resourceHandles,
             &onPublish);

    cout << " result: " << result << " Waiting Publish user resource response from cloud" << endl;

    g_callbackLock.wait(lock);


    cout << "PUT 1/0 to turn on/off air conditioner for observe testing, q to terminate" << endl;

    string cmd;

    while (true)
    {
        cin >> cmd;
        OCRepresentation    rep;

        switch (cmd[0])
        {
            case '1':
                rep.setValue(string("value"), true);
                binarySwitch.setBinarySwitchRepresentation(rep);
                break;

            case '0':
                rep.setValue(string("value"), false);
                binarySwitch.setBinarySwitchRepresentation(rep);
                break;

            case 'q':
                goto exit;
        }
    }

exit:
    return 0;
}
