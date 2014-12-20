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

#ifndef CURRENCYTYPE_H_
#define CURRENCYTYPE_H_

class CurrencyType
{
    public:
        std::string     value;
        bool validateContent()
        {
            if (value == "Unknown" ) {}
            else if (value == "Australian_Dollar" ) {}
            else if (value == "Canadian_Dollar" ) {}
            else if (value == "Korean_Won" ) {}
            else if (value == "US_Dollar" ) {}
            else if (value == "Euro" ) {}
            else { return false; }
            return true ;
        }
};
#endif
