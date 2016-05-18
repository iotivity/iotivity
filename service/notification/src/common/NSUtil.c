//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include "NSUtil.h"

#include "oic_malloc.h"

NSResult NSFreeMessage(NSMessage * obj)
{
    if (!obj)
    {
        return NS_ERROR;
    }

    if (obj->mId)
    {
        OICFree(obj->mId);
        obj->mId = NULL;
    }

    if (obj->mTitle)
    {
        OICFree(obj->mTitle);
        obj->mTitle = NULL;
    }

    if (obj->mContentText)
    {
        OICFree(obj->mContentText);
        obj->mContentText = NULL;
    }

    OICFree(obj);

    return NS_OK;
}


