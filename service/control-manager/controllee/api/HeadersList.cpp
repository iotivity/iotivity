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

#include "Utils.h"
#include "map"
#include "HeadersList.h"

class caseInsensitiveCompare : public std::binary_function<unsigned char, unsigned char, bool>
{
    public:
        bool operator() (const unsigned char &character1, const unsigned char &character2) const
        {
            return tolower(character1) < tolower(character2);
        }
};

bool caseInsensitiveCompareLess::operator() (const std::string &string1, const std::string &string2) const
{
    return std::lexicographical_compare(string1.begin(), string1.end(), string2.begin(), string2.end(), caseInsensitiveCompare());
}

namespace OC
{
    namespace Cm
    {
        namespace Connector
        {

            /**
             * @class       HeaderListIteratorImpl
             * @brief       This class handles iterating of request header list
             *
             * @since       1.0
             *
             * This class handles iterating of request header list
             *
             */
            class HeaderListIteratorImpl : public IHeadersListIterator
            {
                    std::map<std::string, std::string, caseInsensitiveCompareLess>::const_iterator itr, itrEnd;

                public:

                    HeaderListIteratorImpl( std::map<std::string, std::string, caseInsensitiveCompareLess>& headersMap )
                    {
                        itr = headersMap.begin();
                        itrEnd = headersMap.end();
                    }

                    virtual bool isEnd()
                    {
                        return ( itr == itrEnd );
                    }

                    virtual bool moveToNext()
                    {
                        itr++ ;
                        return true ;
                    }

                    virtual bool get(std::string &name, std::string &value)
                    {
                        name = (*itr).first ;
                        value = (*itr).second ;

                        return true;
                    }
            };

            HeadersList::HeadersList(void)
            {

            }

            HeadersList::~HeadersList(void)
            {

            }


            bool
            HeadersList::setHeader(const std::string &name , const std::string &value)
            {
                if ( name.empty() )
                {
                    return false;
                }

                m_headers[name] = value ;
                return true;
            }

            bool
            HeadersList::getHeader(const std::string &name , std::string &value)
            {
                std::map<std::string, std::string, caseInsensitiveCompareLess>::iterator item = m_headers.find(name) ;
                if (item == m_headers.end())
                {
                    return false;
                }

                value = item->second;

                return true;
            }


            OC::Cm::Connector::IHeadersListIterator *
            HeadersList::getIterator()
            {
                return new HeaderListIteratorImpl( m_headers ) ;
            }

        }
    }
}
