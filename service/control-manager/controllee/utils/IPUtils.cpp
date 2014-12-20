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

#include "NetUtils.h"
#include "boost/asio.hpp"
#include <boost/regex.hpp>

bool pal_validateIPAddress( std::string ipAddress )
{
    boost::system::error_code errorCode ;
    boost::asio::ip::address::from_string( ipAddress, errorCode );

    if ( errorCode )
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool pal_validateEmail(const std::string &email)
{
    const std::string expression = "^[_A-Za-z0-9-]+(\\.[_A-Za-z0-9-]+)*@[A-Za-z0-9-]+(\\.[A-Za-z0-9]+)*(\\.[A-Za-z]{2,})$";
    boost::regex validationExpression(expression);
    if (false == boost::regex_match(email, validationExpression))
    {
        return false;
    }
    return true;
}
