package org.iotivity.cloud.accountserver.resources.account.tokenrefresh;

import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;

import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.accountserver.db.AccountDBManager;
import org.iotivity.cloud.accountserver.db.TokenTable;
import org.iotivity.cloud.accountserver.db.UserTable;
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
import org.junit.Before;
import org.junit.Test;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;

public class TokenRefreshResourceTest {
    private static final String            REFRESH_TOKEN_URI         = Constants.ACCOUNT_TOKENREFRESH_FULL_URI;
    private static final String            DEVICE_ID                 = "B371C481-38E6-4D47-8320-7688D8A5B58C";
    private String                         mAuthProvider             = "Google";
    private String                         mAccessToken              = "ya29.Ci9VA06h_WhVG0lV3nU-kXMPSmisPJ6sM5iqMuoaNz0YYCO2lkXo5TGGy7wiol6Rdw";
    private String                         mRefreshToken             = "1/UKLrjrMWErlq8has8XcpPg_riBcHcehKFUdoPoX9k0E";
    private String                         mUuid                     = "3ff62bac-c7cb-46f5-bb9b-c0f2f75f8c1d";
    private String                         mUserId                   = "userId";
    private Cbor<HashMap<String, Object>>  mCbor                     = new Cbor<>();
    private CoapDevice                     mMockDevice               = mock(
            CoapDevice.class);
    private TypeCastingManager<UserTable>  mUserTableCastingManager  = new TypeCastingManager<>();
    private TypeCastingManager<TokenTable> mTokenTableCastingManager = new TypeCastingManager<>();
    private TokenRefreshResource           mTokenRefreshResource     = new TokenRefreshResource();
    private IResponse                      mResponse                 = null;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.initMocks(this);
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
                mResponse = resp;
                return resp;
            }
        }).when(mMockDevice).sendResponse(Mockito.anyObject());
    }

    @Test
    public void testRefreshTokenonDefaultRequestReceived() throws Exception {
        String uuid = this.mUuid;
        RegisterTokenInfo(uuid, mUserId, mAccessToken, mRefreshToken);
        TokenRefresh(RequestMethod.POST, mMockDevice, uuid, DEVICE_ID,
                mRefreshToken);
        assertTrue(methodCheck(mResponse, ResponseStatus.CHANGED));
        assertTrue(hashmapCheck(mResponse, "accesstoken"));
        assertTrue(hashmapCheck(mResponse, "refreshtoken"));
        assertTrue(hashmapCheck(mResponse, "expiresin"));
    }

    @Test(expected = ServerException.NotFoundException.class)
    public void testRefreshTokenonRequestReceivedWrongRefreshToken()
            throws Exception {
        String uuid = this.mUuid + "WrongRefreshTokenCase";
        RegisterTokenInfo(uuid, mUserId, mAuthProvider, mRefreshToken);
        TokenRefresh(RequestMethod.POST, mMockDevice, uuid, DEVICE_ID,
                mRefreshToken + "NotExist");
    }

    @Test(expected = ServerException.BadRequestException.class)
    public void testRefreshTokenonRequestReceivedInvalidMethod()
            throws Exception {
        String uuid = this.mUuid + "InvalidRequestMethod (GET)";
        RegisterTokenInfo(uuid, mUserId, mAuthProvider, mRefreshToken);
        TokenRefresh(RequestMethod.GET, mMockDevice, uuid, DEVICE_ID,
                mRefreshToken + "InvalidMethod");
    }

    @Test(expected = ServerException.PreconditionFailedException.class)
    public void testRefreshTokenonRequestReceivedNullUuid() throws Exception {
        String uuid = this.mUuid + "NullUuid";
        RegisterTokenInfo(uuid, mUserId, mAuthProvider, mRefreshToken);
        TokenRefresh(RequestMethod.POST, mMockDevice, null, DEVICE_ID,
                mRefreshToken + "InvalidMethod");
    }

    @Test(expected = ServerException.PreconditionFailedException.class)
    public void testRefreshTokenonRequestReceivedNullDi() throws Exception {
        String uuid = this.mUuid + "NullDi";
        RegisterTokenInfo(uuid, mUserId, mAuthProvider, mRefreshToken);
        TokenRefresh(RequestMethod.POST, mMockDevice, uuid, null,
                mRefreshToken + "InvalidMethod");
    }

    @Test(expected = ServerException.PreconditionFailedException.class)
    public void testRefreshTokenonRequestReceivedNullRefreshToken()
            throws Exception {
        String uuid = this.mUuid + "NullRefreshToken";
        RegisterTokenInfo(uuid, mUserId, mAuthProvider, mRefreshToken);
        TokenRefresh(RequestMethod.POST, mMockDevice, uuid, DEVICE_ID, null);
    }

    public void TokenRefresh(RequestMethod method, CoapDevice device,
            String uuid, String di, String refreshToken) throws Exception {
        System.out.println(
                "-----Token Refresh using refreshtoken : " + refreshToken);
        IRequest request = null;
        request = RefreshTokenRequest(method, uuid, di, refreshToken);
        mTokenRefreshResource.onDefaultRequestReceived(device, request);
    }

    public IRequest RefreshTokenRequest(RequestMethod method, String uuid,
            String deviceId, String refreshToken) {
        IRequest request = null;
        HashMap<String, String> payloadData = new HashMap<String, String>();
        payloadData.put("uid", uuid);
        payloadData.put("di", deviceId);
        payloadData.put("granttype", "refresh_token");
        payloadData.put("refreshtoken", refreshToken);
        request = MessageBuilder.createRequest(method, REFRESH_TOKEN_URI, null,
                ContentFormat.APPLICATION_CBOR,
                mCbor.encodingPayloadToCbor(payloadData));
        return request;
    }

    public void RegisterTokenInfo(String uuid, String userId,
            String accessToken, String refreshToken) {
        HashMap<String, Object> tokenInfo = mTokenTableCastingManager
                .convertObjectToMap(
                        makeTokenTable(uuid, accessToken, refreshToken));
        HashMap<String, Object> userInfo = mUserTableCastingManager
                .convertObjectToMap(makeUserTable(uuid, userId));
        AccountDBManager.getInstance()
                .insertAndReplaceRecord(Constants.TOKEN_TABLE, tokenInfo);
        AccountDBManager.getInstance().insertRecord(Constants.USER_TABLE,
                userInfo);
    }

    private TokenTable makeTokenTable(String uuid, String accessToken,
            String refreshToken) {
        TokenTable tokenInfo = new TokenTable();
        tokenInfo.setUuid(uuid);
        tokenInfo.setDid(DEVICE_ID);
        tokenInfo.setAccesstoken(accessToken);
        tokenInfo.setRefreshtoken(refreshToken);
        tokenInfo.setProvider(mAuthProvider);
        tokenInfo.setExpiredtime(-1);
        Date currentTime = new Date();
        DateFormat transFormat = new SimpleDateFormat("yyyyMMddkkmm");
        tokenInfo.setIssuedtime(transFormat.format(currentTime));
        return tokenInfo;
    }

    private UserTable makeUserTable(String uuid, String userId) {
        UserTable userInfo = new UserTable();
        userInfo.setUuid(uuid);
        userInfo.setProvider(mAuthProvider);
        userInfo.setUserid(userId);
        return userInfo;
    }

    private boolean methodCheck(IResponse response,
            ResponseStatus responseStatus) {
        if (responseStatus == response.getStatus())
            return true;
        else
            return false;
    }

    private boolean hashmapCheck(IResponse response, String propertyName) {
        HashMap<String, Object> payloadData = mCbor
                .parsePayloadFromCbor(response.getPayload(), HashMap.class);
        if (payloadData.containsKey(propertyName))
            return true;
        else
            return false;
    }
}
