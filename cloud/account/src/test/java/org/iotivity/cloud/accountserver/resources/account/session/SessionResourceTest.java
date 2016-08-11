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

package org.iotivity.cloud.accountserver.resources.account.session;

import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.concurrent.CountDownLatch;

import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.accountserver.db.AccountDBManager;
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

public class SessionResourceTest {
    private final String                   DEFAULT_AUTH_LOGIN        = "/.well-known/ocf/account/session";
    private final String                   DEFAULT_AUTH_LOGOUT       = "/.well-known/ocf/account/session";
    private String                         di                        = "d0001";
    private String                         accessToken               = "5f5536c896da7dd437316585b86ef9dd03441c40";
    private String                         refreshToken              = "rt0001";
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

    @Mock
    ChannelHandlerContext                  ctx;

    @InjectMocks
    PersistentPacketReceiver               receiver                  = serverSystem.new PersistentPacketReceiver();

    @Before
    public void setUp() throws Exception {
        // create log file
        Log.createfile();
        // add resource
        serverSystem.addResource(new SessionResource());
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
        // register the token table and user table to the DB
        HashMap<String, Object> tokenInfo = mTokenTableCastingManager
                .convertObjectToMap(makeTokenTable());
        HashMap<String, Object> userInfo = mUserTableCastingManager
                .convertObjectToMap(makeUserTable());
        AccountDBManager.getInstance()
                .insertAndReplaceRecord(Constants.TOKEN_TABLE, tokenInfo);
        AccountDBManager.getInstance().insertRecord(Constants.USER_TABLE,
                userInfo);
        // response handler mock
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapResponse answer(InvocationOnMock invocation)
                    throws Throwable {

                Object[] args = invocation.getArguments();
                CoapResponse resp = (CoapResponse) args[0];
                System.out.println(
                        "\t----------payload : " + resp.getPayloadString());
                System.out.println("\t---------method : " + resp.getMethod());
                res = resp;
                latch.countDown();
                return resp;
            }
        }).when(mockDevice).sendResponse(Mockito.anyObject());
        // exception handler mock
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapResponse answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapResponse resp = (CoapResponse) args[0];
                System.out.println("\t----exception response status : "
                        + resp.getStatus());
                res = resp;
                latch.countDown();
                return null;
            }
        }).when(channel).writeAndFlush(Mockito.any());
    }

    @After
    public void endTest() {
        AccountDBManager.getInstance().deleteRecord(Constants.TOKEN_TABLE,
                mTokenTableCastingManager.convertObjectToMap(makeTokenTable()));
        AccountDBManager.getInstance().deleteRecord(Constants.USER_TABLE,
                mUserTableCastingManager.convertObjectToMap(makeUserTable()));
    }

    @Test
    public void testSignInOnDefaultRequestReceived() throws Exception {
        System.out.println("\t--------------Sign In Test------------");
        SignIn(di, accessToken);
        assertTrue(hashmapCheck(res, "expiresin"));
        assertTrue(methodCheck(res, ResponseStatus.CHANGED));
    }

    @Test
    public void testSignInWithInvalidMethodOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Sign In With Invalid RequestMethod Test------------");
        SignInWithInvalidMethod(di, accessToken);
        assertTrue(methodCheck(res, ResponseStatus.BAD_REQUEST));
    }

    @Test
    public void testWrongAccessTokenSignInOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Wrong AccessToken Sign In Test------------");
        String accessTokenNotStored = "5689c70ffa245effc563017fee36d250";
        // sign in request
        SignIn(di, accessTokenNotStored);
        assertTrue(methodCheck(res, ResponseStatus.UNAUTHORIZED));
    }

    @Test
    public void testInvalidDeviceIdSignInOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Invalid DeviceId Sign In Test------------");
        String diNotStored = "F371C481-38E6-4D47-8320-7688D8A5B58C";
        SignIn(diNotStored, accessToken);
        assertTrue(methodCheck(res, ResponseStatus.UNAUTHORIZED));
    }

    @Test
    public void testSignOutOnDefaultRequestReceived() throws Exception {
        System.out.println("\t--------------Sign Out Test------------");
        SignOut(di, accessToken);
        assertTrue(methodCheck(res, ResponseStatus.CHANGED));
    }

    @Test
    public void testSignInWithNullPayloadOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Sign In With Null Payload Test------------");
        SignInWithNullPayload();
        assertTrue(methodCheck(res, ResponseStatus.BAD_REQUEST));
    }

    @Test
    public void testSignOutWithNullPayloadOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Sign Out With Null Payload Test------------");
        // sign out request
        SignOutWithNullPayload();
        assertTrue(methodCheck(res, ResponseStatus.BAD_REQUEST));
    }

    @Test
    public void testSignInWithBlankHashMapPayloadOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Sign In With Blank HashMap Payload Test------------");
        SignInWithBlankHashMapPayload();
        assertTrue(methodCheck(res, ResponseStatus.PRECONDITION_FAILED));
    }

    @Test
    public void testSignOutWithBlankHashMapPayloadOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Sign Out With Blank HashMap Test------------");
        SignOutWithBlankHashMapPayload();
        assertTrue(methodCheck(res, ResponseStatus.PRECONDITION_FAILED));
    }

    public void SignInWithInvalidMethod(String di, String accessToken)
            throws Exception {
        System.out.println("-----Sign In With Invalid Method (GET)");
        IRequest request = null;
        request = SignInWithInvalidMethodRequest(di, accessToken);
        receiver.channelRead(ctx, request);
    }

    public void SignIn(String di, String accessToken) throws Exception {
        System.out.println("-----Sign In");
        IRequest request = null;
        request = SignInRequest(di, accessToken);
        receiver.channelRead(ctx, request);
    }

    public void SignInWithBlankHashMapPayload() throws Exception {
        System.out.println("-----Sign In With Blank Hashmap Payload");
        IRequest request = null;
        request = SignInBlankHashmapPayloadRequest();
        receiver.channelRead(ctx, request);
    }

    public void SignInWithNullPayload() throws Exception {
        System.out.println("-----Sign In With Null Payload");
        IRequest request = null;
        request = SignInNullPayloadRequest();
        receiver.channelRead(ctx, request);
    }

    public void SignOut(String di, String accessToken) throws Exception {
        System.out.println("-----Sign Out");
        IRequest request = null;
        request = SignOutRequest(di, accessToken);
        receiver.channelRead(ctx, request);
    }

    public void SignOutWithBlankHashMapPayload() throws Exception {
        System.out.println("-----Sign Out With Blank Hashmap Payload");
        IRequest request = null;
        request = SignOutBlankHashmapPayloadRequest();
        receiver.channelRead(ctx, request);
    }

    public void SignOutWithNullPayload() throws Exception {
        System.out.println("-----Sign Out With Null Payload");
        IRequest request = null;
        request = SignOutNullPayloadRequest();
        receiver.channelRead(ctx, request);
    }

    public IRequest SignInNullPayloadRequest() {
        IRequest request = null;
        request = MessageBuilder.createRequest(RequestMethod.POST,
                DEFAULT_AUTH_LOGIN, null);
        return request;
    }

    public IRequest SignInBlankHashmapPayloadRequest() {
        IRequest request = null;
        HashMap<String, Object> payloadData = new HashMap<String, Object>();
        Cbor<HashMap<String, Object>> cbor = new Cbor<HashMap<String, Object>>();
        request = MessageBuilder.createRequest(RequestMethod.POST,
                DEFAULT_AUTH_LOGIN, null, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        return request;
    }

    public IRequest SignOutNullPayloadRequest() {
        IRequest request = null;
        request = MessageBuilder.createRequest(RequestMethod.POST,
                DEFAULT_AUTH_LOGOUT, null);
        return request;
    }

    public IRequest SignOutBlankHashmapPayloadRequest() {
        IRequest request = null;
        HashMap<String, Object> payloadData = new HashMap<String, Object>();
        Cbor<HashMap<String, Object>> cbor = new Cbor<HashMap<String, Object>>();
        request = MessageBuilder.createRequest(RequestMethod.POST,
                DEFAULT_AUTH_LOGOUT, null, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        return request;
    }

    public IRequest SignInWithInvalidMethodRequest(String deviceId,
            String accessToken) {
        IRequest request = null;
        HashMap<String, Object> payloadData = new HashMap<String, Object>();
        payloadData.put("accesstoken", accessToken);
        payloadData.put("status", true);
        payloadData.put("di", deviceId);
        Cbor<HashMap<String, Object>> cbor = new Cbor<HashMap<String, Object>>();
        request = MessageBuilder.createRequest(RequestMethod.GET,
                DEFAULT_AUTH_LOGIN, null, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        return request;
    }

    public IRequest SignInRequest(String deviceId, String accessToken) {
        IRequest request = null;
        HashMap<String, Object> payloadData = new HashMap<String, Object>();
        payloadData.put("accesstoken", accessToken);
        payloadData.put("login", true);
        payloadData.put("di", deviceId);
        payloadData.put("uid", uuid);
        Cbor<HashMap<String, Object>> cbor = new Cbor<HashMap<String, Object>>();
        request = MessageBuilder.createRequest(RequestMethod.POST,
                DEFAULT_AUTH_LOGIN, null, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        return request;
    }

    public IRequest SignOutRequest(String deviceId, String accessToken) {
        IRequest request = null;
        HashMap<String, Object> payloadData = new HashMap<String, Object>();
        payloadData.put("accesstoken", accessToken);
        payloadData.put("login", false);
        payloadData.put("di", deviceId);
        payloadData.put("uid", uuid);
        Cbor<HashMap<String, Object>> cbor = new Cbor<HashMap<String, Object>>();
        request = MessageBuilder.createRequest(RequestMethod.POST,
                DEFAULT_AUTH_LOGOUT, null, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        return request;
    }

    public boolean hashmapCheck(IResponse response, String propertyName) {
        HashMap<String, Object> payloadData = mCbor
                .parsePayloadFromCbor(response.getPayload(), HashMap.class);
        if (payloadData.get(propertyName) != null)
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
