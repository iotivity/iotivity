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
#define maxSequenceNumber 0xFFFFFF
#define TRUE true
#define FALSE false
#define PUBLIC_GROUP_TYPE_CHECK "Public"
#define PRIVATE_GROUP_TYPE_CHECK "Private"

#define USER_UID "0b81892a-7244-42b8-a485-be12e5a61d46"
#define NAME "Mohammad Sahinur Hossen"
#define EMAIL "iotsrbd@gmail.com"
#define PHONE "+8801710321734"
#define EMPTY_STRING ""
#define INVALID "invalid#mail.b"

using namespace OC;
using namespace OCPlatform;
using namespace std;


class CIOCAccountManagerTest_stc: public ::testing::Test {

public:
	ResourceHandles m_resourceHandles;
	OCPresenceHandle m_ocPresenceHandle = nullptr;
	QueryParamsMap m_queryParams;
	OCStackResult m_actualResult;
	OCAccountManager::Ptr m_accountManager;
	ICHelper* m_ICHelper;

	bool flag = true;
	bool m_isCallbackInvoked;
	bool m_isGetSuccessful;
	bool m_isPutSuccessful;
	bool m_isPostSuccessful;
	bool m_isObserveSuccessful;
	bool m_isDeleteSuccessful;

	const std::vector< std::string > m_vecDeviceIdentifier;

	void onPublish(const OCRepresentation& rep, const int& eCode)
	    {
	        m_isCallbackInvoked = true;
	        IOTIVITYTEST_LOG(INFO, "[CIOCAccountManagerTest_stc] Response from onPublish\n");
	    }

	void handleLoginoutCB(const HeaderOptions &,
	                      const OCRepresentation &rep, const int ecode)
	{
		IOTIVITYTEST_LOG(INFO, "Auth callback is invoked!");

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

	void cloudConnectGetHandler(const HeaderOptions &head,const OCRepresentation &rep, const int ecode)
	{
		IOTIVITYTEST_LOG(INFO, "GET callback is invoked.");

		   if (ecode == OC_STACK_OK || ecode == OC_STACK_RESOURCE_CHANGED)
		       {
			   IOTIVITYTEST_LOG(INFO, "GET callback is Successful!");

			   	   m_isGetSuccessful = true;

			   	   ICHelper::icPrintRepresentation(rep);

		           if(ecode == 4)
		           {
					   ICHelper::GROUP_TYPE = rep.getValueToString("gtype");
					   ICHelper::GROUP_ID = rep.getValueToString("gid");
		           }

		       }
		       else
		       {
		    	   IOTIVITYTEST_LOG(INFO, "GET callback is Fail.");
		       }

	}

	void cloudConnectPostHandler(const HeaderOptions &head,const OCRepresentation &rep, const int ecode)
	{
		IOTIVITYTEST_LOG(INFO, "Post callback is invoked.");

		   if (ecode == OC_STACK_OK || ecode == OC_STACK_RESOURCE_CHANGED)
		       {
			   IOTIVITYTEST_LOG(INFO, "Post callback is Succesful.");

			       m_isPostSuccessful = true;

		           ICHelper::icPrintRepresentation(rep);
		       }
		       else
		       {
		    	   IOTIVITYTEST_LOG(INFO, "Post callback is Fail.");
		       }

	}

	void cloudConnectPutHandler(const HeaderOptions &head,const OCRepresentation &rep, const int ecode)
	{
		IOTIVITYTEST_LOG(INFO, "Put callback is invoked.");

		   if (ecode == OC_STACK_OK || ecode == OC_STACK_RESOURCE_CHANGED)
		       {
			   IOTIVITYTEST_LOG(INFO, "Put callback is Successful.");

			   	   m_isPutSuccessful = true;

		           ICHelper::icPrintRepresentation(rep);
		       }
		       else
		       {
		    	   IOTIVITYTEST_LOG(INFO, "Put callback is Fail.");
		       }

	}

	void cloudConnectDeleteHandler(const HeaderOptions &head, const int eCode)
	{
		IOTIVITYTEST_LOG(INFO, "Delete callback is invoked.");

		   if (eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_DELETED)
		       {
				   m_isDeleteSuccessful = true;
				   IOTIVITYTEST_LOG(INFO, "Delete callback is Successful.");
		       }
		       else
		       {
		    	   IOTIVITYTEST_LOG(INFO, "Delete callback is Fail.");
		       }

	}

	void cloudConnectObserveHandler(const HeaderOptions /*headerOptions*/, const OCRepresentation &rep,
	               const int &eCode, const int &sequenceNumber)
	{
	    try
	    {
	        if (eCode == OC_STACK_OK && sequenceNumber != maxSequenceNumber + 1)
	        {
	            if (sequenceNumber == OC_OBSERVE_REGISTER)
	            {
	            	m_isObserveSuccessful = true;
	            	IOTIVITYTEST_LOG(INFO,"Observe registration action is successful");
	            }

	            IOTIVITYTEST_LOG(INFO,"OBSERVE RESULT:");
	            ICHelper::icPrintRepresentation(rep);
	        }
	        else
	        {
	            if (eCode == OC_STACK_OK)
	            {
	            		m_isObserveSuccessful = true;

	            		IOTIVITYTEST_LOG(INFO, "Observe callback is Successful.");
	            }
	            else
	            {
	            	IOTIVITYTEST_LOG(INFO, "Observe callback is Fail.");
	                exit(-1);
	            }
	        }
	    }
	    catch (exception &e)
	    {
	        cout << "Exception: " << e.what() << " in onObserve" << endl;
	    }
	}


protected:
	std::string m_errorMsg;


	virtual void SetUp() {
		m_ICHelper = ICHelper::getInstance();
		m_actualResult = OC_STACK_ERROR;
		m_queryParams = { };
//		ICHelper::DEVICE_ID = {};

		m_isCallbackInvoked = false;
		m_isGetSuccessful = false;
		m_isDeleteSuccessful = false;
		m_isPutSuccessful = false;
		m_isPostSuccessful = false;
		m_isObserveSuccessful = false;

		if(!ICHelper::isSignedUp)
		{
			ICHelper::isSignedUp = true;

			OCPersistentStorage ps{ ICHelper::readResourceDATFile, fread, fwrite, fclose, unlink };

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

			IOTIVITYTEST_LOG(INFO, "Platform configured successfully...");

			string host = "coap+tcp://";
			host += IC_ACCOUNT_MANAGER_HOST_ADDRESS;

			m_accountManager = OCPlatform::constructAccountManagerObject(host,CT_ADAPTER_TCP);

			IOTIVITYTEST_LOG(INFO,"OCPlatform::constructAccountManagerObject successull..." );


			char* authCode = "";

			authCode = m_ICHelper->getGitLoginAuthCodeMain();

			m_actualResult =  m_accountManager->signUp(AUTH_PROVIDER, authCode,
								std::bind(&CIOCAccountManagerTest_stc::handleLoginoutCB,this, placeholders::_1, placeholders::_2,placeholders::_3));

			ICHelper::waitForServerResponse();

			EXPECT_EQ(OC_STACK_OK,m_actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);


			cout << "ACCESS_TOKEN: "<< ICHelper::ACCESS_TOKEN << endl;
			cout << "UID : "<< ICHelper::UID << endl;
		}

	}

	virtual void TearDown() {
		ICHelper::waitForServerResponse();
	}

};


/**
 * @since 2016-09-07
 * @see none
 * @objective Test 'SignUp', 'SignIn' and 'SignOut' API With valid scenario
 * @target string SignUp() API
 * @target string SignIn() API
 * @target string SignOut() API
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform string host() API
 * @post_condition None
 * @expected 'SignIn' and 'SignOut' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CIOCAccountManagerTest_stc, SignUpSignInSignOut_SRC_P)
{
	string l_accessToken = ICHelper::ACCESS_TOKEN;
	try
	    {
		cout<<"ICHelper::ACCESS_TOKEN :"<<ICHelper::ACCESS_TOKEN<<endl;

		EXPECT_EQ(OC_STACK_OK, m_accountManager->signIn(ICHelper::UID, ICHelper::ACCESS_TOKEN,
				std::bind(&CIOCAccountManagerTest_stc::handleLoginoutCB,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"SignIn API does not sign-in to account server!";

		ICHelper::waitForServerResponse();

		EXPECT_EQ(OC_STACK_OK, m_accountManager->signOut(ICHelper::ACCESS_TOKEN, std::bind(&CIOCAccountManagerTest_stc::handleLoginoutCB,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"SignOut API does not sign-in to account server!";

		ICHelper::waitForServerResponse();
	}
	catch(OCException ex)
	{
		FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
	}

	SUCCEED();

}
#endif

/**
 * @since 2016-09-07
 * @see none
 * @objective Test 'searchUser' API With valid scenario
 * @target string searchUser() API
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure Perform string host() API
 * @post_condition SignOut()
 * @expected 'SearchUser' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CIOCAccountManagerTest_stc, SearchUserWithQueryParamsMapEmail_SRC_P)
{
	string l_accessToken = ICHelper::ACCESS_TOKEN;
	try
	    {
		EXPECT_EQ(OC_STACK_OK, m_accountManager->signIn(ICHelper::UID, ICHelper::ACCESS_TOKEN,
				std::bind(&CIOCAccountManagerTest_stc::handleLoginoutCB,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"SignIn API does not sign-in to account server!";
		ICHelper::waitForServerResponse();

		m_queryParams["email"] = EMAIL;

		EXPECT_EQ(OC_STACK_OK, m_accountManager->searchUser(m_queryParams, std::bind(&CIOCAccountManagerTest_stc::cloudConnectGetHandler,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"searchUser API is not getting information of the user to account server!";
		ICHelper::waitForServerResponse();

		EXPECT_EQ(true, m_isGetSuccessful);

		EXPECT_EQ(OC_STACK_OK, m_accountManager->signOut( l_accessToken, std::bind(&CIOCAccountManagerTest_stc::handleLoginoutCB,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"SignOut API does not sign-in to account server!";
	}
	catch(OCException ex)
	{
		FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
	}

	SUCCEED();

}
#endif

/**
 * @since 2016-09-07
 * @see none
 * @objective Test 'searchUser' API With valid scenario
 * @target string searchUser() API
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure Perform string host() API
 * @post_condition SignOut()
 * @expected 'SearchUser' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CIOCAccountManagerTest_stc, SearchUserWithQueryParamsMapName_SRC_P)
{
	string l_accessToken = ICHelper::ACCESS_TOKEN;

	try
	    {
		EXPECT_EQ(OC_STACK_OK, m_accountManager->signIn(ICHelper::UID, ICHelper::ACCESS_TOKEN,
				std::bind(&CIOCAccountManagerTest_stc::handleLoginoutCB,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"SignIn API does not sign-in to account server!";
		ICHelper::waitForServerResponse();

		m_queryParams["name"] = NAME;

		EXPECT_EQ(OC_STACK_OK, m_accountManager->searchUser(m_queryParams, std::bind(&CIOCAccountManagerTest_stc::cloudConnectGetHandler,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"searchUser API is not getting information of the user to account server!";
		ICHelper::waitForServerResponse();

		EXPECT_EQ(true, m_isGetSuccessful);

		EXPECT_EQ(OC_STACK_OK, m_accountManager->signOut( l_accessToken, std::bind(&CIOCAccountManagerTest_stc::handleLoginoutCB,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"SignOut API does not sign-in to account server!";
	}
	catch(OCException ex)
	{
		FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
	}

	SUCCEED();

}
#endif

/**
 * @since 2016-09-07
 * @see none
 * @objective Test 'searchUser' API With valid scenario
 * @target string searchUser() API
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure Perform string host() API
 * @post_condition SignOut()
 * @expected 'SearchUser' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CIOCAccountManagerTest_stc, SearchUserWithQueryParamsMapPhone_SRC_P)
{
	string l_accessToken = ICHelper::ACCESS_TOKEN;

	try
	    {
		EXPECT_EQ(OC_STACK_OK, m_accountManager->signIn(ICHelper::UID, ICHelper::ACCESS_TOKEN,
				std::bind(&CIOCAccountManagerTest_stc::handleLoginoutCB,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"SignIn API does not sign-in to account server!";
		ICHelper::waitForServerResponse();

		m_queryParams["phone"] = PHONE;

		EXPECT_EQ(OC_STACK_OK, m_accountManager->searchUser(m_queryParams, std::bind(&CIOCAccountManagerTest_stc::cloudConnectGetHandler,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"searchUser API is not getting information of the user to account server!";
		ICHelper::waitForServerResponse();

		EXPECT_EQ(true, m_isGetSuccessful);

		EXPECT_EQ(OC_STACK_OK, m_accountManager->signOut( l_accessToken, std::bind(&CIOCAccountManagerTest_stc::handleLoginoutCB,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"SignOut API does not sign-in to account server!";
	}
	catch(OCException ex)
	{
		FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
	}

	SUCCEED();

}
#endif

/**
 * @since 2016-09-07
 * @see none
 * @objective Test 'searchUser' API With valid scenario
 * @target string searchUser() API
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure Perform string host() API
 * @post_condition SignOut()
 * @expected 'SearchUser' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CIOCAccountManagerTest_stc, SearchUserWithQueryParamsMapEmail_ESV_N)
{
	string l_accessToken = ICHelper::ACCESS_TOKEN;
	try
	    {
		EXPECT_EQ(OC_STACK_OK, m_accountManager->signIn(ICHelper::UID, ICHelper::ACCESS_TOKEN,
				std::bind(&CIOCAccountManagerTest_stc::handleLoginoutCB,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"SignIn API does not sign-in to account server!";
		ICHelper::waitForServerResponse();

		m_queryParams["email"] = EMPTY_STRING;

		EXPECT_EQ(OC_STACK_OK,m_accountManager->searchUser(m_queryParams, std::bind(&CIOCAccountManagerTest_stc::cloudConnectGetHandler,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"searchUser API is not getting information of the user to account server!";
		ICHelper::waitForServerResponse();

		EXPECT_EQ(false, m_isGetSuccessful)<<"Item should not be found! ";

		EXPECT_EQ(OC_STACK_OK, m_accountManager->signOut( l_accessToken, std::bind(&CIOCAccountManagerTest_stc::handleLoginoutCB,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"SignOut API does not sign-in to account server!";
	}
	catch(OCException ex)
	{
		FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
	}

	SUCCEED();

}
#endif

/**
 * @since 2016-09-07
 * @see none
 * @objective Test 'searchUser' API With valid scenario
 * @target string searchUser() API
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure Perform string host() API
 * @post_condition SignOut()
 * @expected 'SearchUser' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CIOCAccountManagerTest_stc, SearchUserWithQueryParamsMapEmail_USV_N)
{
	string l_accessToken = ICHelper::ACCESS_TOKEN;
	try
	    {
		EXPECT_EQ(OC_STACK_OK, m_accountManager->signIn(ICHelper::UID, ICHelper::ACCESS_TOKEN,
				std::bind(&CIOCAccountManagerTest_stc::handleLoginoutCB,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"SignIn API does not sign-in to account server!";
		ICHelper::waitForServerResponse();

		m_queryParams["email"] = INVALID;

		EXPECT_EQ(OC_STACK_OK,m_accountManager->searchUser(m_queryParams, std::bind(&CIOCAccountManagerTest_stc::cloudConnectGetHandler,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"searchUser API is not getting information of the user to account server!";
		ICHelper::waitForServerResponse();

		EXPECT_EQ(false, m_isGetSuccessful)<<"Item should not be found! ";

		EXPECT_EQ(OC_STACK_OK, m_accountManager->signOut(l_accessToken, std::bind(&CIOCAccountManagerTest_stc::handleLoginoutCB,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"SignOut API does not sign-in to account server!";
	}
	catch(OCException ex)
	{
		FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
	}

	SUCCEED();

}
#endif

/**
 * @since 2016-09-07
 * @see none
 * @objective Test 'createGroup' API With valid scenario
 * @target string createGroup() API
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure Perform string host() API
 * @post_condition SignOut()
 * @expected 'createGroup' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CIOCAccountManagerTest_stc, CreateAndGetGroup_SRC_P)
{
	string l_accessToken = ICHelper::ACCESS_TOKEN;

	try
	    {

		EXPECT_EQ(OC_STACK_OK, m_accountManager->signIn(ICHelper::UID, ICHelper::ACCESS_TOKEN,
									std::bind(&CIOCAccountManagerTest_stc::handleLoginoutCB,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"SignIn API does not sign-in to account server!";

		ICHelper::waitForServerResponse();

		EXPECT_EQ(OC_STACK_OK, m_accountManager->createGroup(std::bind(&CIOCAccountManagerTest_stc::cloudConnectGetHandler,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"createGroup API is not creating a group on account server!";

		ICHelper::waitForServerResponse();

		EXPECT_EQ(true, m_isGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";

		EXPECT_EQ(OC_STACK_OK, m_accountManager->getGroupInfoAll(std::bind(&CIOCAccountManagerTest_stc::cloudConnectGetHandler,this, placeholders::_1, placeholders::_2,placeholders::_3)))<<"GetGroupList API is getting a list of groups joined from account server!";

		ICHelper::waitForServerResponse();

		EXPECT_EQ(true, m_isGetSuccessful)<<"getGroupList API is not getting a list of groups joined from account server! ";

		EXPECT_EQ(OC_STACK_OK, m_accountManager->signOut( l_accessToken, std::bind(&CIOCAccountManagerTest_stc::handleLoginoutCB,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"SignOut API does not sign-in to account server!";

	}
	catch(OCException ex)
	{
		FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
	}

	SUCCEED();

}
#endif

/**
 * @since 2016-09-07
 * @see none
 * @objective Test 'createGroup' API With valid scenario
 * @target string createGroup() API
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure Perform string host() API
 * @post_condition SignOut()
 * @expected 'createGroup' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CIOCAccountManagerTest_stc, CreateAndGetGroupInfo_SRC_P)
{
	string l_accessToken = ICHelper::ACCESS_TOKEN;
	try
	    {
		EXPECT_EQ(OC_STACK_OK, m_accountManager->signIn(ICHelper::UID, ICHelper::ACCESS_TOKEN,
						std::bind(&CIOCAccountManagerTest_stc::handleLoginoutCB,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"SignIn API does not sign-in to account server!";
		ICHelper::waitForServerResponse();

		EXPECT_EQ(OC_STACK_OK, m_accountManager->createGroup(std::bind(&CIOCAccountManagerTest_stc::cloudConnectGetHandler,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"createGroup API is not creating a group on account server!";
		ICHelper::waitForServerResponse();

		EXPECT_EQ(TRUE, m_isGetSuccessful)<<"createGroup API is not creating a group on account server! ";

		EXPECT_EQ(OC_STACK_OK, m_accountManager->getGroupInfo(ICHelper::GROUP_ID, std::bind(&CIOCAccountManagerTest_stc::cloudConnectGetHandler,this, placeholders::_1, placeholders::_2,placeholders::_3)))<<"getGroupInfo API is not to get information of the group from account server!";
		ICHelper::waitForServerResponse();

		EXPECT_NE(EMPTY_STRING, ICHelper::GROUP_ID);

		EXPECT_EQ(OC_STACK_OK, m_accountManager->getGroupInfoAll(std::bind(&CIOCAccountManagerTest_stc::cloudConnectGetHandler,this, placeholders::_1, placeholders::_2,placeholders::_3)))<<"GetGroupList API is getting a list of groups joined from account server!";
		ICHelper::waitForServerResponse();

		EXPECT_EQ(OC_STACK_OK, m_accountManager->signOut( l_accessToken, std::bind(&CIOCAccountManagerTest_stc::handleLoginoutCB,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"SignOut API does not sign-in to account server!";

	}
	catch(OCException ex)
	{
		FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
	}

	SUCCEED();

}
#endif

/**
 * @since 2016-09-07
 * @see none
 * @objective Test 'createGroup' API With valid scenario
 * @target string createGroup() API
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure Perform string host() API
 * @post_condition SignOut()
 * @expected 'createGroup' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CIOCAccountManagerTest_stc, ObserveAndGetGroupList_SRC_P)
{
	string l_accessToken = ICHelper::ACCESS_TOKEN;
	try
	    {
		EXPECT_EQ(OC_STACK_OK, m_accountManager->signIn(ICHelper::UID, ICHelper::ACCESS_TOKEN,
						std::bind(&CIOCAccountManagerTest_stc::handleLoginoutCB,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"SignIn API does not sign-in to account server!";
		ICHelper::waitForServerResponse();

		EXPECT_EQ(OC_STACK_OK, m_accountManager->createGroup(std::bind(&CIOCAccountManagerTest_stc::cloudConnectGetHandler,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"createGroup API is not creating a group on account server!";
		ICHelper::waitForServerResponse();

		EXPECT_EQ(TRUE, m_isGetSuccessful)<<"createGroup API is not creating a group on account server! ";

		EXPECT_EQ(OC_STACK_OK, m_accountManager->observeGroup(std::bind(&CIOCAccountManagerTest_stc::cloudConnectObserveHandler,this, placeholders::_1, placeholders::_2,placeholders::_3,placeholders::_4)))<<"ObserveGroup API is not register observe to the group on account server!";
		ICHelper::waitForServerResponse();

		EXPECT_EQ(TRUE, m_isObserveSuccessful)<<"ObserveGroup API is not register observe to the group on account server! ";

		EXPECT_EQ(OC_STACK_OK, m_accountManager->getGroupInfoAll(std::bind(&CIOCAccountManagerTest_stc::cloudConnectGetHandler,this, placeholders::_1, placeholders::_2,placeholders::_3)))<<"GetGroupList API is getting a list of groups joined from account server!";
		ICHelper::waitForServerResponse();

		EXPECT_EQ(TRUE, m_isGetSuccessful)<<"createGroup API is not creating a group on account server! ";

		EXPECT_NE(EMPTY_STRING, ICHelper::GROUP_ID);

		EXPECT_EQ(OC_STACK_OK, m_accountManager->signOut( l_accessToken, std::bind(&CIOCAccountManagerTest_stc::handleLoginoutCB,this, placeholders::_1, placeholders::_2,placeholders::_3))) <<"SignOut API does not sign-in to account server!";

	}
	catch(OCException ex)
	{
		FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
	}

	SUCCEED();

}
#endif
