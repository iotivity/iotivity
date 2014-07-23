//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

#include "OCResource.h"
#include "OCReflect.h"

namespace OC {
	OCResource::OCResource(std::string host, boost::property_tree::ptree& resourceNode) : m_isCollection(false)
	{
		m_host = host;
		m_uri = resourceNode.get<std::string>("href","");
		m_isObservable = resourceNode.get<int>("obs",0)==1;
		
		boost::property_tree::ptree resourceTypes = resourceNode.get_child("rt", boost::property_tree::ptree());
		for(auto itr : resourceTypes)
		{
			m_resourceTypes.push_back(itr.second.data());
		}
		
		boost::property_tree::ptree interfaces = resourceNode.get_child("if", boost::property_tree::ptree());
		for(auto itr : interfaces)
		{
			if(itr.second.data() == "oc.mi.ll")
			{
				m_isCollection = true;
			}
		
			m_interfaces.push_back(itr.second.data());
		}
		
		// TODO: If collection, load children, assuming this becomes a thing
		
		// TODO: Load attributes, assuming this becomes a 'thing'
		
		if (m_uri.empty() || resourceTypes.empty() || interfaces.empty())
		{
			throw ResourceInitException(m_uri.empty(), resourceTypes.empty(), interfaces.empty());
		}
	}
	
	OCResource::~OCResource()
	{
	}
} // namespace OC
