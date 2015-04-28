//******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
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

#include <stdlib.h>
#include "securevirtualresourcetypes.h"

const char * SVR_DB_FILE_NAME = "oic_svr_db.json";
const char * OIC_MI_DEF = "oic.mi.def";

const char * OIC_RSRC_CORE_URI =  "/oc/core";
const char * OIC_RSRC_CORE_D_URI =  "/oc/core/d";
const char * OIC_RSRC_PRESENCE_URI =  "/oc/presence";
const char * OIC_RSRC_TYPES_D_URI =  "/oc/core/types/d";

//ACL
const char * OIC_RSRC_TYPE_SEC_ACL = "oic.sec.acl";
const char * OIC_RSRC_ACL_URI =  "/oic/sec/acl";
const char * OIC_JSON_ACL_NAME = "acl";

//Pstat
const char * OIC_RSRC_TYPE_SEC_PSTAT = "oic.sec.pstat";
const char * OIC_RSRC_PSTAT_URI =  "/oic/sec/pstat";
const char * OIC_JSON_PSTAT_NAME = "pstat";

//doxm
const char * OIC_RSRC_TYPE_SEC_DOXM = "oic.sec.doxm";
const char * OIC_RSRC_DOXM_URI =  "/oic/sec/doxm";
const char * OIC_JSON_DOXM_NAME = "doxm";

//cred
const char * OIC_RSRC_TYPE_SEC_CRED = "oic.sec.cred";
const char * OIC_RSRC_CRED_URI =  "/oic/sec/cred";
const char * OIC_JSON_CRED_NAME = "cred";

const char * OIC_JSON_SUBJECT_NAME = "sub";
const char * OIC_JSON_RESOURCES_NAME = "rsrc";
const char * OIC_JSON_PERMISSION_NAME = "perms";
const char * OIC_JSON_OWNERS_NAME = "ownrs";
const char * OIC_JSON_OWNER_NAME = "ownr";
const char * OIC_JSON_OWNED_NAME = "owned";
const char * OIC_JSON_OXM_NAME = "oxm";
const char * OIC_JSON_OXM_TYPE_NAME = "oxmtype";
const char * OIC_JSON_OXM_SEL_NAME = "oxmsel";
const char * OIC_JSON_DEVICE_ID_FORMAT_NAME = "dvcidfrmt";
const char * OIC_JSON_ISOP_NAME = "isop";
const char * OIC_JSON_COMMIT_HASH_NAME = "commithash";
const char * OIC_JSON_DEVICE_ID_NAME = "deviceid";
const char * OIC_JSON_CM_NAME = "cm";
const char * OIC_JSON_TM_NAME = "tm";
const char * OIC_JSON_OM_NAME = "om";
const char * OIC_JSON_SMS_NAME = "sms";
const char * OIC_JSON_CREDID_NAME = "credid";
const char * OIC_JSON_SUBJECTID_NAME = "subid";
const char * OIC_JSON_ROLEIDS_NAME = "roleid";
const char * OIC_JSON_CREDTYPE_NAME = "credtyp";
const char * OIC_JSON_PUBLICDATA_NAME = "pbdata";
const char * OIC_JSON_PRIVATEDATA_NAME = "pvdata";
const char * OIC_JSON_PERIOD_NAME = "period";

const OicUuid_t WILDCARD_SUBJECT_ID = {"*"};
const size_t WILDCARD_SUBJECT_ID_LEN = 1 ;

