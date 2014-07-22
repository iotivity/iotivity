//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef OC_SERVER_REQUEST_H
#define OC_SERVER_REQUEST_H

#define OC_RSRVD_OC             "oc"
#define OC_RSRVD_PAYLOAD        "payload"
#define OC_RSRVD_HREF           "href"
#define OC_RSRVD_RESOURCE_TYPE  "rt"
#define OC_RSRVD_INTERFACE      "if"
#define OC_RSRVD_OBSERVABLE     "obs"
#define OC_RESOURCE_OBSERVABLE   1

#define OC_RESOURCE_DISCOVERY_URI   PCF("/oc/core")

typedef enum {
    STACK_RES_DISCOVERY_NOFILTER = 0,
    STACK_RES_DISCOVERY_IF_FILTER,
    STACK_RES_DISCOVERY_RT_FILTER
} StackQueryTypes;

OCStackResult processResourceDiscoverReq (const char *request, char *response,
                                 uint8_t filterOn, char *filterValue);

OCStackResult validateUrlQuery (char *url, char *query, uint8_t *filterOn, char **filterValue);
#endif //OC_SERVER_REQUEST_H
