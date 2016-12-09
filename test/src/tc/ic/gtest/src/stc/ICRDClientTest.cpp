/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/
#include "ICHelper.h"

using namespace OC;
using namespace std;

class ICRDClientTest_stc: public ::testing::Test
{
public:
    ResourceHandles m_resourceHandles;
    RDClient m_rdClient;
    OCPresenceHandle m_ocPresenceHandle = nullptr;
    QueryParamsList m_queryParams =
    { };
    const std::vector< std::string > m_vecDeviceIdentifier;
    bool m_isCallbackInvoked;
    OCStackResult m_actualResult;

    OCAccountManager::Ptr m_accountManager;
    bool flag = true;
    ICHelper* m_ICHelper;

protected:
    ICRDClientTest_stc()
    {
        m_isCallbackInvoked = false;
        m_ICHelper = nullptr;
        m_actualResult = OC_STACK_ERROR;
    }

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        m_ICHelper = ICHelper::getInstance();
        m_actualResult = OC_STACK_ERROR;
        m_isCallbackInvoked = false;
        m_rdClient = RDClient::Instance();

        if (!ICHelper::isSignedUp)
        {
            ICHelper::isSignedUp = true;

            OCPersistentStorage ps
            { ICHelper::readResourceDATFile, fread, fwrite, fclose, unlink };

            PlatformConfig cfg
            { ServiceType::InProc, ModeType::Both, "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
                    0, // Uses randomly available port
                    IC_OC_QUALITY_OF_SERVICE, &ps};

            OCPlatform::Configure(cfg);

            cout << "Platform configured successfully..." << endl;

            string host = "coap+tcp://";
            host += IC_ACCOUNT_MANAGER_HOST_ADDRESS;

            m_accountManager = OCPlatform::constructAccountManagerObject(IC_HOST_ADDRESS,
                    CT_ADAPTER_TCP);
            cout << "OCPlatform::constructAccountManagerObject successull..." << endl;

			char* authCode = "";

			authCode = m_ICHelper->getGitLoginAuthCodeMain();

            m_actualResult = m_accountManager->signUp(AUTH_PROVIDER, authCode, onCloudConnect);

            ICHelper::waitForServerResponse();

            EXPECT_EQ(OC_STACK_OK, m_actualResult) << "Actual result string : "
                    << CommonUtil::s_OCStackResultString.at(m_actualResult);

            cout << "ACCESS_TOKEN: " << ICHelper::ACCESS_TOKEN << endl;
            cout << "UID : " << ICHelper::UID << endl;
        }
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
        flag = false;
    }

public:
    static void onPublish(const OCRepresentation& rep, const int& eCode)
    {
        ICHelper::isServerResponsed = true;
        cout << "onPublish callback is invoked with received code : " << eCode << endl;

    }

    static void onCloudConnect(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
    {
        cout << "onCloudConnect callback is invoked with received code : " << ecode << endl;

        ICHelper::isServerResponsed = true;

        if (rep.getPayload() != NULL)
        {
            ICHelper::icPrintRepresentation(rep);
        }

        if (ecode == 4)
        {
            ICHelper::ACCESS_TOKEN = rep.getValueToString("accesstoken");

            ICHelper::UID = rep.getValueToString("uid");
        }
    }

    static void onFoundDevice(shared_ptr< OC::OCResource > resource)
    {
        cout << "onFoundDevice executing..." << endl;
        ICHelper::isServerResponsed = true;

        string host = "coap+tcp://";
        host += IC_CLOUD_INTERFACE_HOST_ADDRESS;

        vector < string > rt = resource->getResourceTypes();

        cout << "Device found: " << resource->uri() << endl;
        cout << "DI: " << resource->sid() << endl;

        for (auto it = rt.begin(); it != rt.end(); it++)
        {
            cout << "RT: " << *it << endl;

            if (it->compare("oic.d.airconditioner") == 0)
            {
                string searchQuery = "/oic/res?di=";
                searchQuery += resource->sid();
                cout << "Airconditioner found : " << searchQuery << endl;

                OCPlatform::findResource(host, searchQuery,
                        static_cast< OCConnectivityType >(CT_ADAPTER_TCP | CT_IP_USE_V4),
                        &onFoundAirconditionerResource);

                OCPlatform::OCPresenceHandle handle;
                if (OCPlatform::subscribeDevicePresence(handle, host,
                { resource->sid() },
                        static_cast< OCConnectivityType >(CT_ADAPTER_TCP | CT_IP_USE_V4),
                        &onObserve) != OC_STACK_OK)
                {
                    cout << "Device presence failed" << endl;
                }
            }
        }
    }

    static void onFoundAirconditionerResource(shared_ptr< OC::OCResource > resource)
    {

        cout << "onFoundAirconditionerResource executing..." << endl;

        vector < string > rt = resource->getResourceTypes();

        cout << "Aircondition resource found: " << resource->uri() << endl;

        for (auto it = rt.begin(); it != rt.end(); it++)
        {
            cout << "RT: " << *it << endl;

            if (it->compare("x.com.samsung.da.device") == 0)
            {
                cout << "Found Samsung Airconditioner" << endl;

                QueryParamsMap query;
                query["if"] = string(LINK_INTERFACE);
                //Request to collection resource
                resource->get(query, &onGetCollectionResource);
            }
        }
    }

    static void onObserve(const HeaderOptions /*headerOptions*/, const OCRepresentation &rep,
            const int &eCode, const int &sequenceNumber)
    {
        cout << "onObserve executing..." << endl;

        try
        {
            if (eCode == OC_STACK_OK && sequenceNumber != MAX_SEQUENCE_NUMBER + 1)
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
        }
        catch (exception &e)
        {
            cout << "Exception: " << e.what() << " in onObserve" << endl;
        }
    }

    static void onPost(const HeaderOptions & /*headerOptions*/, const OCRepresentation &rep,
            const int eCode)
    {
        cout << "onObserve executing..." << endl;

        ICHelper::isServerResponsed = true;
        cout << "POST response: " << eCode << endl;

        printRepresentation(rep);
    }

    static void printRepresentation(OCRepresentation rep)
    {
        cout << "printRepresentation executing..." << endl;

        for (auto itr = rep.begin(); itr != rep.end(); ++itr)
        {
            cout << "\t" << itr->attrname() << ":\t" << itr->getValueToString() << endl;
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

    static void printResource(const OCRepresentation &rep)
    {
        cout << "printResource executing..." << endl;

        cout << "URI: " << rep.getUri() << endl;

        vector < string > rt = rep.getResourceTypes();
        for (auto it = rt.begin(); it != rt.end(); it++)
        {
            cout << "RT: " << (*it) << endl;
        }

        for (auto it = rep.begin(); it != rep.end(); it++)
        {
            cout << it->attrname() << " : " << it->getValueToString() << endl;
        }

        vector< OCRepresentation > children = rep.getChildren();

        for (auto it = children.begin(); it != children.end(); it++)
        {
            printResource(*it);
        }
    }

    static void icFoundResourceCB(std::shared_ptr< OC::OCResource > resource)
    {
        string host = "coap+tcp://";
        host += IC_CLOUD_INTERFACE_HOST_ADDRESS;

        cout << "icFoundResourceCB is invoked..." << endl;

        ICHelper::isServerResponsed = true;

        std::string hostAddress;
        std::string resourceURI;

        try
        {
            if (resource)
            {
                for (auto &resourceTypes : resource->getResourceTypes())
                {
                    if (resourceTypes.compare(IC_RESOURCE_TYPE_AIRCON) == 0)
                    {
                        std::cout << "\t Founded resource type : " << resourceTypes << std::endl;

                        resourceURI = resource->uri();
                        std::cout << "\t URI of the founded resource : " << resourceURI
                                << std::endl;

                        hostAddress = resource->host();
                        std::cout << "\t Host address of the resource: " << hostAddress
                                << std::endl;

                        OCPlatform::OCPresenceHandle handle;
                        if (OCPlatform::subscribeDevicePresence(handle, IC_HOST_ADDRESS,
                        { resource->sid() },
                                static_cast< OCConnectivityType >(CT_ADAPTER_TCP | CT_IP_USE_V4),
                                &onObserve) != OC_STACK_OK)
                        {
                            cout << "Device presence failed" << endl;
                        }

                        QueryParamsMap query;
                        query["if"] = string(LINK_INTERFACE);

                        //Request to collection resource
                        resource->get(query, &onGetCollectionResource);
                    }
                }
            }
            else
            {
                std::cout << "Founded resource is invalid.." << std::endl;
            }

        }
        catch (std::exception &e)
        {
            cout << "Exception in rcFoundResourceCB: " << e.what() << std::endl;
        }
    }

    static void onGetCollectionResource(const HeaderOptions &, const OCRepresentation &rep,
            const int ecode)
    {
        cout << "onGetCollectionResource executing..." << endl;
        ICHelper::isServerResponsed = true;

        string host = "coap+tcp://";
        host += IC_CLOUD_INTERFACE_HOST_ADDRESS;

        cout << "Resource get: " << ecode << endl;

        //printResource(rep);

        vector< OCRepresentation > children = rep.getChildren();

        cout << "Constructing binary switch..." << endl;

        for (auto it = children.begin(); it != children.end(); it++)
        {
            cout << "RT: " << it->getResourceTypes().at(0) << endl;

            if (it->getResourceTypes().at(0).compare(IC_RESOURCE_TYPE_BSWITCH) == 0)
            {
                cout << "Observing " << it->getUri() << endl;
                ICHelper::m_binaryswitchResource = OCPlatform::constructResourceObject(IC_HOST_ADDRESS,
                        it->getUri(),
                        static_cast< OCConnectivityType >(CT_ADAPTER_TCP | CT_IP_USE_V4), true,
                        { string(IC_RESOURCE_TYPE_BSWITCH) },
                        { string(DEFAULT_INTERFACE) });

                QueryParamsMap query;
                ICHelper::m_binaryswitchResource->observe(ObserveType::Observe, query, &onObserve);
            }
        }
    }

    static void onGetRequest(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
    {
        cout << "onGetRequest executing..." << endl;
        ICHelper::isServerResponsed = true;

        string host = "coap+tcp://";
        host += IC_CLOUD_INTERFACE_HOST_ADDRESS;

        cout << "Resource get: " << ecode << endl;

        checkPostRequestValue(rep);

    }

    static void checkPostRequestValue(const OCRepresentation &rep)
    {
        cout << "checkPostRequestValue is executing.." << endl;

        cout << "URI: " << rep.getUri() << endl;

        for (auto it = rep.begin(); it != rep.end(); it++)
        {
            if ((it->attrname().compare("value") == 0)
                    && (it->getValueToString().compare("true") == 0))
            {
                ICHelper::isPostRequestSuccess = true;
                cout << "Successfully done." << endl;
            }
        }
    }
};

/*
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'publishResourceToRD' positively to Publish Virtual Resource to RD
 * @target          publishResourceToRD(const std::string& host,OCConnectivityType connectivityType,
 *                             PublishResourceCallback callback)
 * @test_data       1. host                 Host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onPublish as callback
 * @pre_condition   none
 * @procedure       1. Call publishResourceToRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will Publish Virtual Resource to Resource Directory & return OC_STACK_OK
 */

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_stc, PostResquestToICRD_SRC_FSV_P)
{
    try
    {
        string host = "coap+tcp://";
        host += IC_CLOUD_INTERFACE_HOST_ADDRESS;

        m_actualResult = OC_STACK_ERROR;
        m_actualResult = m_accountManager->signIn(ICHelper::UID, ICHelper::ACCESS_TOKEN,&onCloudConnect);
        ASSERT_EQ(OC_STACK_OK, m_actualResult) << "SignIn API does not sign-in to account server!";

		cout << "\nRegister resources..." << endl;
		ICHelper::isServerResponsed = false;
		bool isResourceResistered = false;
		isResourceResistered = m_ICHelper->isResourceRegistered();
		ICHelper::waitForServerResponse();
		ASSERT_TRUE(isResourceResistered) << "Resource is not registered." << endl;

		m_actualResult = OC_STACK_ERROR;
		ICHelper::isServerResponsed = false;
	    m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS, OCConnectivityType::CT_ADAPTER_TCP,ICHelper::g_ResourceHandles,
							&onPublish);
		ICHelper::waitForServerResponse();
	    ASSERT_EQ(OC_STACK_OK, m_actualResult) << "Resource Published successfully..!";


        cout << "\nFinding airconditioner: " << endl;
        ICHelper::isServerResponsed = false;
        m_actualResult = OC_STACK_ERROR;
        m_actualResult = OCPlatform::findResource(IC_HOST_ADDRESS, OC_RSRVD_WELL_KNOWN_URI,
                static_cast<OCConnectivityType>(CT_ADAPTER_TCP | CT_IP_USE_V4 | CT_DEFAULT), &icFoundResourceCB);
        ICHelper::waitForServerResponse();
        ASSERT_TRUE(ICHelper::isServerResponsed) << "icFoundResourceCB callback is not invoked." << endl;

        cout << "\nSending post request....." << endl;
        OCRepresentation binarySwitch;
        binarySwitch.setValue("value", true);
        ICHelper::isServerResponsed = false;
        QueryParamsMap query;
        ICHelper::m_binaryswitchResource->post(IC_RESOURCE_TYPE_BSWITCH, DEFAULT_INTERFACE, binarySwitch, query,
                &onPost);
        ICHelper::waitForServerResponse();
        ASSERT_TRUE(ICHelper::isServerResponsed) << "onPost callback is not invoked." << endl;

        cout << "\nSending get request....." << endl;
        ICHelper::isServerResponsed = false;
        ICHelper::isPostRequestSuccess = false;
        QueryParamsMap getQuery;
        ICHelper::m_binaryswitchResource->get(getQuery,&onGetRequest);
        ICHelper::waitForServerResponse();

        EXPECT_TRUE(ICHelper::isServerResponsed) << "onGetRequest callback is not invoked." << endl;
        EXPECT_TRUE(ICHelper::isPostRequestSuccess) << "Post request is not successful." << endl;
    }
    catch(OCException ex)
    {
        FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }

    SUCCEED();
}
#endif
