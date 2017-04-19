/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/
#ifndef SMARTHOME_API_COMMON_RESOURCEQUERY_H_
#define SMARTHOME_API_COMMON_RESOURCEQUERY_H_

#include <sstream>
#include <CommonException.h>
#include <map>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class ResourceQuery
            {
            public:
                ResourceQuery()
                {
                }
                ~ResourceQuery()
                {
                }

                std::string getQueryValue(const std::string &key) const
                {
                    std::map< std::string, std::string >::const_iterator it;
                    it = m_query.find(key);

                    if (it == m_query.end())
                    {
                        throw CommonException("can't find key!");
                    }

                    return it->second;
                }

                void setQuery(const std::string &key, const std::string &value)
                {
                    if (0 == key.length() || 0 == value.length())
                    {
                        throw CommonException("not proper query!");
                    }

                    m_query[key] = value;
                }

                bool hasContain(const std::string &key) const
                {
                    std::map< std::string, std::string >::const_iterator it;
                    it = m_query.find(key);

                    if (it != m_query.end())
                    {
                        return true;
                    }

                    return false;
                }

                std::string getQueryAsString() const
                {
                    std::ostringstream queryStream;
                    std::map<std::string, std::string>::const_iterator it;

                    if (0 == m_query.size())
                    {
                        return std::string();
                    }

                    queryStream << '?';

                    for(it = m_query.begin(); it != m_query.end(); it++)
                    {
                        queryStream << it->first << '=' << it->second << ';';
                    }

                    std::string queryString = queryStream.str();

                    if (*(queryString.rbegin()) == ';')
                    {
                        queryString.resize(queryString.size() - 1);
                    }

                    return queryString;
                }

                int size()
                {
                    return m_query.size();
                }

            private:
                std::map< std::string, std::string > m_query;
            };
        }
    }
}

#endif /* SMARTHOME_API_COMMON_RESOURCEQUERY_H_ */
