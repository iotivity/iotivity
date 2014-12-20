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

import java.util.Map;

public class Resource {
    private long nativeResource;

    private native long constructNativeClass(long context, String protocol,
            String deviceAddress, String resourcePath);

    private native void deleteNativeResource(long resource);

    private native int get(long resource, Object[] queryParams,
            OC.Cm.Client.Resource.IResourceResponseListener listener)
            throws Exception;

    private native int put(long resource, Object[] queryParams, long data,
            OC.Cm.Client.Resource.IResourceResponseListener listener)
            throws Exception;

    private native int post(long resource, Object[] queryParams, long data,
            OC.Cm.Client.Resource.IResourceResponseListener listener)
            throws Exception;

    private native int delete(long resource, Object[] queryParams,
            OC.Cm.Client.Resource.IResourceResponseListener listener)
            throws Exception;

    private native int subscribe(long resource, String dUuid, String sUuid,
            OC.Cm.Client.Resource.IResourceResponseListener listener,
            String description) throws Exception;

    private native int unSubscribe(long resource, String uuid,
            OC.Cm.Client.Resource.IResourceResponseListener listener)
            throws Exception;

    private native void setFilter(long resource, String filter);

    private native void setRecursive(long resource, boolean recursive);

    private native boolean setResourcePath(long nativeResource,
            String resourcePath);

    @Override
    protected void finalize() throws Throwable {
        this.destroy();
        super.finalize();
    }

    public void destroy() {
        if (nativeResource != 0) {
            this.deleteNativeResource(nativeResource);
            nativeResource = 0;
        }
    }

    public Resource(long context, String protocol, String deviceAddress) {
        nativeResource = constructNativeClass(context, protocol, deviceAddress,
                "");
    }

    public Resource(long context, String protocol, String deviceAddress,
            String resourcePath) {
        nativeResource = constructNativeClass(context, protocol, deviceAddress,
                resourcePath);
    }

    /**
     * This method sends Get request to the resource. This is an asynchronous
     * method. Response of this request can be received through
     * Sec::Cm::Client::
     * Resource::IResourceResponseListener::OnGetResponseReceived()
     * 
     * @param requestId
     *            request identifier
     * @param queryParams
     *            query parameters
     * @param listener
     *            response listener
     * 
     * @return returns Sec::Cm::CmError object with code Cm_SUCCESS on success @n
     *         Sec::Cm::CmError object with error code, error description if
     *         available on failure
     * 
     * 
     * @see Sec
     *      ::Cm::Client::Resource::IResourceResponseListener::OnGetResponseReceived
     *      ()
     */
    public int get(Map<String, String> queryParams,
            OC.Cm.Client.Resource.IResourceResponseListener listener)
            throws Exception {
        return get(
                nativeResource,
                ((queryParams != null) && (queryParams.size() > 0)) ? queryParams
                        .entrySet().toArray() : null, listener);
    }

    /**
     * This method sends Put request to the resource. This is an asynchronous
     * method. Response of this request can be received through
     * Sec::Cm::Client::
     * Resource::IResourceResponseListener::OnPutResponseReceived()
     * 
     * @param requestId
     *            request identifier
     * @param queryParams
     *            query parameters
     * @param data
     *            request payload
     * @param listener
     *            response listener
     * 
     * @return returns Sec::Cm::CmError object with code Cm_SUCCESS on success @n
     *         Sec::Cm::CmError object with error code, error description if
     *         available on failure
     * 
     * @see Sec
     *      ::Cm::Client::Resource::IResourceResponseListener::OnPutResponseReceived
     *      ()
     */
    public int put(Map<String, String> queryParams,
            OC.Cm.Serialization.ISerializable data,
            OC.Cm.Client.Resource.IResourceResponseListener listener)
            throws Exception {
        if (null == data) {
            return put(
                    nativeResource,
                    ((queryParams != null) && (queryParams.size() > 0)) ? queryParams
                            .entrySet().toArray() : null, 0, listener);
        } else {
            return put(
                    nativeResource,
                    ((queryParams != null) && (queryParams.size() > 0)) ? queryParams
                            .entrySet().toArray() : null,
                    data.getNativeHandle(), listener);
        }
    }

    /**
     * This method sends Post request to the resource. This is an asynchronous
     * method. Response of this request can be received through
     * Sec::Cm::Client::
     * Resource::IResourceResponseListener::OnPostResponseReceived()
     * 
     * @param requestId
     *            request identifier
     * @param queryParams
     *            query parameters
     * @param data
     *            request payload
     * @param listener
     *            response listener
     * 
     * @return returns Sec::Cm::CmError object with code Cm_SUCCESS on success @n
     *         Sec::Cm::CmError object with error code, error description if
     *         available on failure
     * 
     * @see Sec::Cm::Client::Resource::IResourceResponseListener::
     *      OnPostResponseReceived()
     */
    public int post(Map<String, String> queryParams,
            OC.Cm.Serialization.ISerializable data,
            OC.Cm.Client.Resource.IResourceResponseListener listener)
            throws Exception {
        if (null == data) {
            return post(
                    nativeResource,
                    ((queryParams != null) && (queryParams.size() > 0)) ? queryParams
                            .entrySet().toArray() : null, 0, listener);
        } else {
            return post(
                    nativeResource,
                    ((queryParams != null) && (queryParams.size() > 0)) ? queryParams
                            .entrySet().toArray() : null,
                    data.getNativeHandle(), listener);
        }
    }

    /**
     * This method sends delete request to the resource. This is an asynchronous
     * method. Response of this request can be received through
     * Sec::Cm::Client::
     * Resource::IResourceResponseListener::onDeleteResponseReceived()
     * 
     * @param requestId
     *            request identifier
     * @param queryParams
     *            query parameters
     * @param listener
     *            response listener
     * 
     * @return returns @c true if request is sent successfully @n
     * @c false in case of any error
     * 
     * @see Sec::Cm::Client::Resource::IResourceResponseListener::
     *      onDeleteResponseReceived()
     */
    public int deleteResource(Map<String, String> queryParams,
            OC.Cm.Client.Resource.IResourceResponseListener listener)
            throws Exception {
        return delete(
                nativeResource,
                ((queryParams != null) && (queryParams.size() > 0)) ? queryParams
                        .entrySet().toArray() : null, listener);
    }

    /**
     * 
     * This method is used to subscribe notification for this resource. This is
     * an asynchronous method. Response of this request can be received through
     * Sec::Cm::Client::Resource::IResourceResponseListener::
     * onSubscribeResponseReceived()
     * 
     * @param requestId
     *            request identifier
     * @param dUuid
     *            uuid of the device to which this request is being sent
     * @param sUuid
     *            uuid of self device
     * @param listener
     *            listener to receive response of this request
     * @param description
     *            description member value of Subscription Object
     * @return returns Sec::Cm::CmError object with code Cm_SUCCESS on success @n
     *         Sec::Cm::CmError object with error code, error description if
     *         available on failure
     * 
     * @see Sec::Cm::Client::Resource::IResourceResponseListener::
     *      onSubscribeResponseReceived()
     */
    public int subscribe(String dUuid, String sUuid,
            OC.Cm.Client.Resource.IResourceResponseListener listener,
            String description) throws Exception {
        return subscribe(nativeResource, dUuid, sUuid, listener, description);
    }

    /**
     * 
     * This method is used to unsubscribe notifications for this resource. This
     * is an asynchronous method. Response of this request can be received
     * through Sec::Cm::Client::Resource::IResourceResponseListener::
     * onUnSubscribeResponseReceived()
     * 
     * @param requestId
     *            request identifier
     * @param uuid
     *            uuid of the device to which this request is being sent
     * @param listener
     *            listener to receive response of this request
     * @return returns Sec::Cm::CmError object with code Cm_SUCCESS on success @n
     *         Sec::Cm::CmError object with error code, error description if
     *         available on failure
     * 
     * @see Sec::Cm::Client::Resource::IResourceResponseListener::
     *      onUnSubscribeResponseReceived()
     */
    public int unSubscribe(String uuid,
            OC.Cm.Client.Resource.IResourceResponseListener listener)
            throws Exception {
        return unSubscribe(nativeResource, uuid, listener);
    }

    /**
     * 
     * Configures filter query parameter
     * 
     * @param filter
     *            value of filter query parameters
     * 
     * @return returns @c true if request is sent successfully @n
     * @c false in case of any error
     * 
     * @see Sec::Cm::Client::Resource::Resource::unSetFilter()
     */
    public void setFilter(String filter) {
        setFilter(nativeResource, filter);
    }

    /**
     * 
     * Configures recursive query parameter
     * 
     * @param recursive
     *            value of recursive query parameters
     * 
     * @return returns @c true if request is sent successfully @n
     * @c false in case of any error
     * 
     */
    public void setRecursive(boolean recursive) {
        setRecursive(nativeResource, recursive);
    }

    /**
     * Builds and configures resource path using template path. Template path
     * will use path templates and values of these path templates will be passed
     * to this function as const char* parameters \n
     * 
     * Example template path : /devices/{deviceID}/information. \n In above
     * example {deviceID} is path template
     * 
     * to build /device/0/information, invoke this function as below \n
     * buildTemplatePath("/device/{deviceID}/information", "0") ;
     * 
     * @param pathTemplate
     *            resource path with path templates
     * 
     * @see Sec::Cm::Client::Resource::Resource::buildTemplatePath
     * @see Sec::Cm::Client::Resource::Resource::setResourcePath
     */
    public boolean buildTemplatePath(String path, String... templateParameters) {
        String[] tokens = path.split("\\/");

        int currentTokenCount = 0;

        String resourcePath = "";

        for (int index = 0; index < tokens.length; index++) {
            String s = tokens[index];

            if (s.length() == 0) {
                continue;
            }

            if (s.startsWith("{") && s.endsWith("}")) {
                if (currentTokenCount < templateParameters.length) {
                    resourcePath += ("/" + templateParameters[currentTokenCount]);
                    currentTokenCount++;
                } else {
                    return false;
                }
            } else {
                resourcePath += ("/" + s);
            }
        }

        return setResourcePath(resourcePath);
    }

    /**
     * configures resource relative path
     * 
     * @param resourcePath
     *            relative path of this resource
     * 
     * 
     */
    public boolean setResourcePath(String resourcePath) {
        return setResourcePath(nativeResource, resourcePath);
    }
}
