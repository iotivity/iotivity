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

package org.iotivity.service.ns.testapp;

public interface NSAppUtility {
    String  TAG                       = "NSTestApp";
    String  USER_INFO                 = "Info";
    String  MSG_TITLE                 = "NS Android Msg Title";
    String  MSG_BODY                  = "NS Android Msg  Body";
    String  MSG_SOURCE                = "OCF";
    String  TOPIC_NAME                = "OCF_ANDROID_TOPIC";

    Boolean IS_SECURED                = false;

    int     NS_OK                     = 100;
    int     NS_ERROR                  = 200;
    int     NS_SUCCESS                = 300;
    int     NS_FAIL                   = 400;
}
