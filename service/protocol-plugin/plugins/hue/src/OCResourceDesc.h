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

/// @file OCResourceDesc.h

/// @brief

#ifndef __OCRESOURCEDESC_H__
#define __OCRESOURCEDESC_H__
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

class OCResourceDesc
{
    private:
        string resource_id, type, resource_url, resource_state;

    public:
        OCResourceDesc();
        virtual ~OCResourceDesc();
        OCResourceDesc(string, string, string, string);
        void SetResource_id(string);
        void SetType(string);
        void SetResource_url(string);
        void SetResource_state(string);
        string GetResource_id();
        string GetType();
        string GetResource_url();
        string GetResource_state();
};

//class OCResourceDescManager{
//private:
//  vector<OCResourceDesc> ocResources;
//private:
//  TiXmlElement* CreateResource(OCResourceDesc&);
//public:
//
//  void setresources(vector<OCResourceDesc>&);
//  vector<OCResourceDesc> GetResources();
//    OCResourceDescManager();
//
//    vector<OCResourceDesc>* InitResources(string);
//
//    void CreateResourcesXMl(vector<OCResourceDesc>&,string);
//
//
//    ~OCResourceDescManager();
//};

#endif //__OCRESOURCEDESC_H__
