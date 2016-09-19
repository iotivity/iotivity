//******************************************************************
//
// Copyright 2016 Intel Mobile Communications GmbH All Rights Reserved.
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

#include "pkix_interface.h"
#include "credresource.h"
#include "crlresource.h"
#include "logger.h"

#define TAG "SRM-PKIX-INTERFACE"

void GetPkixInfo(PkiInfo_t * inf)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);
    GetDerOwnCert(&inf->crt);
    GetDerKey(&inf->key);
    GetDerCaCert(&inf->ca);
    GetDerCrl(&inf->crl);
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
}
