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



#include "CMServerResourceHandler.h"
#include "CMDataTypes.h"
#include "Utils.h"


namespace OC
{
    namespace Cm
    {
        namespace Server
        {
            namespace ResourceHandler
            {

                bool
                ResourceHandlerFactory::registerResourceHandler(int resourceType, std::string path)
                {
                    if (path.empty())
                    {
                        return false ;
                    }

                    resourceTypeMap[resourceType] = getStringTokens(path, '/') ;

                    return true;
                }

                void
                ResourceHandlerFactory::unRegisterResourceHandler( int resourceType )
                {
                    resourceTypeMap.erase(resourceType);
                }

                void
                ResourceHandlerFactory::setContext(OC::Cm::IContext *context)
                {
                    this->p_context = context ;
                }

                int
                ResourceHandlerFactory::getResourceType( std::string path,
                        std::map<std::string, std::string> &templateParams )
                {
                    if (path.empty())
                    {
                        return -1;
                    }

                    std::list<std::string> pathTokens = getStringTokens(path, '/') ;

                    for (std::map< int, std::list< std::string > >::const_iterator iter = resourceTypeMap.begin();
                         iter != resourceTypeMap.end(); iter++)
                    {
                        std::list<std::string> registeredTokens = iter->second ;

                        if (pathTokens.size() == registeredTokens.size())
                        {
                            std::list<std::string>::iterator pathtorkenIterator = pathTokens.begin() ;
                            std::list<std::string>::iterator registeredTokenIterator = registeredTokens.begin() ;

                            for (; ((pathtorkenIterator != pathTokens.end())
                                    && (registeredTokenIterator != registeredTokens.end()));
                                 pathtorkenIterator++, registeredTokenIterator++ )
                            {
                                std::string rToken = *(registeredTokenIterator) ;
                                std::string pToken = *(pathtorkenIterator) ;

                                if (rToken.empty() == false)
                                {
                                    if ((rToken.at(0) == '{') && (rToken.at(rToken.length() - 1) == '}'))
                                    {
                                        // template parameter.. no need to compare
                                        templateParams[rToken.substr(1, rToken.length() - 2)] = pToken ;
                                    }
                                    else
                                    {
                                        // TODO: Allow case insensitive search check ?
                                        if (0 != shp_strcasecmp(rToken.c_str(), pToken.c_str()))
                                            //if ( rToken.compare( pToken ) != 0 )
                                        {
                                            break ;
                                        }
                                    }
                                }
                            }

                            if (pathtorkenIterator == pathTokens.end())
                            {
                                // found exact match
                                return iter->first;
                            }
                        }
                    }

                    return -1;
                }



            }
        }
    }
}
