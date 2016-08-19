
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

import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.concurrent.CountDownLatch;

import org.iotivity.cloud.accountserver.db.TokenTable;
import org.iotivity.cloud.accountserver.db.UserTable;
import org.iotivity.cloud.accountserver.util.TypeCastingManager;
import org.iotivity.cloud.base.ServerSystem;
import org.iotivity.cloud.base.ServerSystem.PersistentPacketReceiver;
import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.device.Device;
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
import org.iotivity.cloud.util.Log;
import org.junit.Before;
import org.junit.Test;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;

import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelId;
import io.netty.util.Attribute;

public class GroupResourceTest {
    private static final String            GROUP_URI                 = "/.well-known/ocf/acl/group";
    private String                         di                        = "d0001";
    private String                         accessToken               = "5f5536c896da7dd437316585b86ef9dd03441c40";
    private String                         refreshToken              = "rt0001";
    private String                         gid                       = null;
    private TypeCastingManager<UserTable>  mUserTableCastingManager  = new TypeCastingManager<>();
    private TypeCastingManager<TokenTable> mTokenTableCastingManager = new TypeCastingManager<>();
    ServerSystem                           serverSystem              = new ServerSystem();
    final CountDownLatch                   latch                     = new CountDownLatch(
            1);
    private String                         uuid                      = "bc38f243-aab5-44d3-8eb9-4a54ebbaf359";
    private String                         authProvider              = "github";
    private CoapDevice                     mockDevice                = Mockito
            .mock(CoapDevice.class);
    private Cbor<HashMap<String, Object>>  mCbor                     = new Cbor<>();
    IResponse                              res                       = null;
    IResponse                              resObserver               = null;

    @Mock
    ChannelHandlerContext                  ctx;

    @InjectMocks
    PersistentPacketReceiver               receiver                  = serverSystem.new PersistentPacketReceiver();

    @Before
    public void setUp() throws Exception {
        // create log file
        Log.createfile();
        // add resource
        serverSystem.addResource(new GroupResource());
        MockitoAnnotations.initMocks(this);
        ChannelId channelId = mock(ChannelId.class);
        // inject mocked coapDevice into the api
        Channel channel = mock(Channel.class);
        Attribute<Device> attribute = mock(Attribute.class);
        Mockito.doReturn(channel).when(ctx).channel();
        Mockito.doReturn(attribute).when(channel).attr(Mockito.any());
        Mockito.doReturn(mockDevice).when(attribute).get();
        Mockito.doReturn(channelId).when(channel).id();
        Mockito.doReturn(
                "0000000141f3edcfc2c3000000000001d0000000000000000000000000000000000000")
                .when(channelId).asLongText();
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapResponse answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapResponse resp = (CoapResponse) args[0];
                System.out.println(
                        "\t----------payload : " + resp.getPayloadString());
                System.out.println("\t---------method : " + resp.getStatus());
                res = resp;
                HashMap<String, Object> payloadData = mCbor
                        .parsePayloadFromCbor(resp.getPayload(), HashMap.class);
                if (payloadData.containsKey("gid")) {
                    gid = (String) payloadData.get("gid");
                }
                latch.countDown();
                return resp;
            }
        }).when(mockDevice).sendResponse(Mockito.anyObject());
    }

    @Test
    public void testCreateGroup() throws Exception {
        System.out.println("\t--------------Create Group Test------------");
        createGroup(ctx, uuid + "create", "Public");
        assertTrue(methodCheck(res, ResponseStatus.CHANGED));
        assertTrue(hashmapCheck(res, "gid"));

    }

    @Test
    public void testFindMyGroup() throws Exception {
        System.out.println("\t--------------Find My Group Test------------");
        String uuid = this.uuid + "find";
        createGroup(ctx, uuid, "Public");
        findGroup(ctx, uuid);
        assertTrue(methodCheck(res, ResponseStatus.CONTENT));
        assertTrue(hashmapCheck(res, "gidlist"));
    }

    @Test
    public void testDeleteGroup() throws Exception {
        System.out.println("\t--------------Delete Group Test------------");
        String uuid = this.uuid + "delete";
        createGroup(ctx, uuid, "Public");
        deleteGroup(ctx, uuid);
        System.out.println("\t--------res : " + res.getStatus());
        assertTrue(methodCheck(res, ResponseStatus.DELETED));
    }

    @Test
    public void testJoinToinvitedGroup() throws Exception {
        System.out.println(
                "\t--------------Join to invited group Test------------");
        String uuid = this.uuid + "join";
        createGroup(ctx, uuid, "Public");
        joinGroup(ctx, "u0002");
        assertTrue(methodCheck(res, ResponseStatus.CHANGED));
    }

    @Test
    public void testObserveGroup() throws Exception {
        System.out.println("\t--------------Observe Group Test------------");
        String uuid = this.uuid + "obs";
        createGroup(ctx, uuid, "Public");
        observeGroup(ctx, uuid);
        assertTrue(methodCheck(res, ResponseStatus.CONTENT));
        assertTrue(hashmapCheck(res, "gid"));
        assertTrue(hashmapCheck(res, "gmid"));
        assertTrue(hashmapCheck(res, "midlist"));
        assertTrue(hashmapCheck(res, "dilist"));
    }

    @Test
    public void testShareDeviceIntoGroup() throws Exception {
        System.out.println(
                "\t--------------Share device into group Test------------");
        String uuid = this.uuid + "share";
        createGroup(ctx, uuid, "Public");
        shareDevice(ctx, "d0002");
        assertTrue(methodCheck(res, ResponseStatus.CHANGED));
    }

    @Test
    public void testShareDeviceNotification() throws Exception {
        System.out.println(
                "\t--------------Share device Notification into group Test------------");
        String uuid = this.uuid + "share";
        String uuidGuest = "u0002guest";
        ChannelHandlerContext ctxGuest = mock(ChannelHandlerContext.class);
        ChannelId channelIdGuest = mock(ChannelId.class);
        CoapDevice mockDeviceGuest = mock(CoapDevice.class);
        // inject mocked coapDevice into the api
        Channel channelGuest = mock(Channel.class);
        Attribute<Device> attributeGuest = mock(Attribute.class);
        Mockito.doReturn(channelGuest).when(ctxGuest).channel();
        Mockito.doReturn(attributeGuest).when(channelGuest).attr(Mockito.any());
        Mockito.doReturn(mockDeviceGuest).when(attributeGuest).get();
        Mockito.doReturn(channelIdGuest).when(channelGuest).id();
        Mockito.doReturn(
                "0000000141f3edcfc2c3000000000001d0000000000000000000000000000000000001")
                .when(channelIdGuest).asLongText();
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
                resObserver = resp;
                return resp;
            }
        }).when(mockDeviceGuest).sendResponse(Mockito.anyObject());
        createGroup(ctx, uuid, "Public");
        joinGroup(ctxGuest, uuidGuest);
        observeGroup(ctxGuest, uuidGuest);
        shareDevice(ctx, "d0002");
        // assertion for the group master
        assertTrue(methodCheck(res, ResponseStatus.CHANGED));
        // assertion for the observer
        assertTrue(methodCheck(resObserver, ResponseStatus.CONTENT));

    }

    public void shareDevice(ChannelHandlerContext ctx, String deviceId)
            throws Exception {
        System.out.println("-----Share Device");
        IRequest request = null;
        request = createShareDeviceRequest(deviceId);
        receiver.channelRead(ctx, request);
    }

    public IRequest createShareDeviceRequest(String deviceId) {
        IRequest request = null;
        HashMap<String, Object> payloadData = new HashMap<String, Object>();
        ArrayList<String> diList = new ArrayList<>();
        diList.add(deviceId);
        payloadData.put("dilist", diList);
        Cbor<HashMap<String, Object>> cbor = new Cbor<HashMap<String, Object>>();
        request = MessageBuilder.createRequest(RequestMethod.POST,
                GROUP_URI + "/" + gid, null, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        return request;
    }

    public void createGroup(ChannelHandlerContext ctx, String gmid,
            String gtype) throws Exception {
        System.out.println("-----Create Group");
        IRequest request = null;
        request = createGroupRequest(gmid, gtype);
        receiver.channelRead(ctx, request);
    }

    public void findGroup(ChannelHandlerContext ctx, String uuid)
            throws Exception {
        System.out.println("-----Find Group");
        IRequest request = null;
        request = findGroupRequest(uuid);
        receiver.channelRead(ctx, request);
    }

    public void observeGroup(ChannelHandlerContext ctx, String uuid)
            throws Exception {
        System.out.println("-----Observe Group");
        IRequest request = null;
        request = observeGroupRequest(uuid);
        receiver.channelRead(ctx, request);
    }

    public void joinGroup(ChannelHandlerContext ctx, String uuid)
            throws Exception {
        System.out.println("-----Join Group");
        IRequest request = null;
        request = joinGroupRequest(uuid);
        receiver.channelRead(ctx, request);
    }

    public void deleteGroup(ChannelHandlerContext ctx, String uuid)
            throws Exception {
        System.out.println("-----Delete Group");
        IRequest request = null;
        request = deleteGroupRequest(uuid);
        receiver.channelRead(ctx, request);
    }

    public IRequest deleteGroupRequest(String uuid) {
        IRequest request = null;
        request = MessageBuilder.createRequest(RequestMethod.DELETE, GROUP_URI,
                "gid=" + gid + ";" + "gmid=" + uuid);
        return request;
    }

    public IRequest findGroupRequest(String uuid) {
        IRequest request = null;
        request = MessageBuilder.createRequest(RequestMethod.GET, GROUP_URI,
                "mid=" + uuid);
        return request;
    }

    public IRequest observeGroupRequest(String uuid) {
        IRequest request = null;
        request = MessageBuilder.createRequest(RequestMethod.GET,
                GROUP_URI + "/" + gid, "mid=" + uuid);
        ((CoapRequest) request).setObserve(Observe.SUBSCRIBE);
        return request;
    }

    public IRequest joinGroupRequest(String uuid) {
        IRequest request = null;
        HashMap<String, Object> payloadData = new HashMap<String, Object>();
        ArrayList<String> midList = new ArrayList<>();
        midList.add(uuid);
        payloadData.put("midlist", midList);
        Cbor<HashMap<String, Object>> cbor = new Cbor<HashMap<String, Object>>();
        request = MessageBuilder.createRequest(RequestMethod.POST,
                GROUP_URI + "/" + gid, null, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        return request;
    }

    public IRequest createGroupRequest(String uuid, String gtype) {
        IRequest request = null;
        HashMap<String, Object> payloadData = new HashMap<String, Object>();
        payloadData.put("gmid", uuid);
        payloadData.put("gtype", gtype);
        Cbor<HashMap<String, Object>> cbor = new Cbor<HashMap<String, Object>>();
        request = MessageBuilder.createRequest(RequestMethod.POST, GROUP_URI,
                null, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        return request;
    }

    public boolean hashmapCheck(IResponse response, String propertyName) {
        HashMap<String, Object> payloadData = mCbor
                .parsePayloadFromCbor(response.getPayload(), HashMap.class);
        if (payloadData.containsKey(propertyName))
            return true;
        else
            return false;
    }

    public boolean methodCheck(IResponse response,
            ResponseStatus responseStatus) {
        if (responseStatus == response.getStatus())
            return true;
        else
            return false;
    }

    private TokenTable makeTokenTable() {
        TokenTable tokenInfo = new TokenTable();
        tokenInfo.setUuid(uuid);
        tokenInfo.setDid(di);
        tokenInfo.setAccesstoken(accessToken);
        tokenInfo.setRefreshtoken(refreshToken);
        tokenInfo.setProvider(authProvider);
        tokenInfo.setExpiredtime(-1);
        Date currentTime = new Date();
        DateFormat transFormat = new SimpleDateFormat("yyyyMMddkkmm");
        tokenInfo.setIssuedtime(transFormat.format(currentTime));
        return tokenInfo;
    }

    private UserTable makeUserTable() {
        UserTable userInfo = new UserTable();
        userInfo.setUuid(uuid);
        userInfo.setProvider(authProvider);
        userInfo.setUserid("userId");
        return userInfo;
    }
}
