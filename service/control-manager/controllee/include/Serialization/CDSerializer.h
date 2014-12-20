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

#ifndef CDSERIALIZER_H_
#define CDSERIALIZER_H_

#include <list>
#include <string>
#include "ISerializer.h"
#include <cstdint>
/** OIC
#include <json/json.h>
**/
#include "OCApi.h"

using namespace std;

namespace OC
{


    class CDSerializer : public OC::Cm::Serialization::ISerializer
    {
        public:



            CDSerializer();

            virtual ~CDSerializer();

            bool setPropertyValue(std::string propertyName, std::string value) ;

            bool setPropertyValue(std::string propertyName, bool value) ;

            //Double APIs
            bool setPropertyValue(std::string propertyName, double value) ;

            // Integer APIs
            bool setPropertyValue(std::string propertyName, int8_t value) ;

            bool setPropertyValue(std::string propertyName, int16_t value) ;

            bool setPropertyValue(std::string propertyName, int32_t value) ;

            bool setPropertyValue(std::string propertyName, int64_t value) ;

            bool setPropertyValue(std::string propertyName, uint8_t value) ;

            bool setPropertyValue(std::string propertyName, uint16_t value) ;

            bool setPropertyValue(std::string propertyName, uint32_t value) ;

            bool setPropertyValue(std::string propertyName, uint64_t value) ;

            bool setPropertyValue(std::string propertyName, float value) ;





            bool getOutput(OCRepresentation &output, int &lenth); // Changed by SRIB

            bool getPrettyPrintOutput(std::string &output);

            bool startGroupProperty(string nodeName);

            bool endGroupProperty(string nodeName);

            bool startListProperty(string nodeName);

            bool endListProperty(string nodeName);

            bool startDocument();

            bool endDocument();

            std::string getContentType();
            void setValidateMandatory(bool enable) ;
            bool isInFilteringMode(std::string filter ) ;
            void setErrorMessage(std::string errorMessage) ;
            std::string getErrorMessage() ;

            virtual bool setSchemaInstanceType(std::string type) ;

            virtual bool setFilter(std::string filter) ;

            virtual void setRecursive(bool recursive) ;

            virtual std::string getFilter() ;

            virtual bool isRecursive() ;

            virtual bool validateMandatory() ;


            virtual void setIDExcemption(bool excemption)  ;

            virtual bool getIDExcemption() ;

        private :

            list<string> m_currentNodes ;
            list<OCRepresentation *> m_values;
            OCRepresentation m_output ;
            std::string filter ;

            bool inFilteringMode ;

            bool isFilteringDone ;

            bool recursive ;

            bool bValidateMandatory ;

            int currentLevel;

            int totalLevel;

            std::string m_error_message;

            bool bIdExcemtion ;

            bool setAttributeValue(string propertyName, AttributeValue &value); // ADDED BY SRIB
            bool setStringPropertyValue(string propertyName, string value);
            bool setBoolPropertyValue(string propertyName, bool value);
            bool setIntPropertyValue(string propertyName, int value);
            bool setUIntPropertyValue(string propertyName, unsigned int value);
            bool setDoublePropertyValue(string propertyName, double value);
    };

}


#endif /* CDSERIALIZER_H_ */
