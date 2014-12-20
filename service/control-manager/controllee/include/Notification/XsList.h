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

#include "list"
#include "string"
#include "sstream"

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {

            class XsList
            {

                public :
                    bool getValue(std::string &value)
                    {
                        std::string retValue = "";

                        for (std::list<std::string>::const_iterator itr = values.begin();  itr != values.end() ; itr++)
                        {
                            if (retValue.size() != 0)
                            {
                                retValue.append(" ");
                            }

                            std::string token ;

                            if (toString(*itr, token))
                            {
                                retValue.append(token);
                            }
                            else
                            {
                                return false;
                            }
                        }

                        value = retValue;
                        return true ;
                    }

                    bool setValue(std::string value)
                    {
                        std::string::size_type space_index          = std::string::npos;
                        std::string::size_type prev_space_index     = std::string::npos;

                        do
                        {
                            space_index = value.find( ' ' , ( prev_space_index + 1 ) ) ;

                            std::string token ;

                            if ( space_index == std::string::npos )
                            {
                                token = value.substr( prev_space_index + 1 ) ;
                            }
                            else
                            {
                                token = value.substr( prev_space_index + 1, space_index - ( prev_space_index + 1 ) ) ;

                                prev_space_index = space_index ;
                            }

                            if ( ( token.empty() == false ) && ( token.compare(" ") != 0 ) )
                            {
                                std::string converted_token ;

                                if (fromString( token, converted_token ))
                                {
                                    values.push_back( converted_token ) ;
                                }
                                else
                                {
                                    return false;
                                }
                            }

                        }
                        while ( std::string::npos != space_index ) ;

                        return true ;
                    }

                    void addValueToken(std::string token)
                    {
                        values.push_back(token);
                    }

                    void removeValueToken(std::string token)
                    {
                        values.remove(token);
                    }

                    std::list< std::string > getValueTokens()
                    {
                        return values ;
                    }

                    bool toString( std::string input, std::string &output )
                    {
                        bool returnValue = false ;

                        std::ostringstream outputStream ;
#ifdef __APPLE__
#ifdef TARGET_OS_IPHONE
                        outputStream << input ;
                        returnValue = true;
#endif
#else
                        returnValue = ((outputStream << input) != NULL) ;
#endif
                        output = outputStream.str();

                        return returnValue ;
                    }

                    bool fromString( std::string &input, std::string &output )
                    {
                        bool returnValue = false ;

                        std::istringstream inputstream(input) ;
#ifdef __APPLE__
#ifdef TARGET_OS_IPHONE
                        inputstream >> output ;
                        returnValue = true ;
#endif
#else
                        returnValue = ((inputstream >> output) != NULL);
#endif

                        return returnValue ;
                    }

                    std::list< std::string > values;
            };

        }
    }
}
