//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#ifndef CACHETYPES_H
#define CACHETYPES_H

#include <functional>
#include <map>
#include <memory>
#include <string>

#include "ocstack.h"
#include "OCResource.h"

#include "../include/PrimitiveResource.h"

//class ServiceResource;

typedef PrimitiveResource ServiceResource;

enum class REPORT_FREQUENCY
{
    NONE = 0,
    UPTODATE,
    PERIODICTY
};

struct Report_Info
{
    REPORT_FREQUENCY rf;
    int reportID;
    long latestReportTime;
    long repeatTime;
};

enum class CACHE_STATE
{
    READY = 0,
    READY_YET,
    LOST_SIGNAL,
    DESTROYED,
    UPDATING
};

typedef int CacheID;

typedef std::map<std::string, std::string> CacheData;
typedef std::function<OCStackResult(std::shared_ptr<ServiceResource>, CacheData)> CacheCB;
typedef std::map<Report_Info, CacheCB> SubscriberInfo;
typedef std::pair<Report_Info, CacheCB> SubscriberInfoPair;

typedef OC::OCResource BaseResource;
//typedef std::function<void(const OC::HeaderOptions&,
//            const OC::OCRepresentation&, const int, const int)> ObserveCB;
//typedef std::function<void(const OC::HeaderOptions&,
//            const OC::OCRepresentation&, const int)> GetCB;

typedef PrimitiveResource::GetCallback GetCB;
typedef PrimitiveResource::ObserveCallback ObserveCB;

#endif
