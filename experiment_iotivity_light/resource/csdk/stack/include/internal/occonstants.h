//******************************************************************
//
// Copyright 2016 Intel Corporation All Rights Reserved.
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

/*
 * Constants that need scope, manifest or not
 *
 * Local constants need not be here.
 * Configuration values belong in occonfig.h.
 */

#ifndef OC_CONSTANTS_H_
#define OC_CONSTANTS_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "occonfig.h"

/** The coap scheme */
#define OC_COAP_SCHEME "coap://"

/** the first outgoing sequence number will be 5*/
#define OC_OFFSET_SEQUENCE_NUMBER (4)

/**
 * OIC Virtual resources supported by every OIC device.
 */
/**
 *  Default discovery mechanism using '/oic/res' is supported by all OIC devices
 *  That are Discoverable.
 */
#define OC_RSRVD_WELL_KNOWN_URI               "/oic/res"

/** Device URI.*/
#define OC_RSRVD_DEVICE_URI                   "/oic/d"

/** Platform URI.*/
#define OC_RSRVD_PLATFORM_URI                 "/oic/p"

/** Resource Type.*/
#define OC_RSRVD_RESOURCE_TYPES_URI           "/oic/res/types/d"

/** Presence URI through which the OIC devices advertise their presence.*/
#define OC_RSRVD_PRESENCE_URI                 "/oic/ad"

/** Security URIs all start with this prefix.*/
#define OC_RSRVD_SECURITY_PREFIX              "/oic/sec/"

/** Sets the default time to live (TTL) for presence.*/
#define OC_DEFAULT_PRESENCE_TTL_SECONDS (60)

/**
 *  OC_DEFAULT_PRESENCE_TTL_SECONDS sets the default time to live (TTL) for presence.
 */
#define OC_DEFAULT_PRESENCE_TTL_SECONDS (60)

/**
 *  OC_MAX_PRESENCE_TTL_SECONDS sets the maximum time to live (TTL) for presence.
 *  NOTE: Changing the setting to a longer duration may lead to unsupported and untested
 *  operation.
 *  60 sec/min * 60 min/hr * 24 hr/day
 */
#define OC_MAX_PRESENCE_TTL_SECONDS     (60 * 60 * 24)

/** For multicast Discovery mechanism.*/
#define OC_MULTICAST_DISCOVERY_URI            "/oic/res"

/** Separator for multiple query string.*/
#define OC_QUERY_SEPARATOR                    "&;"

/**
 *  Presence "Announcement Triggers".
 */

/** To create.*/
#define OC_RSRVD_TRIGGER_CREATE         "create"

/** To change.*/
#define OC_RSRVD_TRIGGER_CHANGE         "change"

/** To delete.*/
#define OC_RSRVD_TRIGGER_DELETE         "delete"

/**
 *  Attributes used to form a proper OIC conforming JSON message.
 */

#define OC_RSRVD_OC                     "oic"

/** For payload. */

#define OC_RSRVD_PAYLOAD                "payload"

/** To represent href */
#define OC_RSRVD_HREF                   "href"

/** To represent property*/
#define OC_RSRVD_PROPERTY               "prop"

/** For representation.*/
#define OC_RSRVD_REPRESENTATION         "rep"

/** To represent content type.*/
#define OC_RSRVD_CONTENT_TYPE           "ct"

/** To represent resource type.*/
#define OC_RSRVD_RESOURCE_TYPE          "rt"

/** To represent resource type with presence.*/
#define OC_RSRVD_RESOURCE_TYPE_PRESENCE "oic.wk.ad"

/** To represent interface.*/
#define OC_RSRVD_INTERFACE              "if"

/** To represent time to live.*/
#define OC_RSRVD_TTL                    "ttl"

/** To represent non*/
#define OC_RSRVD_NONCE                  "non"

/** To represent trigger type.*/
#define OC_RSRVD_TRIGGER                "trg"

/** To represent links.*/
#define OC_RSRVD_LINKS                  "links"

/** To represent default interface.*/
#define OC_RSRVD_INTERFACE_DEFAULT      "oic.if.baseline"

/** To represent ll interface.*/
#define OC_RSRVD_INTERFACE_LL           "oic.if.ll"

/** To represent batch interface.*/
#define OC_RSRVD_INTERFACE_BATCH        "oic.if.b"

/** To represent interface group.*/
#define OC_RSRVD_INTERFACE_GROUP        "oic.mi.grp"

/** To represent MFG date.*/
#define OC_RSRVD_MFG_DATE               "mndt"

/** To represent FW version.*/
#define OC_RSRVD_FW_VERSION             "mnfv"

/** To represent host name.*/
#define OC_RSRVD_HOST_NAME              "hn"

/** To represent version.*/
#define OC_RSRVD_VERSION                "icv"

/** To represent policy.*/
#define OC_RSRVD_POLICY                 "p"

/** To represent bitmap.*/
#define OC_RSRVD_BITMAP                 "bm"

/** For security.*/
#define OC_RSRVD_SECURE                 "sec"

/** Port. */
#define OC_RSRVD_HOSTING_PORT           "port"

/** For Server instance ID.*/
#define OC_RSRVD_SERVER_INSTANCE_ID     "sid"

/**
 *  Platform.
 */

/** Platform ID. */
#define OC_RSRVD_PLATFORM_ID            "pi"

/** Platform MFG NAME. */
#define OC_RSRVD_MFG_NAME               "mnmn"

/** Platform URL. */
#define OC_RSRVD_MFG_URL                "mnml"

/** Model Number.*/
#define OC_RSRVD_MODEL_NUM              "mnmo"

/** Platform MFG Date.*/
#define OC_RSRVD_MFG_DATE               "mndt"

/** Platform versio.n */
#define OC_RSRVD_PLATFORM_VERSION       "mnpv"

/** Platform Operating system version. */
#define OC_RSRVD_OS_VERSION             "mnos"

/** Platform Hardware version. */
#define OC_RSRVD_HARDWARE_VERSION       "mnhw"

/**Platform Firmware version. */
#define OC_RSRVD_FIRMWARE_VERSION       "mnfv"

/** Support URL for the platform. */
#define OC_RSRVD_SUPPORT_URL            "mnsl"

/** System time for the platform. */
#define OC_RSRVD_SYSTEM_TIME             "st"

/**
 *  Device.
 */

/** Device ID.*/
#define OC_RSRVD_DEVICE_ID              "di"

/** Device Name.*/
#define OC_RSRVD_DEVICE_NAME            "n"

/** Device specification version.*/
#define OC_RSRVD_SPEC_VERSION           "lcv"

/** Device data model.*/
#define OC_RSRVD_DATA_MODEL_VERSION     "dmv"

/** Device specification version.*/
#define OC_SPEC_VERSION                "0.9.0"

/** Device Data Model version.*/
#define OC_DATA_MODEL_VERSION          "sec.0.95"

/**
 *  These provide backward compatibility - their use is deprecated.
 */
#ifndef GOING_AWAY

/** Multicast Prefix.*/
#define OC_MULTICAST_PREFIX                  "224.0.1.187:5683"

/** Multicast IP address.*/
#define OC_MULTICAST_IP                      "224.0.1.187"

/** Multicast Port.*/
#define OC_MULTICAST_PORT                    5683
#endif // GOING_AWAY

#define MAX_ADDR_STR_SIZE (40)  // chars to store IPv6 address

/** Max identity size. */
#define MAX_IDENTITY_SIZE (32)

#define MAX_RESOURCE_NAME_SIZE (128)
#define MAX_RESOURCE_TYPE_SIZE (32)
#define MAX_RESOURCE_IFACE_SIZE (32)
#define MAX_RESOURCE_ATTR_NAME_SIZE (32)

#define MILLISECONDS_PER_SECOND   (1000)

/**
 * Maximum length of the Manufacturer name supported by the server
 * for manufacturer name.
 */
#define MAX_MANUFACTURER_NAME_LENGTH (16)

/**
 * Maximum length of the URL to the Manufacturer details supported by
 * the server.
 */
#define MAX_MANUFACTURER_URL_LENGTH (32)

/**
 * Maximum number of resources which can be contained inside collection
 * resource.
 */
#define MAX_CONTAINED_RESOURCES  (5)

/**
 *  Maximum number of vendor specific header options an application can set or receive
 *  in PDU
 */
#define MAX_HEADER_OPTIONS (2)

/**
 *  Maximum Length of the vendor specific header option
 */
#define MAX_HEADER_OPTION_DATA_LENGTH (16)

/**
 * Sets the time to live (TTL) for response callback(s).
 * The callback(s) will be up for deletion after such time but are not guaranteed
 * to be deleted immediately and you may get responses even after timeout.
 * This timeout will NOT apply to OBSERVE requests. OBSERVE needs an explicit cancel using OCCancel().
 * @note: Changing the setting to a very long duration may lead to unsupported and untested
 * operation. Setting this to as small a value as reasonable will reclaim memory faster.
 */
#define MAX_CB_TIMEOUT_SECONDS   (2 * 60 * 60)  // 2 hours = 7200 seconds.

/**
 * Max payload length supported
 */
#define MAX_ENCODED_PAYLOAD_LENGTH (1024)

/**
 *Maximum length of the remoteEndpoint identity
 */
#define MAX_ENDPOINT_IDENTITY_LEN   (32)

/**
 * Maximum token length -- defined by CoAP
 * Actual token length is TOKEN_LEN.
 */
#define MAX_TOKEN_LEN (8)

#endif // OC_CONSTANTS_H_
