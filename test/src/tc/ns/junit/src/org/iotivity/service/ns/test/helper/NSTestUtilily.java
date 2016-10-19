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

package org.iotivity.service.ns.test.helper;

public interface NSTestUtilily {
  String  TAG               = "NSTest";
  String  USER_INFO         = "Info";
  String  MSG_TITLE         = "TestTitle";
  String  MSG_BODY          = "TestMessageBody";
  String  MSG_SOURCE        = "OCF";
  String  TOPIC_NAME        = "OCF_TestTopic";

  int     WAIT_MAX_TIME     = 10;
  int     WAIT_MIN_TIME     = 1;
  int     WAIT_DEFAULT_TIME = 2;

  int     NS_OK             = 100;
  int     NS_ERROR          = 200;
  int     NS_SUCCESS        = 300;
  int     NS_FAIL           = 400;

  long    MESSAGE_ID        = 1000;
  String  PROVIDER_ID       = "TEST_0001";
  Boolean IS_SECURED        = false;
}
