/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

/**
 * This file contains the exported symbol.
 */
#include <stdlib.h>
#include <map>
#include <string>
#include <sstream>
#include <iostream>

#include "ITS.h"
#include "GeneralData.h"
#include "SysTimer.h"

#include "tizen_public_ble.h"

using namespace ITSName;

//#define __INTERNAL_DEBUG__
#define SENSOR_NAME "IndoorTrajectorySensor"


#define INPUT_EA            4
#define SEQ_ID                  0
#define SEQ_DISTANCE        1
#define SEQ_PROXIMITY       2
#define SEQ_SD                  3



#define TRACKEEID_NAME          "trackeeID"
#define THINGS_NAME                 "things"
char *inputName[INPUT_EA] =
{ (char *)"ID", (char *)"distance", (char *)"proximity", (char *)"SD" };

#define INDEX_TRACKER_THING     0
physicalInput ITS::s_PHYSICAL_SENSORs[PHYSICAL_EA] =
{
    { (char *)"Tracker_Thing", INPUT_EA, (void *) &inputName },
//  { (char*)"Thing_TempHumSensor1", INPUT_EA, (void*)&inputName }
};

ICtxDelegate *g_pDelegate;
ITS *ITS::singleton = NULL;

// map<TrackeeID, Trajectory>
typedef std::map < std::string, Trajectory >        TrajectoryMap;
TrajectoryMap trajectorys;


void parserTracker_Thing( int seq, std::vector< GeneralData > &gVector, CurrentService *CService,
                          Things **output )
{
#ifdef __INTERNAL_DEBUG__
    std::cout << "[DEBUG] " << __func__ << " is called." << std::endl;
#endif

    int isted_cnt = 0;

    int InputNum = ITS::s_PHYSICAL_SENSORs[seq].m_inputNum;
    char **pchar = (char **) (ITS::s_PHYSICAL_SENSORs[seq].m_pInputStruct);
    if (InputNum == 0)
    {
        std::cout << "No input List." << std::endl;
        return ;
    }


    std::string name = "";
    gVector[0].get_Name(name);

    if (name.compare(TRACKEEID_NAME) == 0 )
    {
        gVector[0].get( &(CService->trackee.ID) );
    }

    if ( CService->trackee.ID == "" || CService->trackee.ID.length() == 0 )
    {
        printf("Error : trackee.ID is NULL.\n");
        return ;
    }

    CService->service.timestamp = SysTimer::MilliSecondTime();


    name = "";
    gVector[1].get_Name(name);

    if (name.compare(THINGS_NAME) == 0 )
    {
        int ea = 0;
        gVector[1].get(&ea);

        if ( *output)
        {
            delete *output;
            *output = 0;
        }

        if ( *output == NULL )
        {
            *output = new Things(ea);
        }
    }

    if ( *output == NULL )
    {
        printf("Error : m_TList is NULL.\n");
        return ;
    }


    for (unsigned int j = 2; j < gVector.size(); j++)
    {
        gVector[j].get_Name(name);

        int l = 0;
        for ( l = 0; l < InputNum ; l++ )
        {
            if (name.compare(*(pchar + l)) == 0)
                break;
        }

        switch (l)
        {
            case SEQ_ID:        // id
                gVector[j].get( &((*output)->things[isted_cnt / INPUT_EA].ID) );
                break;
            case SEQ_DISTANCE:      // distance
                gVector[j].get( &((*output)->things[isted_cnt / INPUT_EA].distance) );
                break;
            case SEQ_PROXIMITY: // proximity
                gVector[j].get( &((*output)->things[isted_cnt / INPUT_EA].proximity) );
                break;
            case SEQ_SD:    // SD
                gVector[j].get( &((*output)->things[isted_cnt / INPUT_EA].SD) );
                break;
            default:
                std::cout << "Not Yet Supported Option." << std::endl;
                return ;
        }   // switch

        isted_cnt++;
    }   // for
}


void InitializeContext(ICtxDelegate *pDelegate)
{
    std::vector < ContextData > contextData;

    ITS *eventCls = ITS::GetInstance();
    pDelegate->registerCallback(eventCls);
    g_pDelegate = pDelegate;

    proximity_init(ITS::SelfTrajectoryThread);
    ble_init(proximity_loop);
    std::cout << "ITS sensor loaded -02-" << std::endl;

    return;
}


/******************************************************
 * Loop function based thread.
 */
void *ITS::SelfTrajectoryThread(void *param)
{
//#ifdef __INTERNAL_DEBUG__
    std::cout << "[DEBUG] ITS::" << __func__ << " is called." << std::endl;
//#endif

    ContextData out;
    CurrentService CService;
    Things *TList = (Things *)param;
    Trajectory *tra = 0;
    ITSResult flag = ERROR;
    ITS *its = ITS::GetInstance();

    CService.trackee.ID = "SelfTrackee";
    CService.service.timestamp = SysTimer::MilliSecondTime();

    if ( (flag = its->searchValidService( TList, &CService )) != SUCCESS )
    {
        if ( flag == NO_ELEMENT )
            return 0;

        std::cout << "Error : searchValidService() function failed." << std::endl;
        return NULL;
    }

    if ( (tra = its->makeTrajectory(&CService)) == NULL)
    {
        std::cout << "Error : makeTrajectory() function failed." << std::endl;
        return NULL;
    }

    if ( its->setOutput( tra, &out ) != SUCCESS )
    {
        std::cout << "Error : setOutput() function failed." << std::endl;
        return NULL;
    }

    std::vector < ContextData > outList;
    outList.push_back(out);
    g_pDelegate->addOutput(outList);

    return NULL;
}

ITS *ITS::GetInstance( void )
{
    if ( singleton == NULL )
        singleton = new ITS();

    return singleton;
}

ITS::ITS()
{
#ifdef __INTERNAL_DEBUG__
    std::cout << "[DEBUG] ITS::" << __func__ << " is called." << std::endl;
#endif

    m_TList = 0;
    m_closestThing = 0;
}

ITS::~ITS()
{
#ifdef __INTERNAL_DEBUG__
    std::cout << "[DEBUG] ITS::" << __func__ << " is called." << std::endl;
#endif

    if (m_TList)
    {
        delete m_TList;
        m_TList = 0;
        m_closestThing = 0;
    }

    if ( singleton )
        delete singleton;
}

void ITS::onCtxEvent(enum CTX_EVENT_TYPE eventType,
                     std::vector< ContextData > contextDataList)
{
    switch (eventType)
    {
        case SPF_START:
            runLogic(contextDataList);
            break;
        default:
            std::cout << "Not support onCtxEvent" << std::endl;
            break;
    }
}

int ITS::runLogic(std::vector< ContextData > &contextDataList)
{
#ifdef __INTERNAL_DEBUG__
    std::cout << "[DEBUG] ITS::" << __func__ << " is called." << std::endl;
#endif

    ContextData out;
    CurrentService CService;
    Trajectory *tra = 0;
    ITSResult flag = ERROR;

    if (getInput(contextDataList, &CService) != SUCCESS)
    {
        std::cout << "Error : getInput() function failed." << std::endl;
        return -1;
    }

    if ( (flag = searchValidService( m_TList, &CService )) != SUCCESS )
    {
        if ( flag == NO_ELEMENT )
            return 0;

        std::cout << "Error : searchValidService() function failed." << std::endl;
        return -1;
    }

    if ( (tra = makeTrajectory(&CService)) == NULL)
    {
        std::cout << "Error : makeTrajectory() function failed." << std::endl;
        return -1;
    }

    if ( setOutput( tra, &out ) != SUCCESS )
    {
        std::cout << "Error : setOutput() function failed." << std::endl;
        return -1;
    }

    std::vector < ContextData > outList;
    outList.push_back(out);
    g_pDelegate->addOutput(outList);

    return 0;
}

/**
 * Get Input data (temperature, humidity) using resource Client of Iotivity base.
 */
ITSResult ITS::getInput(std::vector< ContextData > &contextDataList, CurrentService *CService )
{
#ifdef __INTERNAL_DEBUG__
    std::cout << "[DEBUG] ITS::" << __func__ << " is called." << std::endl;
#endif

    int contextSize = 0;

    if ((contextSize = contextDataList.size()) == 0)
    {
        std::cout << "Physical Context data is not exist." << std::endl;
        return ERROR;
    }

    for (int i = 0; i < contextSize; i++)
    {
        for (int k = 0; k < PHYSICAL_EA; k++)
        {
            if (contextDataList[i].rootName == s_PHYSICAL_SENSORs[k].m_thingName)
            {
                std::vector< GeneralData > gVector;
                Conversion_DataFormat( contextDataList[i].outputProperty, gVector );

                switch ( k )
                {
                    case INDEX_TRACKER_THING:
                        parserTracker_Thing( k, gVector, CService, &m_TList);
                        break;
                    default :
                        break;
                }

            } // if
            break;
        } // for
    }

    std::cout << "Success : " << __func__ << std::endl;
    return SUCCESS;
}

/**
 * searchValidService Function.
  */
ITSResult ITS::searchValidService( Things *things , CurrentService *CService )
{
#ifdef __INTERNAL_DEBUG__
    std::cout << "[DEBUG] ITS::" << __func__ << " is called." << std::endl;
#endif

    if ( things == NULL )
    {
        printf("Error : input Things List is null.\n");
        return ERROR;
    }

#ifdef __INTERNAL_DEBUG__
    for ( int i = 0; i < things->EA ; i++ )
    {
        printf("ID=%s, distance=%f, proximity=%d, SD=%f\n", things->things[i].ID.c_str(),
               things->things[i].distance, things->things[i].proximity, things->things[i].SD );
    }
#endif
    int validThingEA = 0;
    for ( int i = 0; i < things->EA ; i++ )
    {
        if ( things->things[i].proximity <= 1 )
        {
            ThingInfo validThing;

            validThing.ID = things->things[i].ID;
            //      validThing.ServiceType = things->things[i].ServiceType;
            validThing.proximity = things->things[i].proximity;
            CService->service.insert(validThing);
            validThingEA++;
        }
    }

    if ( CService->service.getThingsEA() == NULL )
    {
        printf("Valid Thing elements is not exist.\n");
        return NO_ELEMENT;
    }

#ifdef __INTERNAL_DEBUG__
    MAP_TYPE::iterator itr = CService->service.ThingBegin();
    for ( ; itr != CService->service.ThingEnd() ; itr++ )
    {
        printf("CService: ID=%s, proximity=%d\n", itr->second.ID.c_str() , itr->second.proximity );
    }
#endif

    return SUCCESS;
}

/**
 * makeTrajectory Function.
  */
Trajectory *ITS::makeTrajectory(CurrentService *CService)
{
#ifdef __INTERNAL_DEBUG__
    std::cout << "[DEBUG] ITS::" << __func__ << " is called." << std::endl;
#endif

    if ( CService == NULL )
    {
        printf("Error : input Current Service is null.\n");
        return NULL;
    }

    // Trajectory Code contents.
    Trajectory *tra = 0;
    TrajectoryMap::iterator itr = trajectorys.find( CService->trackee.ID );

    if ( itr == trajectorys.end() )
    {
        printf("itr == trajectorys.end().\n");
        Trajectory tempTra(CService->trackee.ID);
        trajectorys[CService->trackee.ID] = tempTra;
        tra = &trajectorys[CService->trackee.ID];
        printf("itr = %d\n", tra);
    }
    else
    {
        printf("itr is find trackeeID in trajectorys.\n");
        tra = &itr->second;
    }

    tra->TrajectoryUpdate( *CService );

    return tra;
}

std::string intToString(int n)
{
    std::stringstream ss;
    ss << n;
    return ss.str();
}

std::string floatToString(float n)
{
    std::stringstream ss;
    ss << n;
    return ss.str();
}

#define OUTPUT_TRAJECTORY_SIZE      2
ITSResult ITS::setOutput( Trajectory *m_result, ContextData *out )
{
#ifdef __INTERNAL_DEBUG__
    std::cout << "[DEBUG] ITS::" << __func__ << " is called." << std::endl;
#endif

    std::map < std::string, std::string > output_property;

    out->rootName = SENSOR_NAME;
    out->outputPropertyCount = 4 * OUTPUT_TRAJECTORY_SIZE + 1;      // ref_count + 1;

    std::vector< ServiceInfo > *serviceTra = m_result->getTrajectoryList(0, OUTPUT_TRAJECTORY_SIZE);
    if ( serviceTra == NULL )
    {
        printf("Error : getTrajectoryList() is failed.\n");
        return ERROR;
    }
    int serviceTraSize = serviceTra->size();

    output_property.insert(std::make_pair("name", "trackeeID"));
    output_property.insert(std::make_pair("type", "string"));
    output_property.insert(std::make_pair("value", m_result->getTrackeeID()));

    out->outputProperty.push_back(output_property);


    for ( int i = 0; i < OUTPUT_TRAJECTORY_SIZE; i++ )
    {
        std::string time_value = "null";
        std::string ref01_value = "null";
        std::string ref02_value = "null";
        std::string ref03_value = "null";

        if ( i < serviceTraSize )
        {
            MAP_TYPE::iterator itr;
            time_value = (*serviceTra)[i].timestamp;

            switch ( (*serviceTra)[i].getThingsEA() )
            {
                case 0:
                    break;
                case 1:
                    itr = (*serviceTra)[i].ThingBegin();
                    ref01_value = itr->second.ID;
                    break;
                case 2:
                    itr = (*serviceTra)[i].ThingBegin();
                    ref01_value = itr->second.ID;
                    itr++;
                    ref02_value = itr->second.ID;
                    break;
                default :
                    itr = (*serviceTra)[i].ThingBegin();
                    ref01_value = itr->second.ID;
                    itr++;
                    ref02_value = itr->second.ID;
                    itr++;
                    ref03_value = itr->second.ID;
                    break;
            }
        }

        output_property.clear();
        std::string time_name = "timeT" + intToString(i);
        output_property.insert(std::make_pair("name", time_name));
        output_property.insert(std::make_pair("type", "string"));
        output_property.insert(std::make_pair("value", time_value ));

        out->outputProperty.push_back(output_property);

        output_property.clear();
        std::string ref01_name = "ref01T" + intToString(i);
        output_property.insert(std::make_pair("name", ref01_name));
        output_property.insert(std::make_pair("type", "string"));
        output_property.insert(std::make_pair("value", ref01_value ));

        out->outputProperty.push_back(output_property);

        output_property.clear();
        std::string ref02_name = "ref02T" + intToString(i);
        output_property.insert(std::make_pair("name", ref02_name));
        output_property.insert(std::make_pair("type", "string"));
        output_property.insert(std::make_pair("value", ref02_value ));

        out->outputProperty.push_back(output_property);

        output_property.clear();
        std::string ref03_name = "ref03T" + intToString(i);
        output_property.insert(std::make_pair("name", ref03_name));
        output_property.insert(std::make_pair("type", "string"));
        output_property.insert(std::make_pair("value", ref03_value ));

        out->outputProperty.push_back(output_property);
    }

    std::cout << "Success : " << __func__ << std::endl;
    return SUCCESS;
}

