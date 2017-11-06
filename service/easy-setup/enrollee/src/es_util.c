//******************************************************************
//
// Copyright 2017 Samsung Electronics All Rights Reserved.
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

#include <string.h>

#include "escommon.h"

// Lookup Table Template
// static const struct
// {
//     <<EnumType>> valEnum;
//     char *valStr;
// } <<TableName>> [] = { { <<Enum value>>, <<char* value>> }, ... }

// Helper to convert Enum to String (char*) value (by searching it in the lookup table)
#define LookupEnumToStr( val, lut, ret) \
{ \
    ret = NULL; \
    const size_t n = sizeof(lut) / sizeof(lut[0]); \
    for (size_t i = 0; i < n; ++i) \
    { \
        if (lut[i].valEnum == val) \
        { \
            ret = lut[i].valStr; \
            break; \
        } \
    } \
}

// Helper to convert String (char*) to Enum value (by searching it in the lookup table)
#define LookupStrToEnum( valIn, lut, valOut, result) \
{ \
    result = false; \
    const size_t n = sizeof(lut) / sizeof(lut[0]); \
    for (size_t i = 0; i < n; ++i) \
    { \
        if (strcmp(lut[i].valStr, valIn) == 0) \
        { \
            valOut = lut[i].valEnum; \
            result = true; \
            break; \
        } \
    } \
}

static const struct
{
    WIFI_MODE valEnum;
    char *valStr;
} wifiModeConvertLookup[] =
{
    { WIFI_11A, "A" },
    { WIFI_11B, "B" },
    { WIFI_11G, "G" },
    { WIFI_11N, "N" },
    { WIFI_11AC, "AC" },
};

static const struct
{
    WIFI_FREQ valEnum;
    char *valStr;
} wifiFreqConvertLookup[] =
{
    { WIFI_24G, "2.4G" },
    { WIFI_5G, "5G"}
};

static const struct
{
    WIFI_AUTHTYPE valEnum;
    char *valStr;
} wifiAuthtypeConvertLookup[] =
{
    { NONE_AUTH, "None" },
    { WEP, "WEP"},
    { WPA_PSK, "WPA_PSK" },
    { WPA2_PSK, "WPA2_PSK" },
};

static const struct
{
    WIFI_ENCTYPE valEnum;
    char *valStr;
} wifiEnctypeConvertLookup[] =
{
    { NONE_ENC, "None" },
    { WEP_64, "WEP_64" },
    { WEP_128, "WEP_128" },
    { TKIP, "TKIP" },
    { AES, "AES" },
    { TKIP_AES, "TKIP_AES" },
};

const char* WiFiModeEnumToString(WIFI_MODE val)
{
    char *ret = NULL;
    LookupEnumToStr(val, wifiModeConvertLookup, ret);
    return ret;
}

const char* WiFiFreqEnumToString(WIFI_FREQ val)
{
    char *ret = NULL;
    LookupEnumToStr(val, wifiFreqConvertLookup, ret);
    return ret;
}

const char* WiFiAuthTypeEnumToString(WIFI_AUTHTYPE val)
{
    char *ret = NULL;
    LookupEnumToStr(val, wifiAuthtypeConvertLookup, ret);
    return ret;
}

bool WiFiAuthTypeStringToEnum(const char *val, WIFI_AUTHTYPE *valOut)
{
    bool result = false;
    LookupStrToEnum(val, wifiAuthtypeConvertLookup, (*valOut), result);
    return result;
}

const char* WiFiEncTypeEnumToString(WIFI_ENCTYPE val)
{
    char *ret = NULL;
    LookupEnumToStr(val, wifiEnctypeConvertLookup, ret);
    return ret;
}

bool WiFiEncTypeStringToEnum(const char *val, WIFI_ENCTYPE *valOut)
{
    bool result = false;
    LookupStrToEnum(val, wifiEnctypeConvertLookup, (*valOut), result);
    return result;
}
