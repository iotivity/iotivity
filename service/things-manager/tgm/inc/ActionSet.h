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

/// @file ActionSet.h

/// @brief

#ifndef __OC_ACTION_SET__
#define __OC_ACTION_SET__

#include <string>
#include <vector>

#include "Action.h"

namespace OC
{

    using namespace std;

    typedef unsigned int ACTIONSET_ID;
    typedef ACTIONSET_ID ASID;

    enum ACTIONSET_RESULT
    {
        RESULT_SUCCESS,

        RESULT_CREATE_ACTUATIONSET_SUCCESS, RESULT_CREATE_ACTUATIONSET_FAIL,

        RESULT_READ_ACTUATIONSET_SUCCESS, RESULT_READ_ACTUATIONSET_FAIL,

        RESULT_UPDATE_ACTUATIONSET_SUCCESS, RESULT_UPDATE_ACTUATIONSET_FAIL,

        RESULT_UPDATE_ACTUATION_SUCCESS, RESULT_UPDATE_ACTUATION_FAIL,

        RESULT_DELETE_ACTUATIONSET_SUCCESS, RESULT_DELETE_ACTUATIONSET_FAIL,

        RESULT_DELETE_ACTUATION_SUCCESS, RESULT_DELETE_ACTUATION_FAIL,

        RESULT_EXECUTE_ACTUATIONSET_SUCCESS, RESULT_EXECUTE_ACTUATIONSET_FAIL,

        RESULT_FAIL
    };

    typedef vector< Action* >::iterator const_iterator;

    class ActionSet
    {

    public:
        ActionSet()
        {
            m_actuationSetId = 0;
        }
        ~ActionSet()
        {
            
        }

        string m_name;
        ASID m_actuationSetId;
        vector< Action* > m_listActions;

        const_iterator findAction(AID aid)
        {
            // T. B. D
            const_iterator it = m_listActions.begin();

            return it;
        }

        std::string description()
        {
            return "actionset name : " + this->m_name;
        }
    };

}

#endif /* __OC_ACTUATION_SET_ */
