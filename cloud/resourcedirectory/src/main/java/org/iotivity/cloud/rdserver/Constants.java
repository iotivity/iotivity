/*
 * //******************************************************************
 * //
 * // Copyright 2016 Samsung Electronics All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */
package org.iotivity.cloud.rdserver;

import java.util.Arrays;
import java.util.List;

import org.iotivity.cloud.base.OICConstants;

public class Constants extends OICConstants {

    public static final String       RD_DB_NAME              = "RD_DB";
    public static final String       RD_TABLE                = "RD_TABLE";
    public static final String       PRESENCE_TABLE          = "PRESENCE_TABLE";

    public static final String       DEVICE_NAME             = "n";
    public static final String       DEVICE_ID               = "di";
    public static final String       POLICY                  = "p";
    public static final String       BITMAP                  = "bm";
    public static final String       INS                     = "ins";
    public static final String       DEVICE_TTL              = "lt";
    public static final String       RESOURCE_TTL            = "ttl";
    public static final String       HREF                    = "href";
    public static final String       RESOURCE_TYPE           = "rt";
    public static final String       INTERFACE               = "if";
    public static final String       REL                     = "rel";
    public static final String       TITLE                   = "title";
    public static final String       ANCHOR                  = "anchor";
    public static final String       MEDIA_TYPE              = "type";
    public static final String       LINKS                   = "links";
    public static final String       RESOURCE_TYPE_RDPUBLISH = "oic.wk.rdpub";
    public static final List<String> TAGS                    = Arrays
            .asList(DEVICE_NAME, DEVICE_ID, DEVICE_TTL);

    public static final String       SEARCH_TYPE             = "st";
    public static final String       SEARCH_TYPE_DEVICE_LIST = "didList";
    public static final String       DEVICE_LIST_KEY         = "devices";

    // for '/oic/prs' resource
    public static final String       PRESENCE_STATE          = "state";
    public static final String       PRESENCE_LIST           = "prslist";

    public static final String       RS_NON                  = "non";
    public static final String       RS_TRIGGER              = "trg";

    public static final long         OBSERVE_REGISTER        = 0;
    public static final long         OBSERVE_DEREGISTER      = 1;

    public static final byte         RES_CREATE              = 0;
    public static final byte         RES_CHANGE              = 1;
    public static final byte         RES_DELETE              = 2;

}
