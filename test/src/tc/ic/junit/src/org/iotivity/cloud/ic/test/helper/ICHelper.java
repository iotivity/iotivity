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

package org.iotivity.cloud.ic.test.helper;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import android.content.Context;
import android.util.Log;

public class ICHelper {

                /************************** Declare class variables *************************/

  public final static String                       TAG                                  = "ICTest";

  private boolean                           m_OnPutCalled                        = false;
  private static final String               IC_KEY_HUMIDITY                      = "humidity";
  public int                                m_PostHum                            = 0;
  public Boolean                            m_PostCompleted                      = false;
  public static final int                   OCSTACK_OK                           = 0;
  public static final int                   OCSTACK_ERROR                        = 255;
  public static final int                   IC_MAX_WAIT_TIME                     = 10;
  public static final int                   IC_MIN_WAIT_TIME                     = 5;


  public ICHelper(){
     
  }
  
  public static  void icConfigurePlatform(Context appContext) {
      OcPlatform.Configure(new PlatformConfig(appContext,
              ServiceType.IN_PROC, ModeType.CLIENT_SERVER, "0.0.0.0", 0,
              QualityOfService.LOW));
      Log.i(TAG, "Configuration done successfully");
  }
}
