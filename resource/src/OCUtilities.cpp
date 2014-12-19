//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

#include <OCApi.h>

#include "OCUtilities.h"

#include <boost/algorithm/string.hpp>

#include <sstream>
#include <iterator>
#include <algorithm>

extern "C" {
#include <uri.h>    // libcoap
#include <option.h> // libcoap
}

namespace OC {

    // Helper function to escape special character.
    std::string escapeString(const std::string& value)
    {
        std::ostringstream stringStream;
        for (const char& c : value)
        {
            switch (c)
            {
                case '\\': stringStream << "\\\\";
                    break;
                case '"': stringStream << "\\\"";
                    break;
                case '/': stringStream << "\\/";
                    break;
                case '\b': stringStream << "\\b";
                    break;
                case '\f': stringStream << "\\f";
                    break;
                case '\n': stringStream << "\\n";
                    break;
                case '\r': stringStream << "\\r";
                    break;
                case '\t': stringStream << "\\t";
                    break;
                default: stringStream << c;
                    break;
           }
        }
        return stringStream.str();
    }
}
// [TODO] remove this function
// it seems that the C stack is parsing and giving out the query separately.
// the entire URI need not be parsed
static OC::Utilities::QueryParamsKeyVal tempPatch(const std::string& _uri)
{
    OC::Utilities::QueryParamsKeyVal qp;
    if(_uri.empty())
    {
        return qp;
    }

    std::vector<std::string> queryparams;
    boost::split(queryparams, _uri, boost::is_any_of("&"));

    for(std::string& it: queryparams)
    {
        std::vector<std::string> keyval;
        boost::split(keyval, it, boost::is_any_of("="));
        if(2 == keyval.size())
        {
            qp[keyval.at(0)] = keyval.at(1);
        }
    }

    return qp;
}

// implementation can be split into two functions if needed
// uses do{}while(0) to avoid returning from multiple locations
OC::Utilities::QueryParamsKeyVal OC::Utilities::getQueryParams(const std::string& _uri)
{

    // this is a temporary fix. [TODO] remove this after permanent fix
    return tempPatch(_uri);

    OC::Utilities::QueryParamsKeyVal qp;
    unsigned char *bufptr = nullptr; // don't delete via bufptr
    unsigned char *bufptrToDelete = nullptr; // bufptr may be incremented. need this one to keep track.
    do // while(0)
    {
        if(_uri.empty())
        {
            break;
        }

        coap_uri_t coapuri = {{0}};
        unsigned char* uristr = reinterpret_cast<unsigned char*>(const_cast<char*>(_uri.c_str()));

        if(coap_split_uri(uristr, _uri.length(), &coapuri) < 0)
        {
            break;
        }

        size_t buflen = 2048; // this is big enough buffer. [TODO] may want to downsize it. I have seen that the size may have to be greater than coap.query.length, which is counterintuitve but there may be a bug in coap uri parser.
        bufptrToDelete = bufptr = new (std::nothrow) unsigned char[buflen](); // why heap? will need it for incrementing the pointer in the logic below

        if(!bufptr)
        {
            break;
        }

        int segments = -1;
        if((segments = coap_split_query(coapuri.query.s, coapuri.query.length, bufptr, &buflen)) < 0)
        {
            break;
        }

        // coap uri parser has weird api. its not straighforward to understand what the coap function calls below do.
        // coap uri parser lacks ability to split the key value pair in query params. that will be done in getQueryParams() function
        std::vector<std::string> queryparams;
        while(segments--)
        {
            queryparams.push_back(std::string (reinterpret_cast<char*>(coap_opt_value(bufptr)), coap_opt_length(bufptr)));
            bufptr += coap_opt_size(bufptr);
        }

        if(queryparams.empty())
        {
            break;
        }

        //[TODO] use foreach
        for(std::string& it : queryparams)
        {
            std::vector<std::string> keyval;
            boost::split(keyval, it, boost::is_any_of("="));
            if(2 == keyval.size())
            {
                qp[keyval.at(0)] = keyval.at(1);
            }
        }
    }
    while(0);

    if(bufptrToDelete)
    {
        delete [] bufptrToDelete;
    }
    return qp;
}

namespace OC {

OCStackResult result_guard(const OCStackResult r)
{
 std::ostringstream os;

 switch(r)
 {
    default:
        os << "result_guard(): unhandled exception: " << OCException::reason(r);
        throw OCException(os.str(), r);

    /* Exceptional conditions: */
    case OC_STACK_NO_MEMORY:
    case OC_STACK_COMM_ERROR:
    case OC_STACK_NOTIMPL:
    case OC_STACK_INVALID_URI:
    case OC_STACK_INVALID_QUERY:
    case OC_STACK_INVALID_IP:
    case OC_STACK_INVALID_PORT:
    case OC_STACK_INVALID_CALLBACK:
    case OC_STACK_INVALID_METHOD:
    case OC_STACK_INVALID_PARAM:
    case OC_STACK_INVALID_OBSERVE_PARAM:
        os << "result_guard(): " << r << ": " << OCException::reason(r);
        throw OCException(os.str(), r);

    /* Non-exceptional failures or success: */
    case OC_STACK_OK:
    case OC_STACK_NO_RESOURCE:
    case OC_STACK_RESOURCE_ERROR:
    case OC_STACK_SLOW_RESOURCE:
    case OC_STACK_NO_OBSERVERS:
    case OC_STACK_OBSERVER_NOT_FOUND:
#ifdef WITH_PRESENCE
    case OC_STACK_PRESENCE_STOPPED:
    case OC_STACK_PRESENCE_TIMEOUT:
    case OC_STACK_PRESENCE_DO_NOT_HANDLE:
#endif

    break;
 }

 return r;
}

} // namespace OC
