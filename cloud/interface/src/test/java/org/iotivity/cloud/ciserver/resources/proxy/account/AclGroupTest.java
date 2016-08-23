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

package org.iotivity.cloud.ciserver.resources.proxy.account;

import static java.util.concurrent.TimeUnit.SECONDS;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

import java.util.Arrays;
import java.util.HashMap;
import java.util.concurrent.CountDownLatch;

import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.device.IRequestChannel;
import org.iotivity.cloud.base.exception.ServerException.BadRequestException;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.RequestMethod;
import org.iotivity.cloud.ciserver.Constants;
import org.iotivity.cloud.ciserver.DeviceServerSystem;
import org.iotivity.cloud.util.Cbor;
import org.junit.Before;
import org.junit.Test;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;

public class AclGroupTest {
    private static final String  TEST_RESOURCE_GROUP_URI = Constants.GROUP_FULL_URI;

    private CoapDevice           mMockDevice             = mock(
            CoapDevice.class);
    private IRequest             mReq                    = null;
    private DeviceServerSystem   mDeviceServerSystem     = new DeviceServerSystem();
    private final CountDownLatch mLatch                  = new CountDownLatch(
            1);

    @Mock
    private IRequestChannel      mRequestChannel;

    @InjectMocks
    private AclGroup             mAclGroupHandler        = new AclGroup();

    @Before
    public void setUp() throws Exception {
        mReq = null;
        Mockito.doReturn("mockUserId").when(mMockDevice).getUserId();
        MockitoAnnotations.initMocks(this);
        mDeviceServerSystem.addResource(mAclGroupHandler);
        // callback mock
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapRequest answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapRequest request = (CoapRequest) args[0];
                System.out.println(
                        "\t----------payload : " + request.getPayloadString());
                System.out.println(
                        "\t----------uripath : " + request.getUriPath());
                System.out.println(
                        "\t---------uriquery : " + request.getUriQuery());
                mReq = request;
                mLatch.countDown();
                return null;
            }
        }).when(mRequestChannel).sendRequest(Mockito.any(IRequest.class),
                Mockito.any(CoapDevice.class));
    }

    @Test
    public void testCreateGroupRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(AS) Create Group Test------------");

        HashMap<String, Object> payloadData = new HashMap<>();
        payloadData.put("gtype", "public");
        Cbor<HashMap<Object, Object>> cbor = new Cbor<>();
        IRequest request = MessageBuilder.createRequest(RequestMethod.POST,
                TEST_RESOURCE_GROUP_URI, null, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        mAclGroupHandler.onRequestReceived(mMockDevice, request);

        assertTrue(mLatch.await(1L, SECONDS));
        assertTrue(cbor.parsePayloadFromCbor(mReq.getPayload(), HashMap.class)
                .containsKey("gtype"));
        assertTrue(cbor.parsePayloadFromCbor(mReq.getPayload(), HashMap.class)
                .containsKey("gmid"));
        assertEquals(mReq.getUriPath(), TEST_RESOURCE_GROUP_URI);
    }

    @Test
    public void testAddMemberRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(AS) Add Member Test------------");

        HashMap<String, Object> payloadData = new HashMap<>();
        Cbor<HashMap<Object, Object>> cbor = new Cbor<>();
        IRequest request = MessageBuilder.createRequest(RequestMethod.POST,
                TEST_RESOURCE_GROUP_URI + "/" + "sampleGroup", null,
                ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        mAclGroupHandler.onRequestReceived(mMockDevice, request);

        assertTrue(mLatch.await(1L, SECONDS));
        assertTrue(cbor.parsePayloadFromCbor(mReq.getPayload(), HashMap.class)
                .containsKey("midlist"));
        assertEquals(mReq.getUriPath(),
                TEST_RESOURCE_GROUP_URI + "/" + "sampleGroup");
    }

    @Test
    public void testAddDeviceRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(AS) Add Device Test------------");

        HashMap<String, Object> payloadData = new HashMap<>();
        payloadData.put("dilist", Arrays.asList("sampleDevice"));
        Cbor<HashMap<Object, Object>> cbor = new Cbor<>();
        IRequest request = MessageBuilder.createRequest(RequestMethod.POST,
                TEST_RESOURCE_GROUP_URI + "/" + "sampleGroup", null,
                ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        mAclGroupHandler.onRequestReceived(mMockDevice, request);

        assertTrue(mLatch.await(1L, SECONDS));
        assertTrue(cbor.parsePayloadFromCbor(mReq.getPayload(), HashMap.class)
                .containsKey("dilist"));
        assertEquals(mReq.getUriPath(),
                TEST_RESOURCE_GROUP_URI + "/" + "sampleGroup");
    }

    @Test
    public void testGetGroupListAndGetGroupInfoRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(AS) Get Group List & Get Group Info Test------------");

        IRequest request = MessageBuilder.createRequest(RequestMethod.GET,
                TEST_RESOURCE_GROUP_URI, null, null, null);
        mAclGroupHandler.onRequestReceived(mMockDevice, request);

        assertTrue(mLatch.await(1L, SECONDS));
        assertTrue(mReq.getUriQueryMap().containsKey("mid"));
        assertEquals(mReq.getUriPath(), TEST_RESOURCE_GROUP_URI);
    }

    @Test
    public void testDeleteGroupRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(AS) Delete Group Test------------");

        IRequest request = MessageBuilder.createRequest(RequestMethod.DELETE,
                TEST_RESOURCE_GROUP_URI, "gid=samplegid", null, null);
        mAclGroupHandler.onRequestReceived(mMockDevice, request);

        assertTrue(mLatch.await(1L, SECONDS));
        assertTrue(mReq.getUriQueryMap().containsKey("gid"));
        assertTrue(mReq.getUriQueryMap().containsKey("gmid"));
        assertEquals(mReq.getUriPath(), TEST_RESOURCE_GROUP_URI);
    }

    @Test
    public void testDeleteMemberRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(AS) Delete Member Test------------");

        IRequest request = MessageBuilder.createRequest(RequestMethod.DELETE,
                TEST_RESOURCE_GROUP_URI + "/" + "sampleGroup", null, null,
                null);
        mAclGroupHandler.onRequestReceived(mMockDevice, request);

        assertTrue(mLatch.await(1L, SECONDS));
        assertTrue(mReq.getUriQueryMap().containsKey("midlist"));
        assertEquals(mReq.getUriPath(),
                TEST_RESOURCE_GROUP_URI + "/" + "sampleGroup");
    }

    @Test
    public void testDeleteDeviceRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(AS) Delete Member Test------------");

        IRequest request = MessageBuilder.createRequest(RequestMethod.DELETE,
                TEST_RESOURCE_GROUP_URI + "/" + "sampleGroup",
                "dilist=sampledi", null, null);
        mAclGroupHandler.onRequestReceived(mMockDevice, request);

        assertTrue(mLatch.await(1L, SECONDS));
        assertTrue(mReq.getUriQueryMap().containsKey("dilist"));
        assertEquals(mReq.getUriPath(),
                TEST_RESOURCE_GROUP_URI + "/" + "sampleGroup");
    }

    @Test(expected = BadRequestException.class)
    public void testExceptionDeviceRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(AS) Delete Member Test------------");

        IRequest request = MessageBuilder.createRequest(RequestMethod.PUT,
                TEST_RESOURCE_GROUP_URI + "/" + "sampleGroup",
                "dilist=sampledi", null, null);
        mAclGroupHandler.onRequestReceived(mMockDevice, request);
    }
}
