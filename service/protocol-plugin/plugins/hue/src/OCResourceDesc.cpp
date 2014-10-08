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

/// @file OCResourceDesc.cpp

/// @brief

#include "OCResourceDesc.h"


OCResourceDesc::OCResourceDesc()
{
    // TODO
}
OCResourceDesc::~OCResourceDesc()
{
    // TODO
}
OCResourceDesc::OCResourceDesc(string t_res_id, string t_type, string t_res_url, string t_res_state)
{
    resource_id = t_res_id;
    type = t_type;
    resource_url = t_res_url;
    resource_state = t_res_state;
}
void OCResourceDesc::SetResource_id(string id)
{
    this->resource_id = id;
}
void OCResourceDesc::SetType(string type)
{
    this->type = type;
}
void OCResourceDesc::SetResource_url(string url)
{
    this->resource_url = url;
}
void OCResourceDesc::SetResource_state(string state)
{
    this->resource_state = state;
}
string OCResourceDesc::GetResource_id()
{
    return this->resource_id;
};
string OCResourceDesc::GetType()
{
    return this->type;
}
string OCResourceDesc::GetResource_url()
{
    return this->resource_url;
}
string OCResourceDesc::GetResource_state()
{
    return this->resource_state;
}


//OCResourceDescManager::OCResourceDescManager(){
//  //todo
//}
//OCResourceDescManager::~OCResourceDescManager(){
//  //todo
//}
//void OCResourceDescManager::setresources(vector<OCResourceDesc>& resources){
//  this->ocResources=resources;
//};
//vector<OCResourceDesc> OCResourceDescManager::GetResources(){
//  return this->ocResources;
//}
//
//vector<OCResourceDesc>* OCResourceDescManager::InitResources(string filename){
//  vector<OCResourceDesc>* resources=new vector<OCResourceDesc>();
//  TiXmlDocument *resourcesDoc = new TiXmlDocument();
//  resourcesDoc->LoadFile(filename.c_str());
//  TiXmlElement *RootElement = resourcesDoc->RootElement();
//  TiXmlElement *Resource = RootElement->FirstChildElement();
//  while(Resource){
//      OCResourceDesc* ocResource=new OCResourceDesc();
//      ocResource->SetResource_id(Resource->Attribute("id"));
//      TiXmlElement *ResourceChild=Resource->FirstChildElement();
//      while(ResourceChild){
//          if (strcmp( ResourceChild->Value(), "type")==0){
//              ocResource->SetType(ResourceChild->FirstChild()->Value());
//          }else if (strcmp(ResourceChild->Value(), "resource_url") == 0) {
//              ocResource->SetResource_url(ResourceChild->FirstChild()->Value());
//          }else if (strcmp(ResourceChild->Value(), "resource_state") == 0) {
//              ocResource->SetResource_state(ResourceChild->FirstChild()->Value());
//          } else{
//              cout<<"error xml";
//          }
//          ResourceChild=ResourceChild->NextSiblingElement();
//      }
//      //cout<<ocResource->GetDrive_name()<<endl;
//        resources->push_back(*ocResource);
//      Resource=Resource->NextSiblingElement();
//  }
//    delete resourcesDoc;
//    return resources;
//}
//
//TiXmlElement* OCResourceDescManager::CreateResource(OCResourceDesc& resource) {
//  TiXmlElement *Resource = new TiXmlElement("resource");
//  Resource->SetAttribute("id", resource.GetResource_id().c_str());
//  TiXmlElement *type = new TiXmlElement("type");
//  TiXmlElement *url = new TiXmlElement("resource_url");
//  TiXmlElement *state = new TiXmlElement("resource_state");
//
//  TiXmlText *typeContent = new TiXmlText(resource.GetType().c_str());
//  TiXmlText *urlContent = new TiXmlText(resource.GetResource_url().c_str());
//  TiXmlText *stateContent = new TiXmlText(resource.GetResource_state().c_str());
//
//  type->LinkEndChild(typeContent);
//  url->LinkEndChild(urlContent);
//  state->LinkEndChild(stateContent);
//
//  Resource->LinkEndChild(type);
//  Resource->LinkEndChild(url);
//  Resource->LinkEndChild(state);
//  if (NULL != Resource) {
//      return Resource;
//  } else {
//      return NULL;
//  }
//  return NULL;
//}
//
//void OCResourceDescManager::CreateResourcesXMl(vector<OCResourceDesc>& resources,string filename){
//  TiXmlDocument *resourcesDoc = new TiXmlDocument();
//  resourcesDoc->LoadFile();
//  TiXmlDeclaration *pDeclaration = new TiXmlDeclaration(("1.0"), (""),(""));
//  TiXmlElement* resourcesElement = new TiXmlElement("resources");
//  vector<OCResourceDesc>::iterator iter;
//  iter = resources.begin();
//        TiXmlElement* element;
//  while (iter != resources.end()) {
//      element=this->CreateResource(*iter);
//      //cout<<element->Attribute("id");
//      resourcesElement->LinkEndChild(element);
//      *iter++;
//  }
//  resourcesDoc->LinkEndChild(pDeclaration);
//  resourcesDoc->LinkEndChild(resourcesElement);
//  resourcesDoc->SaveFile(filename.c_str());
//        delete element;
//        delete pDeclaration;
//}
