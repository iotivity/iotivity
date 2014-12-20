/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
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

#include "generated_resource/list_networks.h"
#include "base/logging.h"

namespace webservice
{

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseList*
      * @return static RegisterList
      */
    RegisterList ListNetworks::regList("networks", &ListNetworks::Create);

    ListNetworks::ListNetworks()
    {
    }

    ListNetworks::~ListNetworks()
    {
    }

    /**
      * Create this class.
      * @return BaseList*
      */
    BaseList *ListNetworks::Create()
    {
        DLOG(ERROR) << "ListNetworks is created";
        return new ListNetworks;
    }

}