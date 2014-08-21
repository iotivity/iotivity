//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace boost::property_tree;

void parseResource(ptree resource)
{
	cout<<"\t\tParsing Resource with "<< resource.size() <<" facts:"<<endl;
	cout << "\t\tAddress is "<< resource.get<std::string>("href", "") <<endl;
	cout << "\t\tIs Observable? : "<< (resource.get<int>("obs",0)==1)<<endl;
	
	// types:
	cout << "\t\tResource Type Count : "<<resource.get_child("rt", ptree()).size()<<endl;
	for(auto resourceItr : resource.get_child("rt", ptree()))
	{
		cout << "\t\t\tResource Type: "<< resourceItr.second.data()<<endl;
	}
	
	// interfaces:
	cout << "\t\tInterface Count : "<<resource.get_child("if", ptree()).size()<<endl;
	for(auto resourceItr : resource.get_child("if", ptree()))
	{
		cout << "\t\t\tInterface: "<< resourceItr.second.data()<<endl;
	}
	
	cout<<endl;
}

void parseTest(const char* data)
{
	
	std::stringstream requestStream;
	requestStream << data;
		
	ptree root;
	boost::property_tree::read_json(requestStream, root);
	
	ptree payload = root.get_child("oc.payload", ptree());
	cout << "\tResource Count:" << payload.size()<<endl;
	
	for(auto itr : payload)
	{
		parseResource(itr.second);
	}
	
//	cout << "PTree Size: " << requestTree.size() << " DATA: "<<requestTree.get_value<string>() <<endl;
	//cout << "\t Child:"<<requestTree.front().first.data()<<"  " << requestTree.front().second.data()<<endl;
}


int main()
{
	const char* data1 = "{\"oc\": {\
    \"payload\": [{\
            \"href\":\"/a/tube\",\
            \"rt\":[\"alpha.act.tb\"],\
            \"if\":[\"oc.mi.a\"]\
                },\
                {\
            \"href\":\"/b/tube\",\
            \"rt\":[\"alpha.act.tb\"],\
            \"if\":[\"oc.mi.a\"],\
            \"obs\" : 1\
                }\
               ]\
        }\
}";

	const char* data2 = "{\"oc\": {\
    \"payload\": [{\
            \"href\":\"/temp-sensor\",\
            \"rt\":[\"beta.sensor.temp\"],\
            \"if\":[\"oc.mi.s\"],\
            \"obs\" : 1\
                }\
               ]\
        }\
} ";

	const char* data3 = "{\"oc\": {\
    \"payload\": [{\
            \"href\":\"/room-lights\",\
            \"rt\":[\"gamma.lights\"],\
            \"if\":[\"oc.mi.ll\", \"oc.mi.b\"]\
                }\
               ]\
        }\
} ";

	cout<<"Data1:\n";
	parseTest(data1);
	cout<<"Data2:\n";
	parseTest(data2);
	cout<<"Data3:\n";
	parseTest(data3);

	return 0;
}