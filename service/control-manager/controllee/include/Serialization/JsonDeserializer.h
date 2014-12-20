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

#ifndef JSONDESERIALIZER_H_
#define JSONDESERIALIZER_H_

#include <list>
#include <string>
#include <map>
#include <json/json.h>
#include "IDeserializer.h"

using namespace std;


namespace OC
{
    namespace Cm
    {
        namespace Core
        {
            namespace Serialization
            {
                namespace Json
                {

                    /**
                     *
                     */
                    class JsonDeserializer : public OC::Cm::Serialization::IDeserializer
                    {

                        public:

                            JsonDeserializer(IContext *context);

                            virtual ~JsonDeserializer();

                            virtual bool parse(std::string &buffer, int length);


                            bool getIsArray(string propertyName);

                            int getSize(string propertyName);

                            bool setCurrentArrayIndex( string propertyName, int index ) ;

                            bool getGroupProperty(string nodeName);

                            bool endGroupProperty( string propertyName );


                            bool hasProperty(string propertyName) ;

                            bool hasGroupProperty(string nodeName);

                            bool getArrayPropertyValue(std::string arrayName, std::string propertyName, int index, std::string &value) ;

                            bool getArrayPropertyValue(std::string arrayName, std::string propertyName, int index, bool &value);

                            bool getPropertyValue(std::string propertyName, double *value) ;

                            //bool getPropertyValue(std::string propertyName, int* value) ;

                            bool getPropertyValue(std::string propertyName, std::string *value) ;

                            bool getPropertyValue(std::string propertyName, bool *value) ;


                            virtual bool getSchemaInstanceType(std::string &type) ;

                            bool setFilter(std::string filter) ;

                            bool isInFilteringMode(std::string filter) ;


                            bool getPropertyValue(std::string propertyName, uint8_t *value) ;

                            bool getPropertyValue(std::string propertyName, uint16_t *value) ;

                            bool getPropertyValue(std::string propertyName, uint32_t *value) ;

                            bool getPropertyValue(std::string propertyName, uint64_t *value) ;

                            bool getPropertyValue(std::string propertyName, int8_t *value) ;

                            bool getPropertyValue(std::string propertyName, int16_t *value) ;

                            bool getPropertyValue(std::string propertyName, int32_t *value) ;

                            bool getPropertyValue(std::string propertyName, int64_t *value) ;

                            bool getPropertyValue(std::string propertyName, float *value) ;

                            virtual bool isRecursive() ;

                            virtual void setRecursive(bool recursive) ;

                            void setErrorMessage(std::string errorMessage) ;

                            std::string getErrorMessage() ;

                            virtual bool validateMandatory() ;

                            void setValidateMandatory(bool enable) ;

                            virtual void setIDExcemption(bool excemption)  ;

                            virtual bool getIDExcemption() ;

                            virtual bool removeParameter(std::string paramName) ;

                            ::Json::Value *getProperty(string propertyName);

                            ::Json::Value getPropertyRemoveAfter(string propertyName);

                            void setHTTPMethod(std::string method) ;

                            OC::Cm::Serialization::ISerializable *createElement(std::string elementName) ;

                            void setNotificationException(bool notificationException);

                        private :

                            bool notification;

                            ::Json::Value root;

                            ::Json::Value *m_pCurrent_property ;

                            ::Json::Value m_null_value ;

                            std::string m_current_property_name;

                            list< string > m_currentNodeNames;

                            list< ::Json::Value * > m_currentNodes;

                            std::map< string, int > m_array_index_map ;

                            std::string m_filter ;

                            std::string m_error_message;

                            std::string method ;

                            bool inFilteringMode ;

                            bool isFilteringDone ;

                            bool recursive ;

                            bool bValidateMandatory ;

                            bool bIdExcemtion ;

                            bool getStringPropertyValue(string propertyName, string &propValue);

                            bool getBoolPropertyValue(string propertyName);

                            bool getIntPropertyValue(string propertyName, int64_t &propValue);

                            bool getUIntPropertyValue(string propertyName, uint64_t &propValue);

                            bool getDoublePropertyValue(string propertyName, double &propValue);

                            bool getCurrentArrayIndex( string propertyName, int &index ) ;
                    };



                }
            }
        }
    }
}

#endif /* JSONDESERIALIZER_H_ */
