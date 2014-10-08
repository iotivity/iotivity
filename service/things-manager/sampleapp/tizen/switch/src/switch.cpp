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

/// @file switch.cpp

/// @brief

#include <appcore-efl.h>
#include <Ecore_X.h>
#include <efl_assist.h>
#include <iostream>
#include <mutex>

#include <Group.h>
#include <ActionSet.h>
#include <Capability.h>
#include <Device.h>
#include <ITGMClientListener.h>
#include "TGMClient.h"				
#include "switch.h"

static TGMClient *g_tgmClient;

static Evas_Object *g_btn;
static Evas_Object *g_imageLightOff;
static Evas_Object *g_imageLightOn;
static Evas_Object *g_btn1;
static Evas_Object *g_btn2;
static Evas_Object *g_btn3;
static Evas_Object *g_btn4;
static Evas_Object *g_btn5;

static int g_LightFlag;

using namespace std;

SESSION_ID g_sessionID;
SESSION_ID g_FindDeviceCallsessionID;

OCPlatform* g_ocplatform;

ACTIONSET_ID g_ActionId;
std::vector<ACTIONSET_ID> g_ActionIdList;
std::vector<GroupInfo*> g_Grouplist;

class MyListener: public ITGMClientListener {

public:

	std::mutex m_mutex;

	std::vector<OC::Device*> m_mandatoryDeviceList;
	std::vector<OC::Device*> m_optionalDeviceList;


	void onCreated()
	{
		m_mutex.lock();

		printf("\nLOG : onCreated Call\n");

		elm_object_text_set(g_btn1, "FIND DEVICE");
		evas_object_resize(g_btn1, 340, 50);	
		evas_object_move(g_btn1, 170, 900);

		evas_object_show(g_btn1);

		if(!evas_object_visible_get(g_btn1)){
			printf("\nLOG : evas_object_visible_get FALSE\n");
			evas_object_show(g_btn1);
		}
		else
			printf("\nLOG : SHOW OK \n");

		m_mutex.unlock();
	}


	void onFindDevice(SESSION_ID sessionID,
			std::vector<OC::Device*> mandatoryDeviceList,
			std::vector<OC::Device*> optionalDevice) {

		cout << __FUNCTION__ << " Called(" << __LINE__ <<")." << endl;

		m_mutex.lock();

		if( g_FindDeviceCallsessionID == sessionID )
		{
			for(unsigned int i = 0 ; i < this->m_mandatoryDeviceList.size() ; ++i)
			{
				delete this->m_mandatoryDeviceList.at(i);
			}

			this->m_mandatoryDeviceList = mandatoryDeviceList;
			this->m_optionalDeviceList = optionalDevice;

		}
		else
		{
		    cerr << __FUNCTION__ << " ERROR: SessionID Dose not match(" << __LINE__ <<")." << endl;
		}
		
		elm_object_text_set(g_btn2, "CREATE GROUP");
		evas_object_resize(g_btn2, 340, 50);
		evas_object_move(g_btn2, 10, 990);
		evas_object_show(g_btn2);

		m_mutex.unlock();
	}

	void onGroupStateChanged(SESSION_ID sessionID, GROUP_ID groupId,
			OC::Device* device, OC::DEVICE_STATE state) {
		cout << __FUNCTION__ << " Called(" << __LINE__ <<")." << endl;


		m_mutex.lock();

        if( g_sessionID == sessionID )
        {
        }
		else
        {
            cerr << __FUNCTION__ << " ERROR: SessionID Dose not match(" << __LINE__ <<")." << endl;
        }

        m_mutex.unlock();

	}

	void onGroup(SESSION_ID sessionID, GroupInfo* groupList) {
		cout << __FUNCTION__ << " Called(" << __LINE__ <<")." << endl;

		m_mutex.lock();

		cout << "Session ID : " << sessionID << endl;
		if( groupList != NULL )
		{
		    g_Grouplist.push_back(groupList);
		    cout << "GROUPINFO RECEIVED" << endl;
		}
		else
		{
		    cerr << __FUNCTION__ << " ERROR: SessionID Dose not match(" << __LINE__ <<")." << endl;
		}

		elm_object_text_set(g_btn3, "CREATE ACTION");
		evas_object_resize(g_btn3, 340, 50);
		evas_object_move(g_btn3, 350, 990);
		evas_object_show(g_btn3);

		m_mutex.unlock();
	}

	void onDevice(SESSION_ID sessionID, OC::Device* device) {
		cout << __FUNCTION__ << " Called(" << __LINE__ <<")." << endl;

		m_mutex.lock();
		if( g_sessionID == sessionID )
		{

		}
		else
        {
            cerr << __FUNCTION__ << " ERROR: SessionID Dose not match(" << __LINE__ <<")." << endl;
        }
		m_mutex.unlock();
	}

	void onGetActionSet(SESSION_ID sessionID, GROUP_ID groupId,
			ACTIONSET_ID actionSetId, ActionSet *actionSet, TGM_RESULT result) {
		cout << __FUNCTION__ << " Called(" << __LINE__ <<")." << endl;

		m_mutex.lock();
		if( g_sessionID == sessionID )
		{


		}
        else
        {
            cerr << __FUNCTION__ << " ERROR: SessionID Dose not match(" << __LINE__ <<")." << endl;
        }
		m_mutex.unlock();
	}

	void onActionSetOperation(SESSION_ID sessionID, GROUP_ID groupId,
			ACTIONSET_ID actionSetId, TGM_RESULT result) {
		cout << __FUNCTION__ << " Called(" << __LINE__ <<")." << endl;

		m_mutex.lock();

		if( g_sessionID == sessionID )
		{
		    g_ActionId = actionSetId;
			g_ActionIdList.push_back(actionSetId);
		}
        else
        {
            cerr << __FUNCTION__ << " ERROR: SessionID Dose not match(" << __LINE__ <<")." << endl;
        }
		m_mutex.unlock();
	}
};


static MyListener *g_pListener;

static Eina_Bool back_cb(void *data, Elm_Object_Item *item)
{
	elm_exit();

	return EINA_FALSE;
}


static void win_del(void *data, Evas_Object *obj, void *event)
{
	DBG("test win_del()");

	elm_exit();
}

static Evas_Object* create_win(const char *name)
{
	Evas_Object *eo;
	int w, h;

	eo = elm_win_add(NULL, name, ELM_WIN_BASIC);
	if (eo) {
		elm_win_title_set(eo, name);
		elm_win_borderless_set(eo, EINA_TRUE);
		evas_object_smart_callback_add(eo, "delete,request", win_del, NULL);
		ecore_x_window_size_get(ecore_x_window_root_first_get(), &w, &h);
		evas_object_resize(eo, w, h);
		elm_win_indicator_mode_set(eo, ELM_WIN_INDICATOR_SHOW);
		elm_win_indicator_opacity_set(eo, ELM_WIN_INDICATOR_OPAQUE);
	}

	evas_object_show(eo);

	return eo;
}

static Evas_Object* create_bg(Evas_Object *parent)
{
	Evas_Object *bg;

	bg = elm_bg_add(parent);
	evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(parent, bg);

	evas_object_show(bg);

	return bg;
}

static Evas_Object* create_conformant(Evas_Object* parent)
{

	Evas_Object *conformant;
	conformant = elm_conformant_add(parent);

	if(NULL == conformant)
		DBG("elm_conformant_add() Failed");

	evas_object_size_hint_weight_set(conformant, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(parent, conformant);
	evas_object_show(conformant);

	return conformant;
}

static void on_clickExit(void *data, Evas_Object *obj, void *event_info)
{
	elm_exit();
}


static void on_clickLight(void *data, Evas_Object *obj, void *event_info)
{

	if(g_LightFlag == 0){
		printf("LOG : event occur --- light-off >> light-on\n");

		evas_object_hide(g_imageLightOff);
		evas_object_show(g_imageLightOn);


		if(!g_Grouplist.empty())
		{
		    g_sessionID = g_tgmClient->executeActionSet(g_Grouplist[0]->m_guid, g_ActionIdList.at(1));
		}
		else
		{
			std::cout << "GROUP IS NOT EXIST" << std::endl;
		}

		g_LightFlag = 1;
	
	}
	else{
		printf("LOG : event occur --- light-on >> light-off\n");

		evas_object_hide(g_imageLightOn);
		evas_object_show(g_imageLightOff);

	   	if(!g_Grouplist.empty())
		{
		    g_sessionID = g_tgmClient->executeActionSet(g_Grouplist[0]->m_guid, g_ActionIdList.at(0));
		}
		else
		{
			std::cout << "GROUP IS NOT EXIST" << std::endl;
		}

		g_LightFlag = 0;
	}

}


static void on_clickStep1(void *data, Evas_Object *obj, void *event_info)
{
	// STEP1. FIND DEVICE
	vector<OC::DEVICE_TYPE> mandatory_Device_Types;
	vector<OC::DEVICE_TYPE> optional_Device_Types;

	mandatory_Device_Types.push_back(OC::DEVICE_TYPE::LIGHT);
	mandatory_Device_Types.push_back(OC::DEVICE_TYPE::BOOKMARK);
	mandatory_Device_Types.push_back(OC::DEVICE_TYPE::MUSIC_PLAYER);

	optional_Device_Types.push_back(OC::DEVICE_TYPE::AIR_CONDITIONER);

	g_FindDeviceCallsessionID = g_tgmClient->findDevice(mandatory_Device_Types,
			optional_Device_Types);

	sleep(1);


}

static void on_clickStep2(void *data, Evas_Object *obj, void *event_info)
{
	// STEP2. CREATE GROUP
	std::vector<OC::Device*> bulbs;

	for( auto it = g_pListener->m_mandatoryDeviceList.begin(); 
			it != g_pListener->m_mandatoryDeviceList.end(); ++it)
	{
		if((*it)->m_type == OC::DEVICE_TYPE::LIGHT)
		{
		    cout << "TESTTEST" <<endl;
		    cout << "=======================================================" << endl;
		    cout << (*it)->m_name << endl;
            cout << (*it)->m_duid << endl;
		    cout << "=======================================================" << endl;

			bulbs.push_back((*it));
		}
	}


	if( bulbs.size() > 0 )
	{
		g_sessionID = g_tgmClient->createGroup("LIGHTING SERVICE",
					bulbs, g_pListener->m_optionalDeviceList);
	}

	sleep(1);

	if(g_pListener->m_mandatoryDeviceList.size() != 0)
	{
		g_sessionID = g_tgmClient->createGroup("THE CONTROL SERVICE",
				g_pListener->m_mandatoryDeviceList, g_pListener->m_optionalDeviceList);
	}
	else
	{
		std::cout << "MENDATORY MEMBER IS NOT EXIST" << std::endl;
	}	

	sleep(1);

	return;
}

static void on_clickStep3(void *data, Evas_Object *obj, void *event_info)
{

	// STEP3. CREATE ACTION SET
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// ActionSet creation of all bulb off about LIGHTING SERVICE
	OC::ActionSet* actionsetofAllBulbOff = new OC::ActionSet();
	actionsetofAllBulbOff->m_name = "allbulboff";

	for( auto it = g_pListener->m_mandatoryDeviceList.begin();
	                    it != g_pListener->m_mandatoryDeviceList.end(); ++it)
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

	g_sessionID = g_tgmClient->createActionSet(g_Grouplist[0]->m_guid, actionsetofAllBulbOff);
	sleep(2);
	delete actionsetofAllBulbOff;
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// ActionSet creation of all bulb on about LIGHTING SERVICE
	OC::ActionSet* actionsetofAllBulbOn = new OC::ActionSet();
	actionsetofAllBulbOn->m_name = "allbulbon";			

	for( auto it = g_pListener->m_mandatoryDeviceList.begin();
	                                it != g_pListener->m_mandatoryDeviceList.end(); ++it)
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
	
	g_sessionID = g_tgmClient->createActionSet(g_Grouplist[0]->m_guid, actionsetofAllBulbOn);
	sleep(2);
	delete actionsetofAllBulbOn;
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// ActionSet creation of all bulb on about THE CONTROL SERVICE
	OC::ActionSet* actionsetofTurnOnAllBulbandMusic = new OC::ActionSet();
	actionsetofTurnOnAllBulbandMusic->m_name = "trun on the light and music";			

	static bool isFirst = true;

	for(auto it = g_pListener->m_mandatoryDeviceList.begin();
			 it != g_pListener->m_mandatoryDeviceList.end(); ++it)
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

		if( action != NULL && ((*it)->m_type == OC::DEVICE_TYPE::MUSIC_PLAYER || isFirst == true))
		{
			actionsetofTurnOnAllBulbandMusic->m_listActions.push_back(action);

			if(isFirst == true && (*it)->m_type == OC::DEVICE_TYPE::LIGHT)
				isFirst = false;
		}
	}

	g_sessionID = g_tgmClient->createActionSet(g_Grouplist[1]->m_guid, actionsetofTurnOnAllBulbandMusic);
	sleep(2);
	delete actionsetofTurnOnAllBulbandMusic;
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// ActionSet creation of all bulb on about THE CONTROL SERVICE
	OC::ActionSet* actionsetofTurnOffAllBulbandMusic = new OC::ActionSet();
	actionsetofTurnOffAllBulbandMusic->m_name = "trun off the light and music";			

	for(auto it = g_pListener->m_mandatoryDeviceList.begin();
			 it != g_pListener->m_mandatoryDeviceList.end(); ++it)
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

	g_sessionID = g_tgmClient->createActionSet(g_Grouplist[1]->m_guid, actionsetofTurnOffAllBulbandMusic);
	sleep(2);
	delete actionsetofTurnOffAllBulbandMusic;
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


	// show bulb image

	char buf[PATH_MAX];
	char buf2[PATH_MAX];
	
	g_imageLightOn = elm_image_add(g_btn);
	g_imageLightOff = elm_image_add(g_btn);

	snprintf(buf, sizeof(buf), "%s/usr/apps/com.samsung.oicapp-test/res/images/lightOff.png", elm_app_data_dir_get());
	elm_image_file_set(g_imageLightOff, buf, NULL);

	snprintf(buf2, sizeof(buf2), "%s/usr/apps/com.samsung.oicapp-test/res/images/lightOn.png", elm_app_data_dir_get());
	elm_image_file_set(g_imageLightOn, buf2, NULL);

	printf("LOG : evas_object_image_file set ok \n");

	evas_object_resize(g_btn, 400, 400);
	evas_object_move(g_btn, 170, 400);
	evas_object_color_set(g_btn, 0, 0, 0, 0);

	evas_object_resize(g_imageLightOn, 400, 400);
	evas_object_move(g_imageLightOn, 170, 400);

	evas_object_resize(g_imageLightOff, 400, 400);
	evas_object_move(g_imageLightOff, 170, 400);

	evas_object_smart_callback_add(g_btn, "clicked", on_clickLight, NULL);

	evas_object_show(g_btn);
	evas_object_hide(g_imageLightOn);
	evas_object_show(g_imageLightOff);

	elm_object_text_set(g_btn4, "SUBSCRIBE BOOKMARK");
	evas_object_resize(g_btn4, 450, 50);
	evas_object_move(g_btn4, 125, 1080);
//	evas_object_smart_callback_add(g_btn4, "clicked", on_clickStep4, NULL);
	evas_object_show(g_btn4);


	return;
}

void sendLightOnOffFromBookmark(std::string bookmarkData)
{
	cout << __FUNCTION__ << " : " << bookmarkData << endl;

	if(bookmarkData == "0")
	{
		g_tgmClient->executeActionSet(g_Grouplist[1]->m_guid, g_ActionIdList.at(2));
	}
	else
	{
		g_tgmClient->executeActionSet(g_Grouplist[1]->m_guid, g_ActionIdList.at(3));
	}
}

void onObserveForData(const OCRepresentation rep, const int& eCode, const int& sequenceNumber)
{
    if(eCode == SUCCESS_RESPONSE)
    {
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

	g_ocplatform->findResource("", "coap://224.0.1.187/oc/core?rt=core.bookmark",  &onFoundResource);
}


static void on_clickStep4(void *data, Evas_Object *obj, void *event_info)
{
	// STEP4. OBSERVE BOOKMARK
	for(unsigned int i = 0 ; i < g_pListener->m_mandatoryDeviceList.size(); ++i)
	{
		OC::Device* device = g_pListener->m_mandatoryDeviceList.at(i);
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


static void init_button(void *data)
{
	SwitchData *sd = (SwitchData *)data;

	g_tgmClient = new TGMClient();
	g_pListener = new MyListener();
	g_ocplatform = g_tgmClient->getPlatform();

	g_tgmClient->bindListener(static_cast<ITGMClientListener*>(g_pListener));

	g_btn = NULL;
	sd->btn = g_btn;
	g_btn = elm_button_add(sd->win);

	g_imageLightOff = NULL;
	g_imageLightOn = NULL;
	g_LightFlag = 0 ;

	g_btn1 = elm_button_add(sd->win);
	g_btn2 = elm_button_add(sd->win);
	g_btn3 = elm_button_add(sd->win);
	g_btn4 = elm_button_add(sd->win);
	g_btn5 = elm_button_add(sd->win);


	elm_object_text_set(g_btn5, "EXIT");
	evas_object_resize(g_btn5, 340, 50);
	evas_object_move(g_btn5, 170, 1170);

	evas_object_smart_callback_add(g_btn1, "clicked", on_clickStep1, NULL);
	evas_object_smart_callback_add(g_btn2, "clicked", on_clickStep2, NULL);
	evas_object_smart_callback_add(g_btn3, "clicked", on_clickStep3, NULL);
	evas_object_smart_callback_add(g_btn4, "clicked", on_clickStep4, NULL);
	evas_object_smart_callback_add(g_btn5, "clicked", on_clickExit, NULL);

	evas_object_hide(g_btn1);
	evas_object_show(g_btn5);

	printf("LOG : evas_object_show ok \n");
}

static Evas_Object* create_layout_main(Evas_Object* parent)
{
	Evas_Object *layout;

	if (parent == NULL)
		return NULL;

	layout = elm_layout_add(parent);

	elm_layout_theme_set(layout, "layout", "application", "default");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(layout);

	return layout;
}

static Evas_Object* create_naviframe_layout(Evas_Object* parent)
{
	Evas_Object *nf;

	if (parent == NULL) return NULL;

	nf = elm_naviframe_add(parent);

	elm_object_part_content_set(parent, "elm.swallow.content", nf);
	evas_object_show(nf);

	return nf;
}

static void create_view_layout(void *data)
{
	SwitchData *sd = (SwitchData *)data;

	Elm_Object_Item *it;

	if (sd == NULL) return;
	if (sd->nf == NULL) return;

	ea_object_event_callback_add(sd->nf, EA_CALLBACK_BACK, ea_naviframe_back_cb, NULL);
	ea_object_event_callback_add(sd->nf, EA_CALLBACK_MORE, ea_naviframe_more_cb, NULL);

	it = elm_naviframe_item_push(sd->nf, "Controller", NULL, NULL, NULL, NULL);
	elm_naviframe_item_pop_cb_set(it, back_cb, sd);

	return;
}


static int switchapp_create(void *data)
{
	SwitchData *sd = (SwitchData *)data;

	/* create window */
	sd->win = create_win(PACKAGE);
	if (sd->win == NULL)
		return -1;

	sd->bg = create_bg(sd->win);
	if (NULL == sd->bg)
		return -1;

	sd->conformant = create_conformant(sd->win);
	if(NULL == sd->conformant)
		return -1;

	/* create layout */
	sd->layout_main = create_layout_main(sd->conformant);
	if (sd->layout_main == NULL)
		return -1;

	elm_object_content_set(sd->conformant, sd->layout_main);
	elm_win_conformant_set(sd->win, EINA_TRUE);

	// Indicator
	elm_win_indicator_mode_set(sd->win, ELM_WIN_INDICATOR_SHOW);

	// Naviframe
	sd->nf = create_naviframe_layout(sd->layout_main);
	// Naviframe Content
	create_view_layout(sd);

	init_button(sd);		


	return 0;
}

static int switchapp_terminate(void *data)
{
	SwitchData *sd = (SwitchData *)data;

	if (sd->win)
		evas_object_del(sd->win);

	return 0;
}

static int switchapp_pause(void *data)
{
	return 0;
}


static int switchapp_resume(void *data)
{
	return 0;
}

static int switchapp_reset(bundle *b, void *data)
{
	SwitchData *sd = (SwitchData *)data;

	if (sd->win)
		elm_win_activate(sd->win);

	return 0;
}

int main(int argc, char *argv[])
{
	SwitchData sd;

	appcore_ops ops;
	ops.create = switchapp_create;
	ops.terminate = switchapp_terminate;
	ops.pause = switchapp_pause;
	ops.resume = switchapp_resume;
	ops.reset = switchapp_reset;

	memset(&sd, 0x0, sizeof(SwitchData));
	ops.data = &sd;

	return appcore_efl_main(PACKAGE, &argc, &argv, &ops);
}
