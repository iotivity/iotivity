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
#include "Trajectory.h"

#include <stdlib.h>
#include <iostream>

// ServiceInfo Class member Function define.
ServiceInfo::ServiceInfo( void ) : timestamp(""), things_ea(0) {    validThings.clear(); }

ServiceInfo::~ServiceInfo()
{
    if ( things_ea )
    {
        validThings.clear();
        things_ea = 0;
    }
}

MAP_TYPE::iterator ServiceInfo::ThingBegin( void )
{
    return validThings.begin();
}

MAP_TYPE::iterator ServiceInfo::ThingEnd( void )
{
    return validThings.end();
}

void ServiceInfo::insert( ThingInfo &thing )
{
    if ( find(thing.ID) == NULL )
    {
        validThings[thing.ID] = thing;
        things_ea++;
    }
}

ThingInfo *ServiceInfo::find( std::string thingID )
{
    MAP_TYPE::iterator itr = validThings.find( thingID );

    if ( itr == validThings.end() )
        return NULL;

    return &itr->second;
}

int ServiceInfo::getThingsEA( void )
{
    return things_ea;
}

//
// CurrentService Class member Function define.
CurrentService::CurrentService( void )
{
    trackee.ID = "";
}

CurrentService::CurrentService( std::string trackeeID, std::string time)
{
    trackee.ID = trackeeID;
    service.timestamp = time;
}

CurrentService::~CurrentService()
{
    trackee.ID = "";
}

//
// Trajectory Class member Function define.
Trajectory::Trajectory( void )
{
    trackee.ID = "";
    ServiceList.clear();
    latestServiceNum = ServiceList.size();
    resultList = NULL;
}

Trajectory::Trajectory( std::string trackeeID )
{
    trackee.ID = trackeeID;
    ServiceList.clear();
    latestServiceNum = ServiceList.size();
    resultList = NULL;
}

Trajectory::~Trajectory( void )
{
    ServiceList.clear();
    latestServiceNum = ServiceList.size();
    if ( resultList )
    {
        delete resultList;
        resultList = NULL;
    }
}

std::string Trajectory::getTrackeeID( void )
{
    return trackee.ID;
}

int Trajectory::getTrajectoryDeepSize( void )
{
    return latestServiceNum;
}

int Trajectory::TrajectoryUpdate( CurrentService &CurrentSvc )
{
    if ( latestServiceNum <= 0 )
        goto INSERT_ELEMENT;

    if ( trackee.ID.compare( CurrentSvc.trackee.ID ) == 0 )
    {
        // if trackee.ID of CurrentSvc == trackee.ID ,
        ServiceInfo *recentSvc = &ServiceList[latestServiceNum - 1];
        ServiceInfo *crtSvc = &CurrentSvc.service;

        if ( recentSvc->getThingsEA() != crtSvc->getThingsEA() )
            goto INSERT_ELEMENT ;

        MAP_TYPE::iterator itr = crtSvc->ThingBegin();
        for ( ; itr != crtSvc->ThingEnd() ; itr++ )
        {
            if ( recentSvc->find( itr->second.ID ) == NULL )
                goto INSERT_ELEMENT;
        }

        return latestServiceNum;
    }

INSERT_ELEMENT :
    ServiceList.push_back( CurrentSvc.service );
    latestServiceNum = ServiceList.size();

    return latestServiceNum;
}

std::vector<ServiceInfo> *Trajectory::getTrajectoryList( int latestNum, int ListSize )
{
    if ( latestServiceNum <= 0 || latestServiceNum <= latestNum )
        return NULL;

    if ( resultList )
    {
        delete resultList;
        resultList = NULL;
    }
    resultList = new std::vector<ServiceInfo>;

    int endIndex = 0;
    if ( latestServiceNum >= (latestNum + ListSize) && ListSize > 0 )
    {
        endIndex = latestServiceNum - latestNum - ListSize;
    }

    for ( int i = (latestServiceNum - 1 - latestNum) ; i >= endIndex ; i-- )
        resultList->push_back( ServiceList[i] );

    return resultList;
}



