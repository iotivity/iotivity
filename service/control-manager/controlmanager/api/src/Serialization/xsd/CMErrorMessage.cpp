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



#include "CMSerialization.h"

namespace OC
{
    namespace Cm
    {
        namespace Serialization
        {
            namespace Xsd
            {

                CMErrorMessage::CMErrorMessage()
                {
                    setList(true);
                }

                CMErrorMessage::CMErrorMessage( std::string errorCode, std::string errorDescription )
                {
                    this->mErrorCode        = errorCode;
                    this->mErrorDescription = errorDescription;
                    setList(true);
                }

                CMErrorMessage::~CMErrorMessage()
                {

                }

                bool
                CMErrorMessage::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
                {
                    serializer.setStringPropertyValue("errorCode",      mErrorCode);
                    serializer.setStringPropertyValue("errorDescription", mErrorDescription);

                    return true;
                }

                bool
                CMErrorMessage::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
                {
                    if (true == deserializer.getPropertyValue("errorCode", &mErrorCode))
                    {
                        if (true == deserializer.getPropertyValue("errorDescription", &mErrorDescription))
                            return true;//11102014_PREVENT
                    }


                    return false;
                }

                std::string CMErrorMessage::getElementName()
                {
                    return "error";
                }

                bool
                CMErrorMessage::clone(CMErrorMessage *pSrcObject)
                {
                    return true;
                }





            } /* namespace Engine */
        } /* namespace Core */
    } /* namespace Cm */
} /* namespace OC */
