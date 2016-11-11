
/*
 * //******************************************************************
 * //
 * // Copyright 2016 Samsung Electronics All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */
package org.iotivity.cloud.accountserver.resources.acl.group;

import static java.util.concurrent.TimeUnit.SECONDS;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.CountDownLatch;

import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.accountserver.db.MongoDB;
import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.exception.ServerException;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.Observe;
import org.iotivity.cloud.base.protocols.enums.RequestMethod;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.util.Cbor;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;

public class GroupResourceTest {
    private static final String           GROUP_URI         = Constants.GROUP_FULL_URI;
    private final String                  mUserUuid         = "bc38f243-aab5-44d3-8eb9-4a54ebbaf359";
    private String                        mGroupId          = null;
    final CountDownLatch                  mLatch            = new CountDownLatch(
            1);
    private CoapDevice                    mMockDevice       = Mockito
            .mock(CoapDevice.class);
    private Cbor<HashMap<String, Object>> mCbor             = new Cbor<>();
    private IResponse                     mResponse         = null;
    private IResponse                     mResponseObserver = null;
    private GroupResource                 mGroupResource    = new GroupResource();

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.initMocks(this);
        // reset data base
        resetDB();
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapResponse answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapResponse resp = (CoapResponse) args[0];
                System.out.println(
                        "\t----------payload : " + resp.getPayloadString());
                System.out.println("\t---------method : " + resp.getStatus());
                mResponse = resp;
                if (mGroupId == null) {
                    HashMap<String, Object> payloadData = mCbor
                            .parsePayloadFromCbor(resp.getPayload(),
                                    HashMap.class);
                    if (payloadData.containsKey("gid")) {
                        mGroupId = (String) payloadData.get("gid");
                    }
                }
                mLatch.countDown();
                return null;
            }
        }).when(mMockDevice).sendResponse(Mockito.anyObject());
    }

    @After
    public void resetAccountDatabase() throws Exception {
        MongoDB mongoDB = new MongoDB(Constants.DB_NAME);
        mongoDB.createTable(Constants.USER_TABLE);
        mongoDB.createTable(Constants.TOKEN_TABLE);
        mongoDB.createTable(Constants.GROUP_TABLE);
    }

    @Test
    public void testCreateGroup() throws Exception {
        getTestMethodName();
        createGroup(mMockDevice, mUserUuid + "create", "Public");
        assertTrue(methodCheck(mResponse, ResponseStatus.CHANGED));
        assertTrue(hashmapCheck(mResponse, "gid"));
        assertTrue(mLatch.await(2L, SECONDS));
    }

    @Test
    public void testFindMyGroup() throws Exception {
        getTestMethodName();
        String uuid = this.mUserUuid + "find";
        createGroup(mMockDevice, uuid, "Public");
        findGroup(mMockDevice, uuid);
        assertTrue(methodCheck(mResponse, ResponseStatus.CONTENT));
        assertTrue(hashmapCheck(mResponse, "gidlist"));
        assertTrue(mLatch.await(2L, SECONDS));
    }

    @Test(expected = ServerException.BadRequestException.class)
    public void testCreateGroupNotSupportedType() throws Exception {
        getTestMethodName();
        String uuid = this.mUserUuid + "delete";
        createGroup(mMockDevice, uuid, "NotSupported");
        deleteGroup(mMockDevice, uuid);
    }

    @Test
    public void testDeleteGroup() throws Exception {
        getTestMethodName();
        String uuid = this.mUserUuid + "delete";
        createGroup(mMockDevice, uuid, "Public");
        deleteGroup(mMockDevice, uuid);
        assertTrue(methodCheck(mResponse, ResponseStatus.DELETED));
        assertTrue(mLatch.await(2L, SECONDS));
    }

    @Test(expected = ServerException.PreconditionFailedException.class)
    public void testDeleteGroupWithoutGid() throws Exception {
        getTestMethodName();
        String uuid = this.mUserUuid + "delete";
        createGroup(mMockDevice, uuid, "Public");
        deleteGroupWithoutGid(mMockDevice, uuid);
    }

    @Test
    public void testAddDeviceAndUserToGroup() throws Exception {
        getTestMethodName();
        String uuid = this.mUserUuid + "AddDeviceAndUser";
        createGroup(mMockDevice, uuid, "Public");
        ArrayList<String> midList = new ArrayList<>();
        midList.add("member0001");
        midList.add("member0002");
        ArrayList<String> diList = new ArrayList<>();
        diList.add("device0001");
        diList.add("device0002");
        addDeviceAndUser(mMockDevice, midList, diList);
    }

    @Test
    public void testDeleteDeviceAndUserToGroup() throws Exception {
        getTestMethodName();
        String uuid = this.mUserUuid + "AddDeviceAndUser";
        createGroup(mMockDevice, uuid, "Public");
        ArrayList<String> midList = new ArrayList<>();
        midList.add("member0001");
        midList.add("member0002");
        ArrayList<String> diList = new ArrayList<>();
        diList.add("device0001");
        diList.add("device0002");
        addDeviceAndUser(mMockDevice, midList, diList);
        deleteDeviceAndUser(mMockDevice, midList, diList);
    }

    @Test(expected = ServerException.BadRequestException.class)
    public void testDeleteGroupInvalidGmid() throws Exception {
        getTestMethodName();
        String uuid = this.mUserUuid + "delete";
        createGroup(mMockDevice, uuid, "Public");
        deleteGroup(mMockDevice, uuid + "invlidGmid");
    }

    @Test
    public void testJoinToinvitedGroup() throws Exception {
        getTestMethodName();
        String uuid = this.mUserUuid + "join";
        createGroup(mMockDevice, uuid, "Public");
        joinGroup(mMockDevice, "u0002");
        assertTrue(methodCheck(mResponse, ResponseStatus.CHANGED));
        assertTrue(mLatch.await(2L, SECONDS));
    }

    @Test
    public void testObserveGroup() throws Exception {
        getTestMethodName();
        String uuid = this.mUserUuid + "obs";
        createGroup(mMockDevice, uuid, "Public");
        observeGroup(mMockDevice, uuid);
        assertTrue(methodCheck(mResponse, ResponseStatus.CONTENT));
        assertTrue(hashmapCheck(mResponse, "gid"));
        assertTrue(hashmapCheck(mResponse, "gmid"));
        assertTrue(hashmapCheck(mResponse, "midlist"));
        assertTrue(hashmapCheck(mResponse, "dilist"));
        assertTrue(mLatch.await(2L, SECONDS));
    }

    @Test
    public void testObserveDeregisterGroup() throws Exception {
        getTestMethodName();
        String uuid = this.mUserUuid + "obs";
        createGroup(mMockDevice, uuid, "Public");
        observeGroup(mMockDevice, uuid);
        observeDeregisterGroup(mMockDevice, uuid);
        assertTrue(methodCheck(mResponse, ResponseStatus.CONTENT));
    }

    @Test
    public void testShareDeviceIntoGroup() throws Exception {
        getTestMethodName();
        String uuid = this.mUserUuid + "share";
        createGroup(mMockDevice, uuid, "Public");
        shareDevice(mMockDevice, "d0002");
        assertTrue(methodCheck(mResponse, ResponseStatus.CHANGED));
        assertTrue(mLatch.await(2L, SECONDS));
    }

    @Test
    public void testShareDeviceNotification() throws Exception {
        getTestMethodName();
        String uuid = this.mUserUuid + "share";
        String uuidGuest = "u0002guest";
        CoapDevice mockDeviceGuest = mock(CoapDevice.class);
        CountDownLatch memberLatch = new CountDownLatch(1);
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapResponse answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapResponse resp = (CoapResponse) args[0];
                System.out.println("\t----------new member payload : "
                        + resp.getPayloadString());
                System.out.println(
                        "\t---------new member method : " + resp.getStatus());
                memberLatch.countDown();
                mResponseObserver = resp;
                return resp;
            }
        }).when(mockDeviceGuest).sendResponse(Mockito.anyObject());
        createGroup(mMockDevice, uuid, "Public");
        joinGroup(mockDeviceGuest, uuidGuest);
        observeGroup(mockDeviceGuest, uuidGuest);
        shareDevice(mMockDevice, "d0002");
        // assertion for the group master
        assertTrue(methodCheck(mResponse, ResponseStatus.CHANGED));
        // assertion for the observer
        assertTrue(methodCheck(mResponseObserver, ResponseStatus.CONTENT));
        assertTrue(mLatch.await(2L, SECONDS));
        assertTrue(memberLatch.await(2L, SECONDS));
    }

    public void addDeviceAndUser(CoapDevice device, ArrayList<String> midList,
            ArrayList<String> diList) throws Exception {
        HashMap<String, Object> payloadData = new HashMap<>();
        payloadData.put("midlist", midList);
        payloadData.put("dilist", diList);
        IRequest request = MessageBuilder.createRequest(RequestMethod.POST,
                GROUP_URI + "/" + mGroupId, null,
                ContentFormat.APPLICATION_CBOR,
                mCbor.encodingPayloadToCbor(payloadData));
        System.out.println("-----add Device and User : " + payloadData);
        mGroupResource.onDefaultRequestReceived(device, request);
    }

    public void deleteDeviceAndUser(CoapDevice device,
            ArrayList<String> midList, ArrayList<String> diList)
            throws Exception {
        String midListString = "";
        String didListString = "";
        for (String mid : midList) {
            midListString += "midlist=";
            midListString += mid;
            midListString += ";";
        }
        for (String di : diList) {
            didListString += "dilist=";
            didListString += di;
            didListString += ";";
        }
        System.out.println("-----delete Device and User, Query: "
                + midListString + didListString);
        IRequest request = MessageBuilder.createRequest(RequestMethod.DELETE,
                GROUP_URI + "/" + mGroupId, midListString + didListString);
        mGroupResource.onDefaultRequestReceived(device, request);
    }

    private void shareDevice(CoapDevice device, String deviceId)
            throws Exception {
        System.out.println("-----Share Device");
        IRequest request = null;
        request = createShareDeviceRequest(deviceId);
        mGroupResource.onDefaultRequestReceived(device, request);
    }

    private void createGroup(CoapDevice device, String gmid, String gtype)
            throws Exception {
        System.out.println("-----Create Group");
        IRequest request = null;
        request = createGroupRequest(gmid, gtype);
        mGroupResource.onDefaultRequestReceived(device, request);
    }

    private void findGroup(CoapDevice device, String uuid) throws Exception {
        System.out.println("-----Find Group");
        IRequest request = null;
        request = findGroupRequest(uuid);
        mGroupResource.onDefaultRequestReceived(device, request);
    }

    private void observeDeregisterGroup(CoapDevice device, String uuid)
            throws Exception {
        System.out.println("-----Observe Deregister Group");
        IRequest request = null;
        request = observeDeregisterGroupRequest(uuid);
        mGroupResource.onDefaultRequestReceived(device, request);
    }

    private void observeGroup(CoapDevice device, String uuid) throws Exception {
        System.out.println("-----Observe Group");
        IRequest request = null;
        request = observeGroupRequest(uuid);
        mGroupResource.onDefaultRequestReceived(device, request);
    }

    private void joinGroup(CoapDevice device, String uuid) throws Exception {
        System.out.println("-----Join Group");
        IRequest request = null;
        request = joinGroupRequest(uuid);
        mGroupResource.onDefaultRequestReceived(device, request);
    }

    private void deleteGroup(CoapDevice device, String uuid) throws Exception {
        System.out.println("-----Delete Group");
        IRequest request = null;
        request = deleteGroupRequest(uuid);
        mGroupResource.onDefaultRequestReceived(device, request);
    }

    private void deleteGroupWithoutGid(CoapDevice device, String uuid)
            throws Exception {
        System.out.println("-----Delete Group");
        IRequest request = null;
        request = deleteGroupWithoutGidRequest(uuid);
        mGroupResource.onDefaultRequestReceived(device, request);
    }

    private IRequest deleteGroupWithoutGidRequest(String uuid) {
        IRequest request = null;
        request = MessageBuilder.createRequest(RequestMethod.DELETE, GROUP_URI,
                "gmid=" + uuid);
        return request;
    }

    private IRequest deleteGroupRequest(String uuid) {
        IRequest request = null;
        request = MessageBuilder.createRequest(RequestMethod.DELETE, GROUP_URI,
                "gid=" + mGroupId + ";" + "gmid=" + uuid);
        return request;
    }

    private IRequest findGroupRequest(String uuid) {
        IRequest request = null;
        request = MessageBuilder.createRequest(RequestMethod.GET, GROUP_URI,
                "mid=" + uuid);
        return request;
    }

    private IRequest createShareDeviceRequest(String deviceId) {
        IRequest request = null;
        HashMap<String, Object> payloadData = new HashMap<String, Object>();
        ArrayList<String> diList = new ArrayList<>();
        diList.add(deviceId);
        payloadData.put("dilist", diList);
        request = MessageBuilder.createRequest(RequestMethod.POST,
                GROUP_URI + "/" + mGroupId, null,
                ContentFormat.APPLICATION_CBOR,
                mCbor.encodingPayloadToCbor(payloadData));
        return request;
    }

    private IRequest observeDeregisterGroupRequest(String uuid) {
        IRequest request = null;
        request = MessageBuilder.createRequest(RequestMethod.GET,
                GROUP_URI + "/" + mGroupId, "mid=" + uuid);
        ((CoapRequest) request).setObserve(Observe.UNSUBSCRIBE);
        return request;
    }

    private IRequest observeGroupRequest(String uuid) {
        IRequest request = null;
        request = MessageBuilder.createRequest(RequestMethod.GET,
                GROUP_URI + "/" + mGroupId, "mid=" + uuid);
        ((CoapRequest) request).setObserve(Observe.SUBSCRIBE);
        return request;
    }

    private IRequest joinGroupRequest(String uuid) {
        IRequest request = null;
        HashMap<String, Object> payloadData = new HashMap<String, Object>();
        ArrayList<String> midList = new ArrayList<>();
        midList.add(uuid);
        payloadData.put("midlist", midList);
        request = MessageBuilder.createRequest(RequestMethod.POST,
                GROUP_URI + "/" + mGroupId, null,
                ContentFormat.APPLICATION_CBOR,
                mCbor.encodingPayloadToCbor(payloadData));
        return request;
    }

    private IRequest createGroupRequest(String uuid, String gtype) {
        IRequest request = null;
        HashMap<String, Object> payloadData = new HashMap<String, Object>();
        payloadData.put("gmid", uuid);
        payloadData.put("gtype", gtype);
        request = MessageBuilder.createRequest(RequestMethod.POST, GROUP_URI,
                null, ContentFormat.APPLICATION_CBOR,
                mCbor.encodingPayloadToCbor(payloadData));
        return request;
    }

    private boolean hashmapCheck(IResponse response, String propertyName) {
        HashMap<String, Object> payloadData = mCbor
                .parsePayloadFromCbor(response.getPayload(), HashMap.class);
        if (payloadData.containsKey(propertyName))
            return true;
        else
            return false;
    }

    private boolean methodCheck(IResponse response,
            ResponseStatus responseStatus) {
        if (responseStatus == response.getStatus())
            return true;
        else
            return false;
    }

    private void getTestMethodName() {
        StackTraceElement[] stacks = new Throwable().getStackTrace();
        StackTraceElement currentStack = stacks[1];
        System.out.println("\t---Test Name : " + currentStack.getMethodName());
    }

    public void resetDB() throws Exception {
        MongoDB mongoDB = new MongoDB(Constants.DB_NAME);
        mongoDB.deleteTable(Constants.GROUP_TABLE);
        mongoDB.createTable(Constants.GROUP_TABLE);
        mongoDB.deleteTable(Constants.USER_TABLE);
        mongoDB.createTable(Constants.USER_TABLE);
        mongoDB.deleteTable(Constants.TOKEN_TABLE);
        mongoDB.createTable(Constants.TOKEN_TABLE);
    }
}
