//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

package OC.Cm.Client.Resource;

public interface IResourceResponseListener {

    /**
     * listener to receive response of Get request
     * 
     * @param requestId
     *            id of the request to which current response belongs
     * @param status
     *            Status code of the response
     * @param headers
     *            response headers
     * @param response
     *            response data
     * 
     * @see Sec::Cm::Client::Resource::Resource::get()
     */
    public void OnGetResponseReceived(int clientSessionId, int status,
            OC.Cm.Connector.HeadersList headers,
            OC.Cm.Serialization.ISerializable response);

    /**
     * listener to receive response of put request
     * 
     * @param requestId
     *            id of the request to which current response belongs
     * @param status
     *            Status code of the response
     * @param headers
     *            response headers
     * @param response
     *            response data
     * 
     * @see Sec::Cm::Client::Resource::Resource::put()
     */
    public void OnPutResponseReceived(int clientSessionId, int status,
            OC.Cm.Connector.HeadersList headers,
            OC.Cm.Serialization.ISerializable response);

    /**
     * listener to receive response of post request
     * 
     * @param requestId
     *            id of the request to which current response belongs
     * @param status
     *            Status code of the response
     * @param headers
     *            response headers
     * @param response
     *            response data
     * 
     * @see Sec::Cm::Client::Resource::Resource::post()
     */
    public void OnPostResponseReceived(int clientSessionId, int status,
            OC.Cm.Connector.HeadersList headers,
            OC.Cm.Serialization.ISerializable response);

    /**
     * listener to receive response of delete resource request
     * 
     * @param requestId
     *            id of the request to which current response belongs
     * @param status
     *            Status code of the response
     * @param headers
     *            response headers
     * @param response
     *            response data
     * 
     * @see Sec::Cm::Client::Resource::Resource::deleteResource()
     */
    public void onDeleteResponseReceived(int clientSessionId, int status,
            OC.Cm.Connector.HeadersList headers,
            OC.Cm.Serialization.ISerializable response);

    /**
     * listener to receive response of subscribe notification request
     * 
     * @param requestId
     *            id of the request to which current response belongs
     * @param status
     *            Status code of the response
     * @param subscriptionUri
     *            Subscription URI
     * 
     * @see Sec::Cm::Client::Resource::Resource::subscribe()
     */
    public void onSubscribeResponseReceived(int clientSessionId, int status,
            String subscriptionUri);

    /**
     * listener to receive response of unsubscribe notification request
     * 
     * @param requestId
     *            id of the request to which current response belongs
     * @param status
     *            Status code of the response
     * 
     * @see Sec::Cm::Client::Resource::Resource::unsubscribe()
     */
    public void onUnSubscribeResponseReceived(int clientSessionId, int status);

    /**
     * Error callback for Cm Requests sent using
     * Sec::Cm::Client::Resource::Resource class.
     * 
     * @param requestId
     *            id of the request to which current response belongs
     * @param error
     *            error object which contains detailed error information
     * @param errorMessage
     *            error object received from server
     * 
     */
    public void onErrorReceived(int clientSessionId, int status,
            OC.Cm.CMErrorCode error_code, String errorMessage,
            OC.Cm.Serialization.ISerializable error);

    /**
     * callback which indicates the completion of processing Cm Requests sent
     * using Sec::Cm::Client::Resource::Resource class.
     * 
     * @param requestId
     *            id of the request to which current response belongs
     * @param requestPayload
     *            HTTP Request data
     * @param responsePayload
     *            HTTP Response data
     * 
     */
    public void onRequestComplete(int clientSessionId, String requestPayload,
            String responsePayload);
}
