/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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

package org.iotivity.service.ic;

import java.util.HashMap;
import java.util.Map;

import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcResource;
import org.iotivity.base.QualityOfService;

public interface ICUtility {
    public final String              TAG                      = "ICTestApp";
    public final Map<String, String> MY_MAP                   = new HashMap<String, String>() {
                                                                  {
                                                                      put("a", "b");
                                                                  }
                                                              };
    public final String              IC_HOST_ADDRESS          = "coap+tcp://192.168.43.174:5683";
    public final String              MQ_BROKER_URI            = "/oic/ps";
    public final QualityOfService    IC_OC_QUALITY_OF_SERVICE = QualityOfService.LOW;
    public final String              DEFAULT_AUTH_PROVIDER    = "github";
    public final String              MQ_DEFAULT_TOPIC_URI     = "/oic/ps/cleanroom";
    public final String              Resource_URI             = "/a/light";
    public final String              RESOURCE_TYPE            = "core.light";
    public final String              RESOURCE_INTERFACE       = OcPlatform.DEFAULT_INTERFACE;
    public final int                 TIMEOUT                  = 5000;
}
