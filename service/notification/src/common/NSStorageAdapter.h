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

#ifndef _NS_STORAGEADAPTER__H_
#define _NS_STORAGEADAPTER__H_

#include "logger.h"
#include <octypes.h>
#include <stdbool.h>
#include "ocstack.h"
#include "ocpayload.h"
#include "NSStructs.h"
#include "NSConstants.h"

NSCacheList * NSStorageCreate();
NSCacheElement * NSStorageRead(NSCacheList * list, const char * findId);
NSResult NSStorageWrite(NSCacheList * list, NSCacheElement * newObj);
NSResult NSStorageDelete(NSCacheList * list, const char * delId);
NSResult NSStorageDestroy(NSCacheList * list);

#endif /* _NS_STORAGEADAPTER__H_ */
