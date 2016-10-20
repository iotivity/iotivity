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
import org.iotivity.cloud.accountserver.db.MongoDB;
import org.iotivity.cloud.accountserver.db.TokenTable;
import org.iotivity.cloud.accountserver.db.UserTable;
import org.iotivity.cloud.accountserver.resources.acl.group.GroupResource;
import org.iotivity.cloud.accountserver.util.TypeCastingManager;
import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.exception.ServerException;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
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

public class AccountResourceTest {
    private static final String            REGISTER_URI              = Constants.ACCOUNT_FULL_URI;
    private static final String            GROUP_URI                 = Constants.GROUP_FULL_URI;
    private static final String            DEVICE_ID                 = "B371C481-38E6-4D47-8320-7688D8A5B58C";
    private String                         mAuthProvider             = "Github";
    private String                         mAuthCode                 = "b4878d614bbc5f8db463";
    private CoapDevice                     mMockDevice               = mock(
            CoapDevice.class);
    private Cbor<HashMap<String, Object>>  mCbor                     = new Cbor<>();
    private IResponse                      mResponse                 = null;
    private CountDownLatch                 mLatch                    = new CountDownLatch(
            1);
    private TypeCastingManager<UserTable>  mUserTableCastingManager  = new TypeCastingManager<>();
    private TypeCastingManager<TokenTable> mTokenTableCastingManager = new TypeCastingManager<>();
    private AccountResource                mAccountResource          = new AccountResource();
    private GroupResource                  mGroupResource            = new GroupResource();
    private String                         mGroupId                  = null;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.initMocks(this);
        resetAccountDatabase();
        // callback mock
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapResponse answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapResponse resp = (CoapResponse) args[0];
                if (resp.getPayload() != null) {
                    HashMap<String, Object> payloadData = mCbor
                            .parsePayloadFromCbor(resp.getPayload(),
                                    HashMap.class);
                    if (payloadData.containsKey("gid")) {
                        mGroupId = (String) payloadData.get("gid");
                    }
                }
                System.out
                        .println("\t----payload : " + resp.getPayloadString());
                System.out
                        .println("\t----responsestatus : " + resp.getStatus());
                mResponse = resp;
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
    public void testSignUpOnDefaultRequestReceived() throws Exception {
        getTestMethodName();
        signUp(DEVICE_ID, mAuthProvider, mAuthCode);
        assertTrue(methodCheck(mResponse, ResponseStatus.CHANGED));
        assertTrue(hashmapCheck(mResponse, "accesstoken"));
        assertTrue(hashmapCheck(mResponse, "refreshtoken"));
        assertTrue(hashmapCheck(mResponse, "tokentype"));
        assertTrue(hashmapCheck(mResponse, "uid"));
    }

    @Test(expected = ServerException.InternalServerErrorException.class)
    public void testSignUpwithWrongAuthCodeOnDefaultRequestReceived()
            throws Exception {
        getTestMethodName();
        String wrongAuthCode = "5fc4fcf376f8d7087a3a";
        signUp(DEVICE_ID, mAuthProvider, wrongAuthCode);
    }

    @Test(expected = ServerException.InternalServerErrorException.class)
    public void testSignUpwithWrongAuthProviderOnDefaultRequestReceived()
            throws Exception {
        getTestMethodName();
        signUp(DEVICE_ID, "wrongAuthProvider", mAuthCode);
    }

    @Test(expected = ServerException.PreconditionFailedException.class)
    public void testSignUpNullAuthProviderOnDefaultRequestReceived()
            throws Exception {
        getTestMethodName();
        signUp(DEVICE_ID, null, mAuthCode);
    }

    @Test(expected = ServerException.PreconditionFailedException.class)
    public void testSignUpNullAuthCodeOnDefaultRequestReceived()
            throws Exception {
        getTestMethodName();
        signUp(DEVICE_ID, mAuthProvider, null);
    }

    @Test(expected = ServerException.PreconditionFailedException.class)
    public void testSignUpNullDiOnDefaultRequestReceived() throws Exception {
        getTestMethodName();
        signUp(null, mAuthProvider, mAuthCode);
    }

    @Test(expected = ServerException.BadRequestException.class)
    public void testSignUpInvalidMethodOnDefaultRequestReceived()
            throws Exception {
        getTestMethodName();
        signUpInvalidMethod(DEVICE_ID, mAuthProvider, mAuthCode);
    }

    @Test
    public void testGetUserInfoSearchOnDefaultRequestReceived()
            throws Exception {
        getTestMethodName();
        String uuid = "u0001Search";
        // register TokenInfo and UserInfo to the DB
        registerTokenUserInfo(uuid, DEVICE_ID);
        // request uuid using search criteria
        getUserInfoSearch("search=userid:userId");
        Cbor<HashMap<String, ArrayList<HashMap<String, Object>>>> cbor = new Cbor<>();
        HashMap<String, ArrayList<HashMap<String, Object>>> payloadData = cbor
                .parsePayloadFromCbor(mResponse.getPayload(), HashMap.class);
        HashMap<String, Object> getUserInfo = ((ArrayList<HashMap<String, Object>>) payloadData
                .get("ulist")).get(0);

        assertTrue(getUserInfo.get("uid").equals(uuid));
    }

    @Test
    public void testDeleteDeviceDefaultGroupOnDefaultRequestReceived()
            throws Exception {
        getTestMethodName();
        // register the token table and user table to the DB
        String uuid = "u0001";
        registerTokenUserInfo(uuid, DEVICE_ID);
        createGroup(uuid, "Private");
        // register three devices to the group
        shareDevice(DEVICE_ID, uuid);
        shareDevice(DEVICE_ID + "2", uuid);
        shareDevice(DEVICE_ID + "3", uuid);
        // delete one device
        deleteDevice(DEVICE_ID, uuid);
        assertTrue(methodCheck(mResponse, ResponseStatus.DELETED));
    }

    @Test
    public void testDeleteDeviceInEveryGroupOnDefaultRequestReceived()
            throws Exception {
        getTestMethodName();
        // register the token table and user table for three resource servers to
        // the DB
        String uuid = "u0001DeleteEveryGroup";
        registerTokenUserInfo(uuid, DEVICE_ID);
        registerTokenInfo(uuid, DEVICE_ID + "2");
        registerTokenInfo(uuid, DEVICE_ID + "3");
        // register token table for the resource client
        registerTokenInfo(uuid, DEVICE_ID + "4");
        createGroup(uuid, "Private");
        // register three devices to the private group
        shareDevice(DEVICE_ID, uuid);
        shareDevice(DEVICE_ID + "2", uuid);
        shareDevice(DEVICE_ID + "3", uuid);
        // register resource servers to the public group
        createGroup(uuid, "Public");
        shareDevice(DEVICE_ID, mGroupId);
        shareDevice(DEVICE_ID + "2", mGroupId);
        shareDevice(DEVICE_ID + "3", mGroupId);
        // register resource servers to the public group
        createGroup(uuid, "Public");
        shareDevice(DEVICE_ID, mGroupId);
        shareDevice(DEVICE_ID + "2", mGroupId);
        shareDevice(DEVICE_ID + "3", mGroupId);
        // device delete for three resource servers
        deleteDevice(DEVICE_ID, uuid);
        assertTrue(methodCheck(mResponse, ResponseStatus.DELETED));
        deleteDevice(DEVICE_ID + "2", uuid);
        assertTrue(methodCheck(mResponse, ResponseStatus.DELETED));
        deleteDevice(DEVICE_ID + "3", uuid);
        assertTrue(methodCheck(mResponse, ResponseStatus.DELETED));
        // device delete for one resource client
        deleteDevice(DEVICE_ID + "4", uuid);
        assertTrue(methodCheck(mResponse, ResponseStatus.DELETED));
    }

    @Test
    public void testDeleteClientOnDefaultRequestReceived() throws Exception {
        getTestMethodName();
        // register the token table and user table to the DB
        String uuid = "u0001Client";
        HashMap<String, Object> tokenInfo = mTokenTableCastingManager
                .convertObjectToMap(makeTokenTable(uuid, DEVICE_ID));
        HashMap<String, Object> userInfo = mUserTableCastingManager
                .convertObjectToMap(makeUserTable(uuid));
        AccountDBManager.getInstance()
                .insertAndReplaceRecord(Constants.TOKEN_TABLE, tokenInfo);
        AccountDBManager.getInstance().insertRecord(Constants.USER_TABLE,
                userInfo);
        deleteDevice(DEVICE_ID, uuid);
        assertTrue(methodCheck(mResponse, ResponseStatus.DELETED));
    }

    @Test
    public void testGetUserInfoUsingUuidOnDefaultRequestReceived()
            throws Exception {
        getTestMethodName();
        // register the token table and user table to the DB
        String uuid = "u0001Get";
        HashMap<String, Object> tokenInfo = mTokenTableCastingManager
                .convertObjectToMap(makeTokenTable(uuid, DEVICE_ID));
        HashMap<String, Object> userInfo = mUserTableCastingManager
                .convertObjectToMap(makeUserTable(uuid));
        AccountDBManager.getInstance()
                .insertAndReplaceRecord(Constants.TOKEN_TABLE, tokenInfo);
        AccountDBManager.getInstance().insertRecord(Constants.USER_TABLE,
                userInfo);
        getUserInfoUsingUuid(uuid);
        assertTrue(methodCheck(mResponse, ResponseStatus.CONTENT));
    }

    private void registerTokenUserInfo(String uuid, String deviceId) {
        registerTokenInfo(uuid, deviceId);
        registerUserInfo(uuid);
    }

    private void registerTokenInfo(String uuid, String deviceId) {
        HashMap<String, Object> tokenInfo = mTokenTableCastingManager
                .convertObjectToMap(makeTokenTable(uuid, deviceId));
        AccountDBManager.getInstance()
                .insertAndReplaceRecord(Constants.TOKEN_TABLE, tokenInfo);
    }

    private void registerUserInfo(String uuid) {
        HashMap<String, Object> userInfo = mUserTableCastingManager
                .convertObjectToMap(makeUserTable(uuid));
        AccountDBManager.getInstance().insertRecord(Constants.USER_TABLE,
                userInfo);
    }

    private void getTestMethodName() {
        StackTraceElement[] stacks = new Throwable().getStackTrace();
        StackTraceElement currentStack = stacks[1];
        System.out.println("\t---Test Name : " + currentStack.getMethodName());
    }

    private void getUserInfoUsingUuid(String uuid) throws Exception {
        System.out.println("-----Get User Info : " + uuid);
        IRequest request = null;
        request = getUserInfoUsingUuidRequest(uuid);
        mAccountResource.onDefaultRequestReceived(mMockDevice, request);
    }

    private IRequest getUserInfoUsingUuidRequest(String uuid) {
        IRequest request = MessageBuilder.createRequest(RequestMethod.GET,
                REGISTER_URI, "uid=" + uuid);
        return request;
    }

    private void createGroup(String uuid, String gtype) throws Exception {
        System.out.println("-----Create Group");
        IRequest request = null;
        request = createDefaultGroupRequest(uuid, gtype);
        mGroupResource.onDefaultRequestReceived(mMockDevice, request);
    }

    private void deleteDevice(String di, String uid) throws Exception {
        System.out.println("-----Delete Device");
        IRequest request = null;
        request = deleteDeviceRequest(di, uid);
        mAccountResource.onDefaultRequestReceived(mMockDevice, request);
    }

    private void signUp(String di, String authProvider, String authCode)
            throws Exception {
        System.out.println("-----Sign Up");
        IRequest request = null;
        request = signUpRequest(di, authProvider, authCode);
        mAccountResource.onDefaultRequestReceived(mMockDevice, request);
    }

    private void signUpInvalidMethod(String di, String authProvider,
            String authCode) throws Exception {
        System.out.println("-----Sign Up Invalid RequestMethod");
        IRequest request = null;
        request = signUpInvalidMethodRequest(di, authProvider, authCode);
        mAccountResource.onDefaultRequestReceived(mMockDevice, request);
    }

    private void shareDevice(String deviceId, String gid) throws Exception {
        System.out.println("-----Share Device");
        IRequest request = null;
        request = createShareDeviceRequest(deviceId, gid);
        mGroupResource.onDefaultRequestReceived(mMockDevice, request);
    }

    private IRequest deleteDeviceRequest(String deviceId, String uid) {
        IRequest request = MessageBuilder.createRequest(RequestMethod.DELETE,
                REGISTER_URI, "di=" + deviceId + ";uid=" + uid);
        return request;
    }

    private IRequest createDefaultGroupRequest(String uuid, String gtype) {
        IRequest request = null;
        HashMap<String, String> payloadData = new HashMap<String, String>();
        payloadData.put("gmid", uuid);
        payloadData.put("gtype", gtype);
        request = MessageBuilder.createRequest(RequestMethod.POST, GROUP_URI,
                null, ContentFormat.APPLICATION_CBOR,
                mCbor.encodingPayloadToCbor(payloadData));
        return request;
    }

    private IRequest createShareDeviceRequest(String deviceId, String gid) {
        IRequest request = null;
        HashMap<String, Object> payloadData = new HashMap<String, Object>();
        ArrayList<String> diList = new ArrayList<>();
        diList.add(deviceId);
        payloadData.put("dilist", diList);
        request = MessageBuilder.createRequest(RequestMethod.POST,
                GROUP_URI + "/" + gid, null, ContentFormat.APPLICATION_CBOR,
                mCbor.encodingPayloadToCbor(payloadData));
        return request;
    }

    private IRequest signUpRequest(String deviceId, String authProvider,
            String authCode) {
        IRequest request = null;
        HashMap<String, String> payloadData = new HashMap<String, String>();
        payloadData.put("authcode", authCode);
        payloadData.put("authprovider", authProvider);
        payloadData.put("di", deviceId);
        request = MessageBuilder.createRequest(RequestMethod.POST, REGISTER_URI,
                null, ContentFormat.APPLICATION_CBOR,
                mCbor.encodingPayloadToCbor(payloadData));
        return request;
    }

    private void getUserInfoSearch(String query) {
        System.out.println("-----get User Info Search using query: " + query);
        IRequest request = MessageBuilder.createRequest(RequestMethod.GET,
                REGISTER_URI, query);
        mAccountResource.onDefaultRequestReceived(mMockDevice, request);
    }

    private IRequest signUpInvalidMethodRequest(String deviceId,
            String authProvider, String authCode) {
        IRequest request = null;
        HashMap<String, String> payloadData = new HashMap<String, String>();
        payloadData.put("authcode", authCode);
        payloadData.put("authprovider", authProvider);
        payloadData.put("di", deviceId);
        request = MessageBuilder.createRequest(RequestMethod.GET, REGISTER_URI,
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

    private TokenTable makeTokenTable(String uuid, String deviceId) {
        TokenTable tokenInfo = new TokenTable();
        tokenInfo.setUuid(uuid);
        tokenInfo.setDid(deviceId);
        tokenInfo.setAccesstoken("at0001");
        tokenInfo.setRefreshtoken("rt0001");
        tokenInfo.setProvider(mAuthProvider);
        tokenInfo.setExpiredtime(-1);
        Date currentTime = new Date();
        DateFormat transFormat = new SimpleDateFormat("yyyyMMddkkmm");
        tokenInfo.setIssuedtime(transFormat.format(currentTime));
        return tokenInfo;
    }

    private UserTable makeUserTable(String uuid) {
        UserTable userInfo = new UserTable();
        userInfo.setUuid(uuid);
        userInfo.setProvider(mAuthProvider);
        userInfo.setUserid("userId");
        return userInfo;
    }

}
