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

#include "SMRemoteHelper.h"

class SMRemoteSceneListTest_btc: public ::testing::Test {
public:
	SMRemoteHelper* m_pSMRemoteHelper;
	RCSRemoteResourceObject::Ptr m_pListResource;
	std::shared_ptr<RemoteSceneList> m_pRemoteSceneList;
	std::shared_ptr<RemoteSceneCollection> m_pRemoteSceneCollection;
	bool isException;
	std::string remoteSceneListName;

	SMRemoteSceneListTest_btc() {
		m_pSMRemoteHelper = nullptr;
		m_pRemoteSceneList = nullptr;
		m_pRemoteSceneCollection = nullptr;
		isException = false;
		remoteSceneListName = "Home";
	}

	virtual void SetUp() {
		IOTIVITYTEST_LOG(INFO, "SetUp IN");

		CommonUtil::runCommonTCSetUpPart();

		CommonUtil::launchApp(SCENE_SERVER);
		CommonUtil::waitInSecond(MAX_SLEEP_TIME);

		m_pSMRemoteHelper = SMRemoteHelper::getInstance();
		m_pSMRemoteHelper->discoverResource();
		CommonUtil::waitInSecond(MAX_SLEEP_TIME);
		m_pSMRemoteHelper->stopDiscovery();
	}

	virtual void TearDown() {
		IOTIVITYTEST_LOG(INFO, "TearDown IN");

		CommonUtil::runCommonTCTearDownPart();

		CommonUtil::killApp(SCENE_SERVER);
		CommonUtil::waitInSecond(MAX_SLEEP_TIME);
	}

public:
	void onRemoteSceneListCreated(RemoteSceneList::Ptr remoteSceneList, int) {
		IOTIVITYTEST_LOG(INFO, "%s is called", __func__);

		if (remoteSceneList) {
			m_pRemoteSceneList = std::move(remoteSceneList);
		} else {
			IOTIVITYTEST_LOG(INFO, "Create Remote list failed.");
		}
	}

	void onRemoteSceneCollectionCreated(
			RemoteSceneCollection::Ptr remoteSceneCol, int) {
		IOTIVITYTEST_LOG(INFO, "%s is called", __func__);

		if (remoteSceneCol) {
			m_pRemoteSceneCollection = remoteSceneCol;
		} else {
			IOTIVITYTEST_LOG(INFO, "Create Remote scene collection failed.");
		}
	}

	void onSetName(int) {
		IOTIVITYTEST_LOG(INFO, "%s is called", __func__);
	}
};

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @objective Test createInstance function positively
 * @target static void createInstance(
 * 		RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @test_data 		1. sceneListResource RCSRemoteResourceObject pointer of SceneList
 * 					2. cb A callback to create instance
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * @procedure call createInstance
 * @post_condition stop the simulator
 * @expected createInstance will succeed and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneListTest_btc, RemoteSceneListCreateInstance_RSV_P)
{
	try
	{
		RemoteSceneList::createInstance(m_pSMRemoteHelper->g_pFoundSceneList,
				std::bind(&SMRemoteSceneListTest_btc::onRemoteSceneListCreated, this, placeholders::_1, placeholders::_2));}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside createInstance: " + std::string(e.what()));
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @objective Test createInstance function negatively with null resource
 * @target static void createInstance(
 * 		RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @test_data 		1. sceneListResource NULL
 *					2. cb A callback to create instance
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * @procedure 		1. call createInstance with null scenelist resource
 * 					2. check if RCSInvalidParameterException has occured
 * @post_condition stop the simulator
 * @expected createInstance will not succeed and RCSInvalidParameterException exception should occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneListTest_btc, RemoteSceneListCreateInstance_NV_ETC_N)
{
	try
	{
		RemoteSceneList::createInstance(NULL,std::bind(
						&SMRemoteSceneListTest_btc::onRemoteSceneListCreated, this, placeholders::_1, placeholders::_2));
	}
	catch (const RCSInvalidParameterException& e)
	{
		IOTIVITYTEST_LOG(INFO,"Expected exception occured");
		isException = true;
	}
	catch (exception& e)
	{
		SET_FAILURE("Unexpected Exception occurred inside createInstance: " + std::string(e.what()));
		return;
	}

	if(isException == false)
	{
		SET_FAILURE("RCSInvalidParameterException did not occur");
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 * 		RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @objective Test addNewSceneCollection function positively
 * @target void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @test_data cb A callback function to create remote scene collection
 * @pre_condition	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. Call createInstance to receive remote sceneList instance
 * @procedure call addNewSceneCollection
 * @post_condition stop the simulator
 * @expected addNewSceneCollection will succeed and no exceptions should occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneListTest_btc, RemoteAddNewSceneCollection_SRC_P)
{
	m_pRemoteSceneList = m_pSMRemoteHelper->createRemoteSceneListInstance();
	if(m_pRemoteSceneList == NULL_PTR)
	{
		SET_FAILURE("did not find remote scene list instance");
		return;
	}

	try
	{
		m_pRemoteSceneList->addNewSceneCollection(std::bind(
						&SMRemoteSceneListTest_btc::onRemoteSceneCollectionCreated, this, placeholders::_1, placeholders::_2));
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside addNewSceneCollection: " + std::string(e.what()));
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 * 		RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @objective Test addNewSceneCollection function negatively with null callback
 * @target void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @test_data cb = NULL
 * @pre_condition	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. Call createInstance to receive remote sceneList instance
 * @procedure 		1. call addNewSceneCollection with NULL parameter
 * 					2. check if RCSInvalidParameterException has occured
 * @post_condition stop the simulator
 * @expected addNewSceneCollection will not succeed and RCSInvalidParameterException should occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneListTest_btc, RemoteAddNewSceneCollection_NV_ETC_N)
{
	m_pRemoteSceneList = m_pSMRemoteHelper->createRemoteSceneListInstance();
	if(m_pRemoteSceneList == NULL_PTR)
	{
		SET_FAILURE("did not find remote scene list instance");
		return;
	}

	try
	{
		m_pRemoteSceneList->addNewSceneCollection(NULL);
	}
	catch (const RCSInvalidParameterException& e)
	{
		IOTIVITYTEST_LOG(INFO,"Expected exception occured");
		isException = true;
	}
	catch (exception& e)
	{
		SET_FAILURE("Unexpected Exception occurred inside addNewSceneCollection: " + std::string(e.what()));
		return;
	}

	if(isException == false)
	{
		SET_FAILURE("RCSInvalidParameterException did not occur");
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 * 	const std::vector< std::string >& resourceTypes,
 * 	ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 * 		RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @objective Test getSceneCollections function positively for getting list of SceneCollections
 * @target std::vector< RemoteSceneCollection::Ptr > getRemoteSceneCollections() const;
 * @test_data none
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. Call createInstance to receive remote sceneList instance
 * 					4. call addNewSceneCollection
 * @procedure 		1. call getSceneCollections
 * 					2. check if list of scene collections is returned
 * @post_condition stop the simulator
 * @expected getRemoteSceneCollections will return list of scene collections and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneListTest_btc, RemoteGetRemoteSceneCollections_SRC_P)
{
	m_pRemoteSceneList = m_pSMRemoteHelper->createRemoteSceneListInstance();
	if(m_pRemoteSceneList == NULL_PTR)
	{
		SET_FAILURE("did not find remote scene list instance");
		return;
	}

	m_pRemoteSceneList->addNewSceneCollection(std::bind(
					&SMRemoteSceneListTest_btc::onRemoteSceneCollectionCreated, this, placeholders::_1, placeholders::_2));
	CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
	ASSERT_NE(m_pRemoteSceneCollection,NULL_PTR) << "could not add remote scene collection";

	std::vector<RemoteSceneCollection::Ptr> remoteSceneCollections;

	try
	{
		remoteSceneCollections = m_pRemoteSceneList->getRemoteSceneCollections();
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside getRemoteSceneCollections: " + std::string(e.what()));
		return;
	}

	if(remoteSceneCollections.size() == INT_ZERO)
	{
		SET_FAILURE("list of remote scene collections is not found ");
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 * 	const std::vector< std::string >& resourceTypes,
 * 	ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 * 		RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @objective Test setName function positively
 * @target void setName(const std::string &name, SetNameCallback cb);
 * @test_data 		1. name = "Home"
 * 					2. cb A callback to receive the response
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. Call createInstance to receive remote sceneList instance
 * @procedure call setName
 * @post_condition stop the simulator
 * @expected setName will succeed and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneListTest_btc, RemoteSceneListSetName_SRC_P)
{
	m_pRemoteSceneList = m_pSMRemoteHelper->createRemoteSceneListInstance();
	if(m_pRemoteSceneList == NULL_PTR)
	{
		SET_FAILURE("did not find remote scene list instance");
		return;
	}

	try
	{
		m_pRemoteSceneList->setName(remoteSceneListName,std::bind(&SMRemoteSceneListTest_btc::onSetName, this, placeholders::_1));
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside setName: " + std::string(e.what()));
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 * 		RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @objective Test setName function positively with empty string
 * @target void setName(const std::string &name, SetNameCallback cb);
 * @test_data 		1. name = ""
 * 					2. cb A callback to receive the response
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. Call createInstance to receive remote sceneList instance
 * @procedure call setName with empty string as name
 * @post_condition stop the simulator
 * @expected setName will succeed and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneListTest_btc, RemoteSceneListSetName_ESV_P)
{
	m_pRemoteSceneList = m_pSMRemoteHelper->createRemoteSceneListInstance();
	if(m_pRemoteSceneList == NULL_PTR)
	{
		SET_FAILURE("did not find remote scene list instance");
		return;
	}

	try
	{
		m_pRemoteSceneList->setName(EMPTY_STRING,std::bind(&SMRemoteSceneListTest_btc::onSetName, this, placeholders::_1));
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside setName: " + std::string(e.what()));
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 * 		RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @objective Test setName function negatively using null callback
 * @target void setName(const std::string &name, SetNameCallback cb);
 * @test_data 		1. name = "Home"
 * 					2. cb NULL callback
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. Call createInstance to receive remote sceneList instance
 * @procedure 		1. call setName with null callback
 * 					2. check if RCSInvalidParameterException has occured
 * @post_condition stop the simulator
 * @expected setName will not succeed and RCSInvalidParameterException will occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneListTest_btc, RemoteSceneListSetName_NV_ETC_N)
{
	m_pRemoteSceneList = m_pSMRemoteHelper->createRemoteSceneListInstance();
	if(m_pRemoteSceneList == NULL_PTR)
	{
		SET_FAILURE("did not find remote scene list instance");
		return;
	}

	try
	{
		m_pRemoteSceneList->setName(remoteSceneListName,NULL);
	}
	catch (const RCSInvalidParameterException& e)
	{
		IOTIVITYTEST_LOG(INFO,"Expected exception occured");
		isException = true;
	}
	catch (exception& e)
	{
		SET_FAILURE("Unexpected Exception occurred inside setName: " + std::string(e.what()));
		return;
	}

	if(isException == false)
	{
		SET_FAILURE("RCSInvalidParameterException did not occur");
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 * 		RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void setName(const std::string &name, SetNameCallback cb);
 * @objective Test getName function positively
 * @target std::string getName() const;
 * @test_data none
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. Call createInstance to receive remote sceneList instance
 * @procedure call getName and check if returned name is not empty
 * @post_condition stop the simulator
 * @expected getName will return name and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneListTest_btc, RemoteSceneListGetName_SRC_P)
{
	m_pRemoteSceneList = m_pSMRemoteHelper->createRemoteSceneListInstance();
	if(m_pRemoteSceneList == NULL_PTR)
	{
		SET_FAILURE("did not find remote scene list instance");
		return;
	}

	m_pRemoteSceneList->setName(remoteSceneListName,std::bind(&SMRemoteSceneListTest_btc::onSetName, this, placeholders::_1));
	CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);

	std::string receivedName = "";

	try
	{
		receivedName = m_pRemoteSceneList->getName();
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside getName: " + std::string(e.what()));
		return;
	}

	if(receivedName == EMPTY_STRING)
	{
		SET_FAILURE("getName returns empty string");
	}

	SUCCEED();
}
#endif
