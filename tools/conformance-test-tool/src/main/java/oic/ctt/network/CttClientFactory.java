// ******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package oic.ctt.network;

import java.util.ArrayList;
import java.util.HashMap;

import org.slf4j.Logger;

import oic.ctt.formatter.OCPayloadType;
import oic.ctt.logger.CTLogger;
import oic.ctt.network.OICHelper.MessageParameters;
import oic.ctt.network.coap.OICCoapClient;
import oic.ctt.network.http.OICHttpClient;

/**
 * The Ctt Client Factory class. Provides a specific type of client.
 * Also provides abstraction over HTTP & CoAP.
 */
public class CttClientFactory {

    public enum CttClientStack 
    {
    	OIC, IOTIVITY;
    }
    
    public static CttClient getCttClient(CttClientStack cttClientStack)
    {
    	if(cttClientStack == CttClientStack.OIC)
    	{
    		return new OICClient();
    	}
    	else if(cttClientStack == CttClientStack.IOTIVITY)
    	{
    		return new IotivityClient();
    	}
    	
    	return null;
    }
}
