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

#if !defined(_HEADERSLIST_H)
#define _HEADERSLIST_H

#include "string"
#include "map"

/**
 * @interface   caseInsensitiveCompareLess
 * @brief       This class implements case insensitive compare.
 * @since       1.0
 */
class caseInsensitiveCompareLess: std::binary_function < std::string, std::string,
    bool >
{
    public:
        /**
         * This is operator overloading for () operator. Used for compare two string in case insensitivity.
         *
         * @param string1   Represents string which is comparing with.
         * @param string2   Represents string which is comparing with.
         *
         * @return  tyep:bool true If both strings are equal, false If both strings are not equal
         */
        bool operator()(const std::string &string1,
                        const std::string &string2) const;
};

namespace OC
{
    namespace Cm
    {
        namespace Connector
        {

            /**
             * @interface   IHeadersListIterator
             * @brief       This interface defines methods required to iterate HTTP Headers
             * @since       1.0
             */
            class IHeadersListIterator
            {
                public:

                    /**
                     *  ~IHeadersListIterator()  - default destructor
                     */
                    virtual ~IHeadersListIterator()
                    {
                    }
                    ;

                    /**
                     * isEnd() checks whether iterator reached end position or not.
                     *
                     * @return-bool- true if iterator reaches end position, false if iterator does not reached end position
                     */
                    virtual bool isEnd() = 0;

                    /**
                     * moveToNext moves the iterator to next header.
                     *
                     * @return  type:bool true in case of success, false in case of failure
                     */
                    virtual bool moveToNext() = 0;

                    /**
                     * getg ets the details of current HTTP header
                     *
                     * @param   name    name of the header
                     * @param   value   value of the header
                     *
                     * @return  type:bool  true in case of success, false in case of failure
                     */
                    virtual bool get(std::string &name, std::string &value) = 0;
            };

            /**
             * @interface   HeadersList
             * @brief       This class holds a list of HTTP Headers
             * @since       1.0
             */
            class HeadersList
            {
                public:

                    /**
                     * Default constructor
                     */
                    HeadersList(void);

                    /**
                     * Default destructor
                     */
                    ~HeadersList(void);

                    /**
                     * setHeader adds a HTTP header and value to the list
                     *
                     * @param   name    name of the header
                     * @param   value   value of the header
                     *
                     * @return  type:bool true in case of success ,false in case of failure
                     */
                    bool setHeader(const std::string &name, const std::string &value);

                    /**
                     * getHeader gets the details of current HTTP header
                     *
                     * @param   name    name of the header
                     * @param   value   value of the header
                     *
                     * @return  type:bool true in case of success, false in case of failure
                     */
                    bool getHeader(const std::string &name, std::string &value);

                    /**
                     * getIterator returns an iterator
                     *
                     * @return type:IHeadersListIterator*   http header iterator
                     */
                    OC::Cm::Connector::IHeadersListIterator *getIterator();

                private:

                    std::map<std::string, std::string, caseInsensitiveCompareLess> m_headers;

            };

        }
    }
}

#endif // _HEADERSLIST_H
