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

import java.util.EnumSet;
import java.util.LinkedList;
import java.util.List;
import android.util.Log;

import org.iotivity.base.OcAccountManager.OnDeleteListener;
import org.iotivity.base.OcAccountManager.OnGetListener;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcException;
import org.iotivity.base.OcAccountManager.OnObserveListener;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.ServiceType;
import org.iotivity.base.ModeType;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.ResourceProperty;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.OcAccountManager.OnPostListener;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.ErrorCode;
import org.iotivity.base.OcAccountManager;

import static org.iotivity.service.ic.ICUtility.*;
import org.iotivity.service.testapp.framework.Base;

public class ICHelper extends Base {
    private static ICHelper sICHelper = null;
    
    private ICHelper() {
    }
    
    public static ICHelper getInstance(){
        if (sICHelper == null) {
            sICHelper = new ICHelper();
        }
        return sICHelper;
    }
    
     public void showLog(LogLevel level, String log, Boolean isLogClear) {
            switch (level) {
                case INFO:
                    Log.i(TAG, log);
                    break;

                case DEBUG:
                    Log.d(TAG, log);
                    break;

                case WARNING:
                    Log.w(TAG, log);
                    break;

                case ERROR:
                    Log.e(TAG, log);
                    break;

                default:
                    Log.v(TAG, log);
                    break;
            }

            if (isLogClear) {
                clearOutPut();
            }

            showOutPut(log);
        }
     
     enum LogLevel {
            INFO, DEBUG, WARNING, VERBOSE, ERROR
        }

}
