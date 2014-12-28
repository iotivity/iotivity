//******************************************************************
//
// Copyright 2014 MediaTek All Rights Reserved.
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

package org.iotivity.base;

public class OCStackResult {
    /* Success status code - START HERE */
    static final public int  OC_STACK_OK                    = 0;
    static final public int  OC_STACK_RESOURCE_CREATED      = 1;
    static final public int  OC_STACK_RESOURCE_DELETED      = 2;
    /* Success status code - END HERE */
    /* Error status code - START HERE */
    static final public int  OC_STACK_INVALID_URI           = 3;
    static final public int  OC_STACK_INVALID_QUERY         = 4;
    static final public int  OC_STACK_INVALID_IP            = 5;
    static final public int  OC_STACK_INVALID_PORT          = 6;
    static final public int  OC_STACK_INVALID_CALLBACK      = 7;
    static final public int  OC_STACK_INVALID_METHOD        = 8;
    static final public int  OC_STACK_INVALID_PARAM         = 9;
    static final public int  OC_STACK_INVALID_OBSERVE_PARAM = 10;
    static final public int  OC_STACK_NO_MEMORY             = 11;
    static final public int  OC_STACK_COMM_ERROR            = 12;
    static final public int  OC_STACK_NOTIMPL               = 13;
    static final public int  OC_STACK_NO_RESOURCE           = 14;               /* resource not found */
    static final public int  OC_STACK_RESOURCE_ERROR        = 15;            /* ex: not supported method or interface */
    static final public int  OC_STACK_SLOW_RESOURCE         = 16;
    static final public int  OC_STACK_NO_OBSERVERS          = 17;              /* resource has no registered observers */
    static final public int  OC_STACK_OBSERVER_NOT_FOUND    = 18;
    static final public int  OC_STACK_OBSERVER_NOT_ADDED    = 19;
    static final public int  OC_STACK_OBSERVER_NOT_REMOVED  = 20;
    static final public int  OC_STACK_PRESENCE_STOPPED      = 21;
    static final public int  OC_STACK_PRESENCE_DO_NOT_HANDLE = 22;
    static final public int  OC_STACK_INVALID_OPTION        = 23;
    static final public int  OC_STACK_MALFORMED_RESPONSE    = 24;        /* the remote reply contained malformed data */
    static final public int  OC_STACK_ERROR                 = 25;
    /* Error status code - END HERE */
}
