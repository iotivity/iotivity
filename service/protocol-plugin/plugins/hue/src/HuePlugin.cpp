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

/// @file HuePlugin.cpp

/// @brief


#include <typeinfo>
#include "curl/curl.h"
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include "HueBridge.h"

#include "HuePlugin.h"
#include "json_parse.h"
#include "simple_parse.h"


using namespace std;
using namespace boost::property_tree;
using namespace boost;

using namespace OIC;


/**
 * Response Data for curl handler to hue bridge
 * @return Return the number of response data
 *
 */
static size_t OnWriteData(void *buffer, size_t size, size_t nmemb, void *lpVoid)
{
    std::string *str = dynamic_cast<std::string *>((std::string *)lpVoid);
    if ( NULL == str || NULL == buffer )
    {
        return -1;
    }
    char *pData = (char *)buffer;
    str->append(pData, size * nmemb);
    return nmemb;
}


HuePlugin::HuePlugin() {cout << "HuePlugin::HuePlugin()" << endl;}
HuePlugin::~HuePlugin() { cout << "HuePlugin::~HuePlugin()" << endl;}


/**
 *  convert received json object to our defined resource info
 *
 */
void HuePlugin::ConvertOcRes(string &hueRes, map<string, string> &dictmap)
{
    stringstream stream;
    ptree pt;
    cout << "ConvertOcRes --" << hueRes.c_str() << endl;
    if (hueRes.size())
    {

        replace_all(hueRes, "u\'", "\"");
        replace_all(hueRes, "\'", "\"");

        stream << hueRes;
        read_json<ptree>(stream, pt);
        string lig_id, lig_name;
        for (ptree::iterator it = pt.begin(); it != pt.end(); ++it)
        {
            lig_id = it->first;
            lig_name = it->second.get<string>("name"); //first为空
            dictmap.insert(make_pair(lig_id, lig_name));
        }
    }
}


/**
 *  Found the changed hue resources
 *
 *  @param oldR  the vector for the last search result
 *  @param newR  the vector for the current search result
 *  @param comm  the vector for both oldR and newR contain
 *  @return the map for changed resrouces
 */

map<string, string> HuePlugin::ResourcesChange(vector<string> &oldR, vector<string> &newR,
        vector<string> &comm)
{
    vector<string>tempR;
    vector<string>::iterator iter = oldR.begin();
    vector<string>::iterator tempiter;
    while (iter != oldR.end())
    {
        tempiter = find(newR.begin(), newR.end(), iter->c_str());
        if (tempiter != newR.end())
        {
            comm.push_back(tempiter->c_str());
        }
        iter++;
    }
    iter = comm.begin();
    while (iter != comm.end())
    {
        tempiter = find(oldR.begin(), oldR.end(), iter->c_str());
        if (tempiter != oldR.end())
        {
            oldR.erase(tempiter);
        }
        tempiter = find(newR.begin(), newR.end(), iter->c_str());
        if (tempiter != newR.end())
        {
            newR.erase(tempiter);
        }
        iter++;
    }
    map<string, string>res_changed;                              // return the changed resource info
    for (iter = oldR.begin(); iter != oldR.end(); iter++)
        res_changed.insert(make_pair(*iter, "leave"));
    for (iter = newR.begin(); iter != newR.end(); iter++)
        res_changed.insert(make_pair(*iter, "join"));

    return res_changed;
}


/**
 *  create a local xml file to mark the resources information
 *
 */

//void HuePlugin::CreateResourcesFile(vector<OCResourceDesc>& resources, string& template_file)
//{
//      OCResourceDescManager* pi = new OCResourceDescManager();
//      if(resources.size())
//          pi->CreateResourcesXMl(resources, template_file);
//      cout<<"After create xml file"<<endl;
//      delete pi;
//}
//

/**
 * Search Resources by philips hue protocol.
 *
 * @return Return a vector, which members are ResourceDesc type, a inner defined structure
 *
 */

vector<OCResourceDesc> HuePlugin::SearchDeviceResources(vector<string> &pool)
{
    cout << "HuePlugin::SearchDeviceResources()" << endl;

    res_pool = pool;


    vector<string>dev_url;
    vector<string>new_res_pool;

    vector<OCResourceDesc> change_resources;
    vector<OCResourceDesc> template_resources;

    vector<string> bridge_list;
    bridge_list = GetBridges();
    if (bridge_list.empty())
    {
        cout << "No hue bridge found" << endl;
        return change_resources;
    }
    vector<string>::iterator it;
    cout << "the result: " << endl;
    for (it = bridge_list.begin(); it != bridge_list.end(); it++)
    {
        cout << *it << "," << endl;
        string hue_bridge_url;
        hue_bridge_url.append("http://").append(*it).append("/api/newdeveloper/lights");
        cout << "total url ---" << hue_bridge_url << endl;
        dev_url.push_back(hue_bridge_url);
    }

    vector<string>::iterator url_iter;
    vector<pthread_t> tids;
    for (url_iter = dev_url.begin(); url_iter != dev_url.end(); url_iter++)
    {
        string strUrl = *url_iter;
        string strResponse;
        cout << "ready to get_all_lights: " << strUrl << endl;

        CURLcode res;
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        cout << "ready to curl" << endl;
        CURL *curl = curl_easy_init();
        if (NULL == curl)
        {
            cout << "error" << endl;
        }

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        cout << "response " << strResponse.c_str() << endl;

        map<string, string> dictmap;
        HuePlugin::ConvertOcRes((string &) strResponse, dictmap);
        map<string, string>::iterator it;
        for (it = dictmap.begin(); it != dictmap.end(); it++)
        {
            string res_url = strUrl;
            res_url.append("/").append(it->first).append("/state");
            new_res_pool.push_back(res_url);
        }

    }

    vector<string> res_comm_pool;

    pool = new_res_pool;
    map<string, string> res_changed = ResourcesChange(res_pool, new_res_pool, res_comm_pool);

    for (map<string, string>::iterator m_it = res_changed.begin(); m_it != res_changed.end(); m_it++)
    {
        string resUrl = m_it->first;
        cout << "Changed resource: " << resUrl << endl;
        size_t l_pos, r_pos;
        l_pos = resUrl.find("//");
        r_pos = resUrl.find("/", l_pos + 2);
        string br_ip = resUrl.substr(l_pos + 2, r_pos - l_pos - 2);
        size_t index = resUrl.find_last_of("/");
        string add_res = resUrl.substr(index);
        string res_id = br_ip.append(add_res);

        string res_state = m_it->second;
        string res_type = "light";

        OCResourceDesc r(res_id, res_type, resUrl, res_state);
        change_resources.push_back(r);
        template_resources.push_back(r);
    }

    for (vector<string>::iterator iter = res_comm_pool.begin(); iter != res_comm_pool.end(); iter++)
    {
        string resUrl = *iter;
        size_t l_pos, r_pos;
        l_pos = resUrl.find("//");
        r_pos = resUrl.find("/", l_pos + 2);
        string br_ip = resUrl.substr(l_pos + 2, r_pos - l_pos - 2);
        size_t index = resUrl.find_last_of("/");
        string add_res = resUrl.substr(index);
        string res_id = br_ip.append(add_res);

        string res_state = "keep";
        string res_type = "light";

        OCResourceDesc r(res_id, res_type, resUrl, res_state);
        template_resources.push_back(r);
    }

//    cout << "ready to create Xml file"<<endl;
//    size_t rpos = real_conf_file.find_last_of("/");
//    string abs_path = real_conf_file.substr(0,rpos+1);
//    string template_file = abs_path.append("test.xml");
//    CreateResourcesFile(template_resources,template_file);

//    cout << "Create Xml file OK "<<endl;
    return change_resources;
}


/**
 * Read config file to fetch the Hue bridge search info
 *
 * @return Return a vector, which members are hue url type
 *
 */

//vector<string> HuePlugin::ReadConfig(const char* filepath)
//{
//    string filename(filepath);
//    vector<string> devUrl;
//
//    if(filename.find(".json") != string::npos){
//         cout<<"json file !"<<endl;
//         ptree pt, p1, p2;
//         string hue_ip,hue_user, hue_res, hue_ext;
//
//         read_json<ptree>(filename, pt);
//         p1 = pt.get_child("bridges");
//         hue_res = pt.get<string>("resource");
//         hue_ext = pt.get<string>("extra");
//
//         for (ptree::iterator it = p1.begin(); it != p1.end(); ++it)
//         {
//             p2 = it->second; //first为空
//             hue_ip = p2.get<string>("ip");
//             hue_user = p2.get<string>("username");
//
//             string hue_url;
//
//             hue_url.append("http://");
//             hue_url.append(hue_ip);
////             hue_url.append(":8080/");
//             hue_url.append("/");
//             hue_url.append(hue_ext);
//             hue_url.append("/");
//             hue_url.append(hue_user);
//             hue_url.append("/");
//             hue_url.append(hue_res);
//             cout<<"Hue Url --- "<<hue_url<<endl;
//             devUrl.push_back(hue_url);
//         }
//    }else if(filename.find(".ini") != string::npos){
//         cout<<"ini file !"<<endl;
//         cout<<"filepath = "<<filepath<<endl;
//         string ip_lst, uname_lst, res, ext;
//
//         string key="ip";
//         readConfigFile(filepath,(const string&)key, (string&)ip_lst);
//         vector<string> ip_vc;
//         ip_vc = NormalizedOut((string&)ip_lst);
//
//         key="username";
//         readConfigFile(filepath,(const string&)key, (string&)uname_lst);
//         vector<string> uname_vc;
//         uname_vc = NormalizedOut((string&)uname_lst);
//
//         vector<string>::iterator iter1, iter2;
//         map<string, string> dictmap;
//         for(iter1 = ip_vc.begin(), iter2 = uname_vc.begin(); iter1 != ip_vc.end(), iter2 != uname_vc.end(); iter1++,iter2++)
//         {
////             dictmap.insert(make_pair(*iter1, *iter2));
//             dictmap.insert(make_pair(*iter2, *iter1));
//         }
//
//         key = "extra";
//         readConfigFile(filepath,(const string&)key, (string&)ext);
//
//         key = "resource";
//         readConfigFile(filepath,(const string&)key, (string&)res);
//
//         map<string, string>::iterator it;
//         for (it=dictmap.begin();it!=dictmap.end();it++)
//         {
//             string hue_url;
//
//             hue_url.append("http://");
////             hue_url.append(it->first);
//             hue_url.append(it->second);
////             hue_url.append(":8080/");
//             hue_url.append("/");
//             hue_url.append(ext);
//             hue_url.append("/");
////             hue_url.append(it->second);
//             hue_url.append(it->first);
//             hue_url.append("/");
//             hue_url.append(res);
//             cout<<"Hue Url --- "<<hue_url<<endl;
//             devUrl.push_back(hue_url);
//         }
//    }
//    return devUrl;
//}


