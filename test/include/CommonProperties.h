/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#ifndef __COMMON_PROPERTIES_H__
#define __COMMON_PROPERTIES_H__

#define DEFAULT_DEV_ID "99999999-0000-0000-0000-0001311"
#define DEFAULT_DEV_ID_CS_01 "99999999-0000-0000-0000-0001201"
#define DEFAULT_OWNER_ID "4d83e543-22c4-4f58-bd67-8846c2cde4ba"
#define DEFAULT_HOST "127.0.0.1"
#define DASH "-"

/*
 * File Names
 */
const string CLOUD_ACCESSTOKEN_TXT = "cloud_accessToken.txt";
const string CLOUD_UUID_TXT = "cloud_uuid.txt";
const string CI_SERVER_IP_TXT = "ci_server_ip.txt";


/*
 * Callback Releated Resources
 */
const int CALLBACK_NOT_INVOKED = 0;
const int CALLBACK_INVOKED = 1;
const int CALLBACK_CODE_MISMATCH = 2;
const int CALLBACK_TIMEOUT = 60;
const int DELAY_SHORT = 1;
const int DEFAULT_PORT = 5683;

/**
 *  Time Related Resources
 */
const int DELAY_MEDIUM = 5;
const int DELAY_LONG = 10;
const int DISCOVERY_TIMEOUT = 5;
const int DISCOVERY_TIMEOUT_ZERO = 0;
const int DISCOVERY_TIMEOUT_ONE = 1;
const int DISCOVERY_TIMEOUT_TWO = 2;
const int DISCOVERY_TIMEOUT_INVALID = -1;

const int CRED_ID_NEGATIVE = -1;
const int CERT_ID_ONE = 1;

/**
 * DEVICE INDEX
 */
const int DEVICE_INDEX_ONE = 1;
const int DEVICE_INDEX_TWO = 2;

/**
 * Cloud
 */
static const string COAP_HOST_ADDRESS = "coap+tcp://127.0.0.1:5683";
static const string COAPS_HOST_ADDRESS = "coaps+tcp://127.0.0.1:5683";
static const string COAP_TLSOFF_PREFIX = "coap+tcp://";
static const string COAP_TLSON_PREFIX = "coaps+tcp://";
const int  REMOTE_CI_SERVER_PORT = 5683;
#define CLOUD_CLIENT_DAT "./cloud.dat"

/**
 * CLoud Acl Resource
 */
#define CLOUD_ACL_CONTROLLER_DAT "./cloud.dat"
#define CLOUD_ACL_CONTROLLER_DAT_BACKUP "../cloud.dat"
#define ROOT_CERT_FILE "rootca.crt"
#define ROOT_CERT_FILE_BACKUP "../rootca.crt"

const string CRL_THIS_UPDATE = "201712010000000";
const string CRL_NEXT_DATE = "20171202000000";

#define DEFAULT_AUTH_PROVIDER "github"
#define CLOUD_ACL_CONTROLLER_DAT "./cloud.dat"
#define DEFAULT_ACL_ID "5736bf74-b767-425f-bc31-3cae0870c7e2"


/*
 * ACL Releated Resources (PM)
 */
#define DEFAULT_SUBJECT_UUID "*"
#define ACL_RESOURCE_LEN 2
#define DEAFULT_RESOURCE_URI "*"
#define DEFAULT_RESOURCE_TYPE "*"
#define DEAFULT_INTERFACE_TYPE "*"
#define DEAFULT_ROWNER_UUID "*"

#define ACL_RESRC_MAX_NUM   16
#define ACL_RESRC_MAX_LEN   128
#define ACL_PEMISN_CNT      5
#define DEFAULT_DP_PROVSIONING_PIN "00000000"
#define DEFAULT_DP_PROVSIONING_PIN2 "00000011"
#define FULL_PERMISSION 31
#define NO_PERMISSION 0
#define MAX_PERMISSION_RANGE 65535

#ifdef __SECURED__
static const OicSecPrm_t SUPPORTED_PRMS[1] =
{ PRM_PRE_CONFIGURED, };
#endif
#endif
