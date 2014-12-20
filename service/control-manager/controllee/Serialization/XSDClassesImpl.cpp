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
#include "DateTimeType.h"
#include "TimeType.h"
#include "UuidType.h"
#include "TimeOffsetType.h"

namespace OC
{
    namespace Cm
    {
        namespace Serialization
        {
            namespace Xsd
            {

                bool
                DateTimeType::validateContent()
                {
                    // [yyyy]-[mm]-[dd]T[hh]:[mm]:[ss]
                    // yyyy-mm-ddThh:mm:ss

                    this->value = trimString(this->value) ;

                    if (value.length() != 19)
                    {
                        return false ;
                    }

                    const char *inputCString = value.c_str();

                    for (unsigned int index = 0; index < value.length(); index++)
                    {
                        if ((index == 4) || (index == 7))
                        {
                            if ( inputCString[index] != '-')
                            {
                                return false;
                            }
                        }
                        else if ((index == 13) || (index == 16))
                        {
                            if ( inputCString[index] != ':')
                            {
                                return false;
                            }
                        }
                        else if (index == 10)
                        {
                            if ( inputCString[index] != 'T')
                            {
                                return false;
                            }
                        }
                        else
                        {
                            if (isdigit(inputCString[index]) == false)
                            {
                                return false;
                            }
                        }
                    }


                    return true;
                }

                bool
                TimeType::validateContent()
                {
                    // [hh]:[mm]:[ss]
                    // hh:mm:ss
                    this->value = trimString(this->value) ;

                    if (value.length() != 8)
                    {
                        return false ;
                    }

                    const char *inputCString = value.c_str();

                    for (unsigned int index = 0; index < value.length(); index++)
                    {
                        if ((index == 2) || (index == 5))
                        {
                            if ( inputCString[index] != ':')
                            {
                                return false;
                            }
                        }
                        else
                        {
                            if (isdigit(inputCString[index]) == false)
                            {
                                return false;
                            }
                        }
                    }

                    return true;
                }


                bool
                UuidType::validateContent()
                {
                    // 8-4-4-4-12
                    // 550e8400-e29b-41d4-a716-446655440000

                    this->value = trimString(this->value) ;

                    if (value.length() != 36)
                    {
                        return false ;
                    }

                    const char *inputCString = value.c_str();

                    for (unsigned int index = 0; index < value.length(); index++)
                    {
                        if ((index == 8) || (index == 13) || (index == 18) || (index == 23))
                        {
                            if ( inputCString[index] != '-')
                            {
                                return false;
                            }
                        }
                        else
                        {
                            if (isalnum(inputCString[index]) == false)
                            {
                                return false;
                            }
                        }
                    }


                    return true;
                }


                bool
                TimeOffsetType::validateContent()
                {
                    // �[hh]:[mm]
                    // �hh:mm
                    this->value = trimString(this->value) ;

                    if (value.length() != 6)
                    {
                        return false ;
                    }

                    const char *inputCString = value.c_str();

                    for (unsigned int index = 0; index < value.length(); index++)
                    {
                        if (index == 0)
                        {
                            if (inputCString[index] != '+' && inputCString[index] != '-')
                            {
                                return false;
                            }
                        }
                        else if (index == 3)
                        {
                            if ( inputCString[index] != ':')
                            {
                                return false;
                            }
                        }
                        else
                        {
                            if (isdigit(inputCString[index]) == false)
                            {
                                return false;
                            }
                        }
                    }

                    return true;
                }




            }
        }
    }
}
