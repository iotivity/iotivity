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

#ifndef TRAJECTORY_H_
#define TRAJECTORY_H_

#include <map>
#include <string>
#include <vector>

struct TrackeeInfo
{
    public :
        std::string ID;
};

struct ThingInfo
{
    public :
        std::string ID;
        std::string ServiceType;
        int proximity;
};

typedef std::map < std::string, ThingInfo >     MAP_TYPE;

class ServiceInfo
{
    private :
        int things_ea;
        MAP_TYPE validThings;

    public :
        std::string timestamp;

        ServiceInfo( void );

        ~ServiceInfo();

        MAP_TYPE::iterator ThingBegin( void );

        MAP_TYPE::iterator ThingEnd( void );

        void insert( ThingInfo &thing );

        ThingInfo *find( std::string thingID );

        int getThingsEA( void );
};

class CurrentService
{
    public :
        TrackeeInfo trackee;
        ServiceInfo service;

        CurrentService( void );

        CurrentService( std::string trackeeID, std::string time );

        ~CurrentService();
};

class Trajectory
{
    private :
        TrackeeInfo trackee;
        int latestServiceNum;
        std::vector< ServiceInfo > ServiceList;
        std::vector< ServiceInfo > *resultList;

    public :
        Trajectory( void );

        Trajectory( std::string trackeeID );

        ~Trajectory( void );

        std::string getTrackeeID( void );

//      std::string getTrackeeServiceType( void );

        int getTrajectoryDeepSize( void );

        int TrajectoryUpdate( CurrentService &CurrentSvc );

        std::vector<ServiceInfo> *getTrajectoryList( int latestNum, int ListSize );

};



#endif /* TRAJECTORY_H_ */
