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

package org.iotivity.service.ca.test.helper;

import android.content.Context;
import android.app.Activity;

public class RMInterface {

	static {
		System.loadLibrary("RMInterface");
	}

	private org.iotivity.service.ca.test.helper.MainActivity mResponseListener = null;

	public native void setNativeResponseListener(Object listener);

	public native boolean setContext(Context context, Activity activity);

	public native boolean initialize();

	public native void terminate();

	public native boolean startListeningServer();

	public native boolean stopListeningServer(int expectedResult);

	public native boolean startDiscoveryServer();

	public native boolean registerServerHandler();

	public native boolean registerClientHandler();

	public native boolean selectNetwork();

	public native boolean selectCustomNetwork(int interestedNetwork);

	public native boolean unSelectNetwork();

	public native boolean unSelectCustomNetwork(int uninterestedNetwork);

	public native boolean getNetworkInfomation(boolean positive);

	public native boolean generateTokenAndDestroyToken();

	public native boolean startListeningServerCustom(int expectedResult);

	public native boolean generateCustomToken(int targetARG, int expectedResult);

	public native boolean selectNetworkWithExpectedResult(long networkType,
			int expectedResult);

	public native boolean unSelectNetworkWithExpectedResult(long networkType,
			int expectedResult);

	public native boolean startDiscoveryWithExpectedResult(int expectedResult);

	public native boolean getNetworkInfomationWithExpectedResult(int targetARG,
			int expectedResult);

	public native boolean createEndpointWithExpectedResult(int targetARG,
			int expectedResult);

	public native boolean destroyEndpointInvalidEndpoint();

	public native boolean handleRequestResponse();

	public native boolean createEndpoint();

	public native boolean destroyEndpoint();

	public native boolean generateToken();

	public native boolean destroyToken();

	public native boolean initNetwork();

	public native boolean initServerNetwork();

	public native boolean initClientNetwork();

	public native boolean initServerWithoutNetwork();

	public native boolean initClientWithoutNetwork();

	public native boolean checkReceiveCount(int count);

	public native void registerServerHandler(int requestHandler,
			int responseHandler);

	public native void registerClientHandler(int requestHandler,
			int responseHandler);

	public native void setupTestCase(int intoutType, int messageDataType,
			int communicationType);

	public native boolean setDtls();

	public native boolean setCredentials();

	public native int getSecurePort();

	public native int getNonsecurePort();

	public native void setNonsecurePort();

	public native void setRemoteIp(String ip);

	public native void setRemoteAddressSet(String[] addressSet);

	public native void setNetwork(int network);

	public native boolean sendConfigurationRequest(int taskType, int msgType,
			int method);

	public native boolean sendRequest(int uriType, int payloadType,
			int msgType, int methodType, int numOfHeader, boolean isSecured);

	public native boolean sendRequestToAll(int uriType, int payloadType,
			int msgType, int methodType, int numOfHeader);

	public native boolean sendResponse(int uriType, int payloadType,
			int msgType, int responseValue, int numOfHeader);

	public native boolean establishConnectionWithServer();

	public void setResponseListener(
			org.iotivity.service.ca.test.helper.MainActivity listener) {
		mResponseListener = listener;
		setNativeResponseListener(this);
	}

	private void OnResponseReceived(String subject, String receivedData) {
		if (null != mResponseListener) {
			mResponseListener.OnResponseReceived(subject, receivedData);
		}
	}

	public void sleep(int seconds) {
		try {
			Thread.sleep(seconds * 1000);
		} catch (Exception ex) {

		}
	}
}
