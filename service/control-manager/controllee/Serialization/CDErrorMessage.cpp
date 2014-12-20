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

#include "ISerializer.h"
#include "IDeserializer.h"
#include "CDErrorMessage.h"
#include "log.h"

namespace OC
{
    namespace Cm
    {
        namespace Serialization
        {
            namespace Xsd
            {

                CDErrorMessage::CDErrorMessage()
                {
                    setList(true);
                }

                CDErrorMessage::CDErrorMessage(std::string errorCode,
                                               std::string errorDescription)
                {
                    this->mErrorCode = errorCode;
                    this->mErrorDescription = errorDescription;
                    setList(true);
                }

                CDErrorMessage::~CDErrorMessage()
                {

                }

                bool CDErrorMessage::serializeContents(
                    OC::Cm::Serialization::ISerializer &serializer)
                {
                    serializer.setStringPropertyValue("errorCode", mErrorCode);
                    serializer.setStringPropertyValue("errorDescription", mErrorDescription);

                    return true;
                }

                bool CDErrorMessage::deSerializeContents(
                    OC::Cm::Serialization::IDeserializer &deserializer)
                {
                    if (!deserializer.getPropertyValue("errorCode", &mErrorCode))
                    {
                        CE(cout << "Error getting error code in" << __PRETTY_FUNCTION__);
                    }
                    if (!deserializer.getPropertyValue("errorDescription", &mErrorDescription))
                    {
                        CE(cout << "Error getting error description in" << __PRETTY_FUNCTION__);
                    }

                    return true;
                }

                std::string CDErrorMessage::getElementName()
                {
                    return "error";
                }

                bool CDErrorMessage::clone(CDErrorMessage *pSrcObject)
                {
                    return true;
                }

            } /* namespace Engine */
        } /* namespace Core */
    } /* namespace Cm */
} /* namespace OC */
