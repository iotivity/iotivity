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


#ifndef BUNDLEINFO_H_
#define BUNDLEINFO_H_

#include <string>

using namespace std;

namespace RC{
	/*
	 * Describes a bundle with resources, that can be loaded dynamically.
	 */
	class BundleInfo {
		public:
			BundleInfo();
			virtual ~BundleInfo();
			void setName(string name);
			string getName();
			void setPath(string path);
			string getPath();
			void setVersion(string version);
			string getVersion();
			virtual int getId() = 0; // will be set by container
			static BundleInfo* createBundleInfo();
		private:
			string m_name, m_path, m_version;
	};
}

#endif /* BUNDLEINFO_H_ */
