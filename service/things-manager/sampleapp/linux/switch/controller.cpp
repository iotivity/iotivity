//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <ITGMClientListener.h>
#include <TGMClient.h>


#include <iostream>
#include <mutex>
#include <Group.h>
#include <ActionSet.h>
#include <Capability.h>

using namespace std;


int g_Steps = 0;

SESSION_ID g_sessionID;
SESSION_ID g_FindDeviceCallsessionID;

OCPlatform* g_ocplatform;


ACTIONSET_ID gActionId;
std::vector<ACTIONSET_ID> g_ActionIdList;
std::vector<GroupInfo*> g_Grouplist;

class MyListener: public ITGMClientListener {

public:

	std::mutex m_Mutex;

	TGMClient* tgmClient;

	std::vector<Device*> mandatoryDeviceList;
	std::vector<Device*> optionalDevcieList;

	MyListener* setTGMClient(TGMClient* pClient) {
		cout << __FUNCTION__ << " Called(" << __LINE__ <<")." << endl;

		tgmClient = pClient;
		return this;
	}

	void onCreated(/*std::shared_ptr<OCResource> resource*/)
	{
		m_Mutex.lock();

		m_Mutex.unlock();
	}


	void onFindDevice(SESSION_ID sessionID,
		std::vector<Device*> mandatoryDeviceList,
		std::vector<Device*> optionalDevcie) {

		cout << __FUNCTION__ << " Called(" << __LINE__ <<")." << endl;

		m_Mutex.lock();
		if( g_FindDeviceCallsessionID == sessionID )
		{
			for(unsigned int i = 0 ; i < this->mandatoryDeviceList.size() ; ++i)
			{
				delete this->mandatoryDeviceList.at(i);
			}

			this->mandatoryDeviceList = mandatoryDeviceList;
			this->optionalDevcieList = optionalDevcie;

			g_Steps = 1;

		}
		else
		{
			cerr << __FUNCTION__ << " ERROR: SessionID Dose not match(" << __LINE__ <<")." << endl;
		}
		m_Mutex.unlock();
	}

	void onGroupStateChanged(SESSION_ID sessionID, GROUP_ID groupId,
		Device* device, DEVICE_STATE state) {
		cout << __FUNCTION__ << " Called(" << __LINE__ <<")." << endl;


		m_Mutex.lock();

		if( g_sessionID == sessionID )
		{
		}
		else
		{
			cerr << __FUNCTION__ << " ERROR: SessionID Dose not match(" << __LINE__ <<")." << endl;
		}
		m_Mutex.unlock();

	}

	void onGroup(SESSION_ID sessionID, GroupInfo* groupList) {
		cout << __FUNCTION__ << " Called(" << __LINE__ <<")." << endl;

		m_Mutex.lock();

		cout << "Session ID : " << sessionID << endl;
		//if( g_sessionID == sessionID )
		{
			if( groupList != NULL )
			{
				g_Grouplist.push_back(groupList);
				cout << "GROUPINFO RECEIVED" << endl;
			}
		}
		//else
		{
			//cerr << __FUNCTION__ << " ERROR: SessionID Dose not match(" << __LINE__ <<")." << endl;
		}

		m_Mutex.unlock();
	}

	void onDevice(SESSION_ID sessionID, Device* device) {
		cout << __FUNCTION__ << " Called(" << __LINE__ <<")." << endl;

		m_Mutex.lock();
		if( g_sessionID == sessionID )
		{

		}
		else
		{
			cerr << __FUNCTION__ << " ERROR: SessionID Dose not match(" << __LINE__ <<")." << endl;
		}
		m_Mutex.unlock();
	}

	void onGetActionSet(SESSION_ID sessionID, GROUP_ID groupId,
		ACTIONSET_ID actionSetId, ActionSet *actionSet, TGM_RESULT result) {
		cout << __FUNCTION__ << " Called(" << __LINE__ <<")." << endl;

		m_Mutex.lock();
		if( g_sessionID == sessionID )
		{


		}
		else
		{
			cerr << __FUNCTION__ << " ERROR: SessionID Dose not match(" << __LINE__ <<")." << endl;
		}
		m_Mutex.unlock();
	}

	void onActionSetOperation(SESSION_ID sessionID, GROUP_ID groupId,
		ACTIONSET_ID actionSetId, TGM_RESULT result) {
		cout << __FUNCTION__ << " Called(" << __LINE__ <<")." << endl;

		m_Mutex.lock();
		if( g_sessionID == sessionID )
		{

			gActionId = actionSetId;
			g_ActionIdList.push_back(actionSetId);


		}
		else
		{
			cerr << __FUNCTION__ << " ERROR: SessionID Dose not match(" << __LINE__ <<")." << endl;
		}
		m_Mutex.unlock();
	}
};

TGMClient* tgmClient;
MyListener *pListener;

void sendLightOnOffFromBookmark(std::string bookmarkData)
{
	cout << __FUNCTION__ << " : " << bookmarkData << endl;

	if(bookmarkData == "0")
	{
		tgmClient->executeActionSet(g_Grouplist[1]->m_guid, g_ActionIdList.at(2));
	}
	else
	{
		tgmClient->executeActionSet(g_Grouplist[1]->m_guid, g_ActionIdList.at(3));
	}
}

void onObserveForData(const OCRepresentation rep, const int& eCode, const int& sequenceNumber)
{
	if(eCode == SUCCESS_RESPONSE)
	{
        //std::cout << "ActionPutCallback >> " << std::endl;
        //std::string outStr;

		AttributeMap attributeMap = rep.getAttributeMap();

		std::cout << "onObserveForData" << std::endl;
		std::cout << "OBSERVE RESULT:"<<std::endl;
		std::cout << "\tSequenceNumber: "<< sequenceNumber << endl;

		if(attributeMap.find("level") != attributeMap.end())
		{
			std::cout << "\tAttribute"  << std::endl;
			std::cout << "\tlevel = "<< attributeMap["level"][0] << " ";
			
			sendLightOnOffFromBookmark(attributeMap["level"][0]);
		}
	}
}

//for bookmark
void onFoundResource(std::shared_ptr< OCResource > resource)
{
	cout << __FUNCTION__ << " Called(" << __LINE__ << ").\n" << endl;

	std::string resourceURI;
	std::string hostAddress;

	try
	{
		if (resource)
		{
            // Get the resource URI
			resourceURI = resource->uri();

            // Get the resource host address
			hostAddress = resource->host();
			//std::cout<<"\n\n ON FOUND RESOURCE" << resourceURI <<" \n\n\n\n" ;
			if (resourceURI.compare("/a/bookmark") == 0)
			{
				QueryParamsMap qp;
				resource->observe(OC::ObserveType::Observe,qp,&onObserveForData);
			}
			else
			{
				QueryParamsMap qp;
				resource->observe(OC::ObserveType::Observe,qp,&onObserveForData);

			}
		}
	}
	catch (std::exception& e)
	{

	}
}

void findResource(const std::string query)
{
	std::cout << "[Debug]:" << __func__ << ": Called. : " << query << std::endl;
	//g_platform->findResource("", query,  &onFoundResource); // not working (coap://x.x.x.x:xxxx/a/bookmark)

	g_ocplatform->findResource("", "coap://224.0.1.187/oc/core?rt=core.bookmark",  &onFoundResource);
}


int main(int argc, char** argv)
{

	//*********************************************************************
	// STEPS 0
	tgmClient = new TGMClient();
	pListener = new MyListener();
	g_ocplatform = tgmClient->getPlatform();

	tgmClient->bindListener(static_cast<ITGMClientListener*>(pListener));
	//*********************************************************************

	while (true) {
		cout << "input : ";
		cin >> g_Steps;

		if(g_Steps == 0)
		{
			//tgmClient->findResource();

			for(unsigned int i = 0 ; i < pListener->mandatoryDeviceList.size(); ++i)
			{
				OC::Device* device = pListener->mandatoryDeviceList.at(i);
				std::string targetStr = "" ;
				if(device->m_type == OC::DEVICE_TYPE::BOOKMARK)
				{
					std::cout<<device->m_duid<<std::endl;
					targetStr.append(device->m_duid);
					findResource(targetStr);
					break;
				}
				
			}

		}
		else if(g_Steps == 1)
		{


			//FindDevice
			//*********************************************************************
			vector<DEVICE_TYPE> mandatory_Device_Types;
			vector<DEVICE_TYPE> optional_Device_Types;

			mandatory_Device_Types.push_back(DEVICE_TYPE::BOOKMARK);
			mandatory_Device_Types.push_back(DEVICE_TYPE::LIGHT);
			mandatory_Device_Types.push_back(DEVICE_TYPE::MUSIC_PLAYER);

			g_FindDeviceCallsessionID = tgmClient->findDevice(mandatory_Device_Types,
				optional_Device_Types);
			//*********************************************************************
		}
		else if (g_Steps == 2)
		{	// Create Group
			g_sessionID = tgmClient->createGroup("LIGHT SERVICE");
		}
		else if(g_Steps == 3)
		{
			std::vector<Device*> bulbs;
			std::vector<Device*> empty_device_list;
			for( auto it = pListener->mandatoryDeviceList.begin(); 
				it != pListener->mandatoryDeviceList.end(); ++it)
			{
				if((*it)->m_type == OC::DEVICE_TYPE::LIGHT)
				{
					cout << "LIGHT DEVICE INFO" <<endl;
					cout << "=======================================================" << endl;
					cout << (*it)->m_name << endl;
					cout << (*it)->m_duid << endl;
					cout << "=======================================================" << endl;

					bulbs.push_back((*it));
				}
			}

			if( bulbs.size() > 0 )
			{
				g_sessionID = tgmClient->addGroupMember(g_Grouplist[0]->m_guid,
					bulbs, empty_device_list);
			}
		}
		else if(g_Steps == 4)
		{			
			
			g_sessionID = tgmClient->createGroup("THE CONTROL SERVICE");
			
		}
		else if(g_Steps == 5)
		{
			if( g_Grouplist.size() > 1)
			{
				if(pListener->mandatoryDeviceList.size() > 0 )
				{


					// Will be supported. but current should be sent each device.
					// because, base could not support enough space about coap message.
					// g_sessionID = tgmClient->addGroupMember(g_Grouplist[1]->m_guid,
					// 		pListener->mandatoryDeviceList, pListener->optionalDevcieList);
					for( auto it  = pListener->mandatoryDeviceList.begin();
						it != pListener->mandatoryDeviceList.end(); ++it)
					{
						std::vector<OC::Device*> man_device;
						std::vector<OC::Device*> option_device;

						man_device.push_back((*it));
						g_sessionID = tgmClient->addGroupMember(g_Grouplist[1]->m_guid,
					 		man_device, option_device);

						sleep(1);
					}

				} else {
					std::cout << "MENDATORY MEMBER IS NOT EXIST" << std::endl;
				}
			}
		}
		else if (g_Steps == 6) //CREATE TWO ACTION SET (ON / OFF)
		{

			// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			// ActionSet creation of all bulb off about LIGHTING SERVICE
			OC::ActionSet* actionsetofAllBulbOff = new OC::ActionSet();
			actionsetofAllBulbOff->m_name = "allbulboff";

			for( auto it = pListener->mandatoryDeviceList.begin();
				it != pListener->mandatoryDeviceList.end(); ++it)
			{
				OC::Action* action = NULL;

				if((*it)->m_type == OC::DEVICE_TYPE::LIGHT)
				{
					action = new OC::Action();
					action->m_duid = (*it)->m_duid;
					action->m_capability = new OC::Capability();
					action->m_capability->m_type = OC::CAPABILITY_TYPE::CAPABILITY_POWER_STATE;
					action->m_capability->m_data = "off";
				}

				if( action != NULL)
					actionsetofAllBulbOff->m_listActions.push_back(action);
			}

			g_sessionID = tgmClient->createActionSet(g_Grouplist[0]->m_guid, actionsetofAllBulbOff);
			sleep(1);
			delete actionsetofAllBulbOff;
			// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

			// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			// ActionSet creation of all bulb on about LIGHTING SERVICE
			OC::ActionSet* actionsetofAllBulbOn = new OC::ActionSet();
			actionsetofAllBulbOn->m_name = "allbulbon";			

			for( auto it = pListener->mandatoryDeviceList.begin();
				it != pListener->mandatoryDeviceList.end(); ++it)
			{
				OC::Action* action = NULL;

				if((*it)->m_type == OC::DEVICE_TYPE::LIGHT)
				{
					action = new OC::Action();
					action->m_duid = (*it)->m_duid;
					action->m_capability = new OC::Capability();
					action->m_capability->m_type = OC::CAPABILITY_TYPE::CAPABILITY_POWER_STATE;
					action->m_capability->m_data = "on";
				}

				if( action != NULL)
					actionsetofAllBulbOn->m_listActions.push_back(action);
			}
			
			g_sessionID = tgmClient->createActionSet(g_Grouplist[0]->m_guid, actionsetofAllBulbOn);
			sleep(1);
			delete actionsetofAllBulbOn;
			// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			
			// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			// ActionSet creation of all bulb on about THE CONTROL SERVICE
			OC::ActionSet* actionsetofTurnOnAllBulbandMusic = new OC::ActionSet();
			actionsetofTurnOnAllBulbandMusic->m_name = "trun on the light and music";			

			for(auto it = pListener->mandatoryDeviceList.begin();
				it != pListener->mandatoryDeviceList.end(); ++it)
			{
				OC::Action* action = NULL;
				if((*it)->m_type == OC::DEVICE_TYPE::LIGHT)
				{					
					action = new OC::Action();
					action->m_duid = (*it)->m_duid;
					action->m_capability = new OC::Capability();
					action->m_capability->m_type = OC::CAPABILITY_TYPE::CAPABILITY_POWER_STATE;
					action->m_capability->m_data = "on";
				}
				else if((*it)->m_type == OC::DEVICE_TYPE::MUSIC_PLAYER)
				{
					action = new OC::Action();
					action->m_duid = (*it)->m_duid;
					action->m_capability = new OC::Capability();
					action->m_capability->m_type = OC::CAPABILITY_TYPE::CAPABILITY_POWER_STATE;
					action->m_capability->m_data = "on";
				}

				if( action != NULL)
					actionsetofTurnOnAllBulbandMusic->m_listActions.push_back(action);
			}
			
			g_sessionID = tgmClient->createActionSet(g_Grouplist[1]->m_guid, actionsetofTurnOnAllBulbandMusic);
			sleep(1);
			delete actionsetofTurnOnAllBulbandMusic;
			// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

			// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			// ActionSet creation of all bulb on about THE CONTROL SERVICE
			OC::ActionSet* actionsetofTurnOffAllBulbandMusic = new OC::ActionSet();
			actionsetofTurnOffAllBulbandMusic->m_name = "trun off the light and music";			

			for(auto it = pListener->mandatoryDeviceList.begin();
				it != pListener->mandatoryDeviceList.end(); ++it)
			{
				OC::Action* action = NULL;
				if((*it)->m_type == OC::DEVICE_TYPE::LIGHT)
				{					
					action = new OC::Action();
					action->m_duid = (*it)->m_duid;
					action->m_capability = new OC::Capability();
					action->m_capability->m_type = OC::CAPABILITY_TYPE::CAPABILITY_POWER_STATE;
					action->m_capability->m_data = "off";
				}
				else if((*it)->m_type == OC::DEVICE_TYPE::MUSIC_PLAYER)
				{
					action = new OC::Action();
					action->m_duid = (*it)->m_duid;
					action->m_capability = new OC::Capability();
					action->m_capability->m_type = OC::CAPABILITY_TYPE::CAPABILITY_POWER_STATE;
					action->m_capability->m_data = "off";
				}

				if( action != NULL)
					actionsetofTurnOffAllBulbandMusic->m_listActions.push_back(action);
			}

			g_sessionID = tgmClient->createActionSet(g_Grouplist[1]->m_guid, actionsetofTurnOffAllBulbandMusic);
			sleep(1);
			delete actionsetofTurnOffAllBulbandMusic;
			// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		}
		else if(g_Steps == 8) //ON
		{
			if(!g_Grouplist.empty())
			{
				g_sessionID = tgmClient->executeActionSet(g_Grouplist[0]->m_guid, g_ActionIdList.at(0));
			}
			else
			{
				std::cout << "GROUP IS NOT EXIST" << std::endl;
			}

		}
		else if(g_Steps == 9) //OFF
		{
			if(!g_Grouplist.empty())
			{
				g_sessionID = tgmClient->executeActionSet(g_Grouplist[0]->m_guid, g_ActionIdList.at(1));
			}
			else
			{
				std::cout << "GROUP IS NOT EXIST" << std::endl;
			}
		}
		sleep(1);
	}

	return 0;
}

