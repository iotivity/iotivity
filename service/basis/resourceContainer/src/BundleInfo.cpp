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

#include "BundleInfo.h"
#include "BundleInfoInternal.h"

namespace RC{
	BundleInfo::BundleInfo(){

	}

	BundleInfo::~BundleInfo(){

	}

	void BundleInfo::setName(string name){
		m_name = name;
	}

	string BundleInfo::getName(){
		return m_name;
	}

	void BundleInfo::setPath(string path){
		m_path = path;
	}
	string BundleInfo::getPath(){
		return m_path;
	}

	void BundleInfo::setVersion(string version){
		m_version = version;
	}

	string BundleInfo::getVersion(){
		return m_version;
	}

	BundleInfo* BundleInfo::createBundleInfo(){
		BundleInfoInternal* newBundleInfo = new BundleInfoInternal();
		return newBundleInfo;
	}
}
