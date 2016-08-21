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

package org.iotivity.cloud.accountserver.resources.account;

import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.concurrent.CountDownLatch;

import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.accountserver.db.AccountDBManager;
import org.iotivity.cloud.accountserver.db.TokenTable;
import org.iotivity.cloud.accountserver.db.UserTable;
import org.iotivity.cloud.accountserver.resources.acl.group.GroupResource;
import org.iotivity.cloud.accountserver.util.TypeCastingManager;
import org.iotivity.cloud.base.ServerSystem;
import org.iotivity.cloud.base.ServerSystem.PersistentPacketReceiver;
import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.RequestMethod;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.util.Cbor;
import org.iotivity.cloud.util.Log;
import org.junit.After;
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

public class AccountResourceTest {
    private static final String            REGISTER_URI              = "/.well-known/ocf/account";
    private static final String            GROUP_URI                 = "/.well-known/ocf/acl/group";
    private String                         di                        = "B371C481-38E6-4D47-8320-7688D8A5B58C";
    private String                         authProvider              = "Github";
    private String                         authCode                  = "7af49183966f84e553dc";
    String                                 userId                    = "testuser";
    private CoapDevice                     mockDevice                = mock(
            CoapDevice.class);
    private Cbor<HashMap<String, Object>>  mCbor                     = new Cbor<>();
    IResponse                              res                       = null;
    ServerSystem                           serverSystem              = new ServerSystem();
    final CountDownLatch                   latch                     = new CountDownLatch(
            1);
    private TypeCastingManager<UserTable>  mUserTableCastingManager  = new TypeCastingManager<>();
    private TypeCastingManager<TokenTable> mTokenTableCastingManager = new TypeCastingManager<>();

    @Mock
    ChannelHandlerContext                  ctx;

    @InjectMocks
    PersistentPacketReceiver               receiver                  = serverSystem.new PersistentPacketReceiver();

    @Before
    public void setUp() throws Exception {
        // create log file
        Log.createfile();
        // add resource
        serverSystem.addResource(new AccountResource());
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
                        "\t----Exception responsestatus : " + resp.getStatus());
                res = resp;
                latch.countDown();
                return null;
            }
        }).when(channel).writeAndFlush(Mockito.any());
        // callback mock
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapResponse answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapResponse resp = (CoapResponse) args[0];
                System.out
                        .println("\t----payload : " + resp.getPayloadString());
                System.out
                        .println("\t----responsestatus : " + resp.getStatus());
                res = resp;
                return resp;
            }
        }).when(mockDevice).sendResponse(Mockito.anyObject());
        receiver.channelActive(ctx);
    }

    @After
    public void end() throws Exception {
        receiver.channelInactive(ctx);
    }

    @Test
    public void testSignUpOnDefaultRequestReceived() throws Exception {
        System.out.println("\t--------------Sign Up Test------------");
        signUp(di, authProvider, authCode);
        assertTrue(methodCheck(res, ResponseStatus.CHANGED));
        assertTrue(hashmapCheck(res, "accesstoken"));
        assertTrue(hashmapCheck(res, "refreshtoken"));
        assertTrue(hashmapCheck(res, "tokentype"));
        assertTrue(hashmapCheck(res, "uid"));
    }

    @Test
    public void testSignUpwithWrongAuthCodeOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Sign Up with Wrong AuthCode Test------------");
        String wrongAuthCode = "5fc4fcf376f8d7087a3a";
        signUp(di, authProvider, wrongAuthCode);
        assertTrue(methodCheck(res, ResponseStatus.INTERNAL_SERVER_ERROR));
    }

    @Test
    public void testSignUpNullAuthProviderOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Sign Up Null AuthProvider Test------------");
        signUp(di, null, authCode);
        assertTrue(methodCheck(res, ResponseStatus.PRECONDITION_FAILED));
    }

    @Test
    public void testSignUpNullAuthCodeOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Sign Up with Null AuthCode Test------------");
        signUp(di, authProvider, null);
        assertTrue(methodCheck(res, ResponseStatus.PRECONDITION_FAILED));
    }

    @Test
    public void testSignUpNullDiOnDefaultRequestReceived() throws Exception {
        System.out.println(
                "\t--------------Sign Up with Null DeviceId Test------------");
        signUp(null, authProvider, authCode);
        assertTrue(methodCheck(res, ResponseStatus.PRECONDITION_FAILED));
    }

    @Test
    public void testSignUpInvalidMethodOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Sign Up with Invalid Method (GET) Test------------");
        signUpInvalidMethod(di, authProvider, authCode);
        assertTrue(methodCheck(res, ResponseStatus.BAD_REQUEST));
    }

    @Test
    public void testDeleteDeviceOnDefaultRequestReceived() throws Exception {
        System.out.println(
                "\t--------------Delete Device from all Groups Test------------");
        // register the token table and user table to the DB
        String uuid = "u0001";
        HashMap<String, Object> tokenInfo = mTokenTableCastingManager
                .convertObjectToMap(makeTokenTable(uuid));
        HashMap<String, Object> userInfo = mUserTableCastingManager
                .convertObjectToMap(makeUserTable(uuid));
        AccountDBManager.getInstance()
                .insertAndReplaceRecord(Constants.TOKEN_TABLE, tokenInfo);
        AccountDBManager.getInstance().insertRecord(Constants.USER_TABLE,
                userInfo);
        createDefaultGroup(uuid);
        shareDevice(ctx, di);
        deleteDevice(di);
        assertTrue(methodCheck(res, ResponseStatus.DELETED));
    }

    // @Test
    public void testGetUserInfoUsingUuidOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Sign Up with Invalid Method (GET) Test------------");
        // register the token table and user table to the DB
        String uuid = "u0001Get";
        HashMap<String, Object> tokenInfo = mTokenTableCastingManager
                .convertObjectToMap(makeTokenTable(uuid));
        HashMap<String, Object> userInfo = mUserTableCastingManager
                .convertObjectToMap(makeUserTable(uuid));
        AccountDBManager.getInstance()
                .insertAndReplaceRecord(Constants.TOKEN_TABLE, tokenInfo);
        AccountDBManager.getInstance().insertRecord(Constants.USER_TABLE,
                userInfo);
        getUserInfoUsingUuid(uuid);
        assertTrue(methodCheck(res, ResponseStatus.CONTENT));
    }

    public void getUserInfoUsingUuid(String uuid) throws Exception {
        System.out.println("-----Get User Info : " + uuid);
        IRequest request = null;
        request = getUserInfoUsingUuidRequest(uuid);
        receiver.channelRead(ctx, request);
    }

    public IRequest getUserInfoUsingUuidRequest(String uuid) {
        IRequest request = MessageBuilder.createRequest(RequestMethod.GET,
                REGISTER_URI, "uid=" + uuid);
        return request;
    }

    public void createDefaultGroup(String uuid) throws Exception {
        System.out.println("-----Create Default Group");
        IRequest request = null;
        request = createDefaultGroupRequest(uuid);
        receiver.channelRead(ctx, request);
    }

    public void deleteDevice(String di) throws Exception {
        System.out.println("-----Delete Device");
        IRequest request = null;
        request = deleteDeviceRequest(di);
        receiver.channelRead(ctx, request);
    }

    public void signUp(String di, String authProvider, String authCode)
            throws Exception {
        System.out.println("-----Sign Up");
        IRequest request = null;
        request = signUpRequest(di, authProvider, authCode);
        receiver.channelRead(ctx, request);
    }

    public void signUpInvalidMethod(String di, String authProvider,
            String authCode) throws Exception {
        System.out.println("-----Sign Up Invalid RequestMethod");
        IRequest request = null;
        request = signUpInvalidMethodRequest(di, authProvider, authCode);
        receiver.channelRead(ctx, request);
    }

    public void shareDevice(ChannelHandlerContext ctx, String deviceId)
            throws Exception {
        System.out.println("-----Share Device");
        IRequest request = null;
        request = createShareDeviceRequest(deviceId);
        receiver.channelRead(ctx, request);
    }

    public IRequest deleteDeviceRequest(String deviceId) {
        IRequest request = MessageBuilder.createRequest(RequestMethod.DELETE,
                REGISTER_URI, "di=" + deviceId + ";uid=u0001");
        return request;
    }

    public IRequest createDefaultGroupRequest(String uuid) {
        IRequest request = null;
        HashMap<String, String> payloadData = new HashMap<String, String>();
        payloadData.put("gmid", uuid);
        payloadData.put("gtype", "Private");
        Cbor<HashMap<String, String>> cbor = new Cbor<HashMap<String, String>>();
        request = MessageBuilder.createRequest(RequestMethod.POST, GROUP_URI,
                null, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        return request;
    }

    public IRequest createShareDeviceRequest(String deviceId) {
        IRequest request = null;
        HashMap<String, Object> payloadData = new HashMap<String, Object>();
        ArrayList<String> diList = new ArrayList<>();
        diList.add(deviceId);
        payloadData.put("dilist", diList);
        Cbor<HashMap<String, Object>> cbor = new Cbor<HashMap<String, Object>>();
        request = MessageBuilder.createRequest(RequestMethod.POST,
                GROUP_URI + "/" + "u0001", null, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        return request;
    }

    public IRequest signUpRequest(String deviceId, String authProvider,
            String authCode) {
        IRequest request = null;
        HashMap<String, String> payloadData = new HashMap<String, String>();
        payloadData.put("authcode", authCode);
        payloadData.put("authprovider", authProvider);
        payloadData.put("di", deviceId);
        Cbor<HashMap<String, String>> cbor = new Cbor<HashMap<String, String>>();
        request = MessageBuilder.createRequest(RequestMethod.POST, REGISTER_URI,
                null, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        return request;
    }

    public IRequest signUpInvalidMethodRequest(String deviceId,
            String authProvider, String authCode) {
        IRequest request = null;
        HashMap<String, String> payloadData = new HashMap<String, String>();
        payloadData.put("authcode", authCode);
        payloadData.put("authprovider", authProvider);
        payloadData.put("di", deviceId);
        Cbor<HashMap<String, String>> cbor = new Cbor<HashMap<String, String>>();
        request = MessageBuilder.createRequest(RequestMethod.GET, REGISTER_URI,
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

    private TokenTable makeTokenTable(String uuid) {
        TokenTable tokenInfo = new TokenTable();
        tokenInfo.setUuid(uuid);
        tokenInfo.setDid(di);
        tokenInfo.setAccesstoken("at0001");
        tokenInfo.setRefreshtoken("rt0001");
        tokenInfo.setProvider(authProvider);
        tokenInfo.setExpiredtime(-1);
        Date currentTime = new Date();
        DateFormat transFormat = new SimpleDateFormat("yyyyMMddkkmm");
        tokenInfo.setIssuedtime(transFormat.format(currentTime));
        return tokenInfo;
    }

    private UserTable makeUserTable(String uuid) {
        UserTable userInfo = new UserTable();
        userInfo.setUuid(uuid);
        userInfo.setProvider(authProvider);
        userInfo.setUserid("userId");
        return userInfo;
    }

}
