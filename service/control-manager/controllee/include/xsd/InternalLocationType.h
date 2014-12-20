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

#ifndef INTERNALLOCATIONTYPE_H_
#define INTERNALLOCATIONTYPE_H_

class InternalLocationType
{
    public:
        std::string     value;
        bool validateContent()
        {
            if (value == "Living_Room" ) {}
            else if (value == "Bed_Room" ) {}
            else if (value == "Study_Room" ) {}
            else if (value == "Dining_Room" ) {}
            else if (value == "Kitchen" ) {}
            else if (value == "Bathroom" ) {}
            else if (value == "Laundry_Room" ) {}
            else if (value == "Storage_Room" ) {}
            else if (value == "Balcony" ) {}
            else if (value == "Terrace" ) {}
            else if (value == "Garden" ) {}
            else if (value == "Parking_Garage" ) {}
            else if (value == "Lobby" ) {}
            else if (value == "Main_Entrance" ) {}
            else if (value == "Window" ) {}
            else if (value == "Door" ) {}
            else if (value == "Others" ) {}
            else { return false; }
            return true ;
        }
};
#endif
