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

#include <string>
#include <list>
#include "DeviceDomain.h"
#include "NetUtils.h"
#include "Utils.h"

using namespace std;

std::string trimString(std::string input)
{
    int start_index, end_index ;
    const char *inputCString = input.c_str() ;
    std::string retValue = "";

    for (start_index = 0; start_index < (int)input.length(); start_index++)
    {
        if (isspace(inputCString[start_index]) == false)
        {
            break ;
        }
    }

    for (end_index = (int)input.length() - 1; end_index >= 0; end_index--)
    {
        if (isspace(inputCString[end_index]) == false)
        {
            break ;
        }
    }

    if ((start_index == (int)input.length()) || (end_index < 0))
    {
        return retValue ;
    }
    else
    {
        return input.substr(start_index, end_index - start_index + 1) ;
    }
}

std::list<std::string> getStringTokens( std::string str, char delimiter)
{
    std::string::size_type space_index          = -1;
    std::string::size_type prev_space_index     = -1;
    std::list<std::string> tokens ;
    std::string delimiterStr("");
    delimiterStr = delimiterStr + delimiter ;

    do
    {
        space_index = str.find( delimiter , ( prev_space_index + 1 ) ) ;

        std::string token ;

        if ( (int)space_index == -1 )
        {
            token = str.substr( prev_space_index + 1 ) ;
        }
        else
        {
            token = str.substr( prev_space_index + 1, space_index - ( prev_space_index + 1 ) ) ;

            prev_space_index = space_index ;
        }

        if ( ( token.empty() == false ) && ( token.compare(delimiterStr) != 0 ) )
        {
            tokens.push_back( token ) ;
        }

    }
    while ( -1 != (int)space_index ) ;


    return tokens ;
}

bool parseUrl( const std::string &url, std::string &protocol, std::string &host, std::string &port, std::string &path, std::string &host_header_value )
{
    size_t domainstart = 0;
    size_t n_pos = url.find("http://");

    if (std::string::npos == n_pos)
    {
        n_pos = url.find("https://");

        if (std::string::npos == n_pos)
        {
            n_pos = url.find("coap://");
            if (std::string::npos == n_pos)
            {
                return false;
            }

            else
            {
                domainstart = n_pos + 7 ;
                protocol = "coap";
            }

        }
        else
        {
            domainstart = n_pos + 8 ;
            protocol = "https";
        }
    }
    else
    {
        domainstart = n_pos + 7 ;
        protocol = "http";
    }

    std::string domain_port_str ;
    n_pos = url.find("/", domainstart);
    if (std::string::npos == n_pos)
    {
        //path not found...
        domain_port_str = url.substr(domainstart);
        path.assign("/");

    }
    else
    {
        //path found...
        domain_port_str = url.substr(domainstart, n_pos - domainstart);
        path.assign(url.substr(n_pos));
    }

    host_header_value.assign(domain_port_str) ;

    //trim ?..
    n_pos = domain_port_str.find(":", domainstart);
    if (std::string::npos == n_pos)
    {
        //port not found...
        host.assign(domain_port_str);
        port.assign("80");
    }
    else
    {
        host.append(domain_port_str.substr(0, n_pos));
        port.assign(domain_port_str.substr(n_pos + 1));
    }

    return true ;
}

bool isValidUri(std::string url)
{
    size_t n_pos = url.find("http://");

    if (std::string::npos != n_pos)
    {
        return true ;
    }

    n_pos = url.find("https://");

    if (std::string::npos != n_pos)
    {
        return true;
    }

    return false ;
}

bool isValidAddress( OC::Cm::DeviceDomain deviceDomain, std::string deviceAddress )
{
    bool bRet = false;

    if (deviceAddress.empty() == true) { return false ; }

    if ( deviceDomain == OC::Cm::DeviceDomain_Local )
    {
        // DeviceAddress for Local Domain contains ip and port(ip:port)
        std::string ipAdress ;
        std::string port ;

        std::string::size_type n_pos = deviceAddress.find(":");
        if (std::string::npos == n_pos)
        {
            //port not found...
            ipAdress.assign(deviceAddress);
            port.assign("443");
        }
        else
        {
            ipAdress = deviceAddress.substr(0, n_pos);
            port.assign(deviceAddress.substr(n_pos + 1));
        }

        bRet = isValidIPAddress(ipAdress) ;
        if (!bRet) return bRet;

        bRet = isValidPort(port);
        if (!bRet) return bRet;
    }
    else
    {
        bRet = true ;
    }

    return bRet ;
}

bool isValidPort( std::string &port )
{
    uint32_t result = 0;

    std::string trimmedString = trimString(port);

    if ( (trimmedString.empty()) || (trimmedString.length() > 5) )
    {
        return false ;
    }

    if ( stringToUI32(trimmedString.c_str(), result) == false )
    {
        return false ;
    }

    if (( result <= 0 ) || (result > 65535 ))
    {
        return false ;
    }

    return true;
}

bool isValidIPAddress( std::string address )
{
    if (address.empty() == true) { return false ; }

    return pal_validateIPAddress( address );
}

bool stringToUI32(const char *pStr, uint32_t &result)
{
    result = 0;

    if ( pStr == NULL )
    {
        return false;
    }

    char *pC = (char *)pStr;

    while (*pC != 0x00)
    {
        if ((*pC >= '0') || (*pC <= '9'))
        {
            result = (result * (10)) + (*pC - '0');
        }
        else
        {
            return false;
        }

        pC++;
    }

    return true ;
}

