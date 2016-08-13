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

package org.iotivity.cloud.mqserver.resources;

import static org.junit.Assert.*;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.timeout;
import static org.mockito.Mockito.verify;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.atomic.AtomicBoolean;

import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.Observe;
import org.iotivity.cloud.base.protocols.enums.RequestMethod;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.mqserver.resources.MQBrokerResource;
import org.iotivity.cloud.util.Cbor;
import org.junit.Before;
import org.junit.Test;
import org.mockito.Mockito;
import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;
import static com.jayway.awaitility.Awaitility.await;
import static java.util.concurrent.TimeUnit.SECONDS;

public class MQBrokerResourceTest {
    private final String     MQ_BROKER_URI    = "/.well-known/ocf/ps";
    private MQBrokerResource mqBrokerResource = null;
    private String           topicNamePrefix  = null;
    private CoapDevice       mockDevice       = null;
    IResponse                res              = null;
    CountDownLatch           latch            = null;

    @Before // setup for each test
    public void setUp() throws Exception {
        mqBrokerResource = new MQBrokerResource();
        // insert user's zookeper and broker addresses
        String zookeeper = "127.0.0.1:2181";
        String broker = "127.0.0.1:9092";
        topicNamePrefix = "mqtestTopic";
        mqBrokerResource.setKafkaInformation(zookeeper, broker);
        latch = new CountDownLatch(1);
        res = null; // initialize response packet
        mockDevice = mock(CoapDevice.class);
        mqBrokerResource.setKafkaInformation(zookeeper, broker);
        // callback mock
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapResponse answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapResponse resp = (CoapResponse) args[0];
                res = resp;
                latch.countDown();
                return resp;
            }
        }).when(mockDevice).sendResponse(Mockito.anyObject());
    }

    @Test // test topic creation
    public void testTopicCreationOnDefaultRequestReceived() throws Exception {
        System.out.println("\t--------------Topic Creation Test------------");
        CreateTopic(mockDevice, topicNamePrefix);
        // assertion: if the response status is "CREATED"
        assertTrue(latch.await(1L, SECONDS));
        assertTrue(methodCheck(res, ResponseStatus.CREATED));
    }

    @Test // test subtopic create
    public void testSubTopicCreationOnDefaultRequestReceived()
            throws Exception {
        System.out
                .println("\t--------------SubTopic Creation Test------------");
        String mainTopic = topicNamePrefix + "Main";
        String subTopic = topicNamePrefix + "Sub";
        // create main topic
        CreateTopic(mockDevice, mainTopic);
        // create sub topic
        CreateSubTopic(mockDevice, mainTopic, subTopic);
        // assertion: if the response status is "CREATED"
        assertTrue(latch.await(1L, SECONDS));
        assertTrue(methodCheck(res, ResponseStatus.CREATED));
    }

    @Test // test topic publish
    public void testTopicPublishOnDefaultRequestReceived() throws Exception {
        System.out.println("\t--------------Topic Publish Test------------");
        String topic = topicNamePrefix + "ForPub";
        // topic creation
        CreateTopic(mockDevice, topic);
        // topic publish
        PublishTopic(mockDevice, topic);
        // assertion: if the response status is "CHANGED"
        assertTrue(latch.await(1L, SECONDS));
        assertTrue(methodCheck(res, ResponseStatus.CHANGED));
    }

    @Test // test subscribe request
    public void testSubscribeOnDefaultRequestReceived() throws Exception {
        System.out.println("\t--------------Topic Subscribe Test------------");
        CoapDevice mockSubscriber = mock(CoapDevice.class);
        String topic = topicNamePrefix + "SubscribeTest";
        // create topic
        CreateTopic(mockDevice, topic);
        // publish topic
        PublishTopic(mockDevice, topic);
        // callback mock for subscriber
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapResponse answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapResponse resp = (CoapResponse) args[0];
                // assertion: if the response status is "CONTENT"
                // assertion: if the response payload has the "message" property
                assertTrue(methodCheck(resp, ResponseStatus.CONTENT));
                assertTrue(hashmapCheck(resp, "message"));
                return resp;
            }
        }).when(mockSubscriber).sendResponse(Mockito.anyObject());
        // subscribe topic
        SubscribeTopic(mockSubscriber, topic, Observe.SUBSCRIBE);
    }

    @Test // test unsubscribe request
    public void testUnSubscribeOnDefaultRequestReceived() throws Exception {
        System.out
                .println("\t--------------Topic Unsubscribe Test------------");
        CountDownLatch latchSubscriber = new CountDownLatch(2);
        CoapDevice mockSubscriber = mock(CoapDevice.class);
        String topic = topicNamePrefix + "UnSubscribeTest";
        // create topic
        CreateTopic(mockDevice, topic);
        // publish topic
        PublishTopic(mockDevice, topic);
        // callback mock for subscriber
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapResponse answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapResponse resp = (CoapResponse) args[0];
                latchSubscriber.countDown();
                if (latchSubscriber.getCount() == 0) {
                    // assertion: if the response payload has the "message"
                    // property
                    assertTrue(methodCheck(resp, ResponseStatus.CONTENT));
                    assertTrue(hashmapCheck(resp, "message"));
                }
                return resp;
            }
        }).when(mockSubscriber).sendResponse(Mockito.anyObject());
        // subscribe topic
        SubscribeTopic(mockSubscriber, topic, Observe.SUBSCRIBE);
        // unsubscribe topic
        SubscribeTopic(mockSubscriber, topic, Observe.UNSUBSCRIBE);
    }

    @Test // test delete request
    public void testDeleteTopicOnDefaultRequestReceived() throws Exception {
        System.out.println("\t--------------Topic Delete Test------------");
        String topic = topicNamePrefix + "DeleteTest";
        // create topic
        CreateTopic(mockDevice, topic);
        // delete topic
        DeleteTopic(mockDevice, topic);
        // assertion: if the response status is "DELETED"
        assertTrue(methodCheck(res, ResponseStatus.DELETED));
        assertTrue(latch.await(1L, SECONDS));
    }

    @Test // test delete subtopic request
    public void testDeleteSubTopicOnDefaultRequestReceived() throws Exception {
        System.out.println("\t--------------Subtopic Delete Test------------");
        String topic = topicNamePrefix + "DeleteTest";
        String subTopic = topicNamePrefix + "DeleteTestSub";
        // create topic
        CreateTopic(mockDevice, topic);
        // create subtopic
        CreateSubTopic(mockDevice, topic, subTopic);
        // delete subtopic
        DeleteSubTopic(mockDevice, topic, subTopic);
        // assertion: if the response status is "DELETED"
        assertTrue(methodCheck(res, ResponseStatus.DELETED));
    }

    @Test // test notify
    public void testTopicSubscribeNofityOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Topic Publish Notify Test------------");
        CoapDevice mockSubscriber = mock(CoapDevice.class);
        CountDownLatch latchSubscriber = new CountDownLatch(2);
        AtomicBoolean countTrue = new AtomicBoolean();
        countTrue.set(false);
        String topic = topicNamePrefix + "NotifyTest";
        // callback mock for subscriber
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapResponse answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapResponse resp = (CoapResponse) args[0];
                latchSubscriber.countDown();
                // assertion for subscriber
                if (latchSubscriber.getCount() == 0) {
                    assertTrue(methodCheck(resp, ResponseStatus.CONTENT));
                    assertTrue(hashmapCheck(resp, "message"));
                }
                return resp;
            }
        }).when(mockSubscriber).sendResponse(Mockito.anyObject());
        // create topic
        CreateTopic(mockDevice, topic);
        // publish topic (publisher)
        PublishTopic(mockDevice, topic);
        // subscribe topic (subscriber)
        SubscribeTopic(mockSubscriber, topic, Observe.SUBSCRIBE);
        await().atMost(2, SECONDS).untilFalse(countTrue);
        PublishTopic(mockDevice, topic);
        // verity if subscriber receives two responses
        assertTrue(latchSubscriber.await(2L, SECONDS));
        verify(mockSubscriber, timeout(5000).times(2))
                .sendResponse(Mockito.anyObject());
    }

    @Test // test discover request
    public void testTopicDiscoverOnDefaultRequestReceived() throws Exception {
        System.out.println("\t--------------Topic Discover Test------------");
        String topic = topicNamePrefix + "DiscoverTest";
        String subTopic = topic + "sub";
        // create topic
        CreateTopic(mockDevice, topic);
        // create sub topic
        CreateSubTopic(mockDevice, topic, subTopic);
        // discover topic
        DiscoverTopic();
        // assertion 1: if the response status is "CONTENT"
        // assertion 2: if the response payload has "topiclist" property
        // and there is the topic created in this unit test in the array
        boolean methodCheck = methodCheck(res, ResponseStatus.CONTENT);
        Cbor<HashMap<String, ArrayList<String>>> mArrayCbor = new Cbor<>();
        HashMap<String, ArrayList<String>> payloadData = mArrayCbor
                .parsePayloadFromCbor(res.getPayload(), HashMap.class);
        ArrayList<String> topicList = payloadData.get("topiclist");
        System.out.println("\ttopicList : " + topicList);
        assertTrue(methodCheck);
        assertTrue(topicList.contains("/.well-known/ocf/ps/" + topic));
        assertTrue(topicList
                .contains("/.well-known/ocf/ps/" + topic + "/" + subTopic));
    }

    @Test // topic read request
    public void testTopicReadOnDefaultRequestReceived() throws Exception {
        System.out.println("\t--------------Topic Read Test------------");
        String topic = topicNamePrefix + "ReadTest";
        // create topic
        CreateTopic(mockDevice, topic);
        // publish topic
        PublishTopic(mockDevice, topic);
        // read topic
        ReadTopic(topic);
        // assertion1 : if the response status is "CONTENT"
        // assertion2 : if the response payload has the "message" property
        assertTrue(methodCheck(res, ResponseStatus.CONTENT));
        assertTrue(hashmapCheck(res, "message"));
    }

    @Test // duplicate topic creation
    public void testDuplicatedTopicCreateOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Duplicated Topic Creation Test------------");
        String topic = topicNamePrefix + "DuplicateTest";
        // create topic
        CreateTopic(mockDevice, topic);
        // create topic again
        CreateTopic(mockDevice, topic);
        // assertion: if the response status is "BAD REQUEST"
        assertTrue(methodCheck(res, ResponseStatus.BAD_REQUEST));
    }

    @Test // publish not created topic
    public void testNotCreatedTopicPublishOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Not Created Topic Publish Test------------");
        String topic = topicNamePrefix + "NotCreatedTopicTest";
        // publish not created topic
        PublishTopic(mockDevice, topic);
        // assertion: if the response status is "BAD REQUEST"
        assertTrue(methodCheck(res, ResponseStatus.BAD_REQUEST));
    }

    @Test // subscribe not created topic
    public void testNotCreatedTopicSubscribeOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Not Created Topic Subscribe Test------------");
        CoapDevice mockSubscriber = mock(CoapDevice.class);
        String topic = topicNamePrefix + "NotCreatedTopicSubscribeTest";
        // callback mock for subscriber
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapResponse answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapResponse resp = (CoapResponse) args[0];
                // assertion: if the response status is "BAD REQUEST"
                assertTrue(methodCheck(resp, ResponseStatus.BAD_REQUEST));
                return resp;
            }
        }).when(mockSubscriber).sendResponse(Mockito.anyObject());
        // subscribe topic (subscriber)
        SubscribeTopic(mockSubscriber, topic, Observe.SUBSCRIBE);
    }

    @Test // unsubscribe not created topic
    public void testNotCreatedTopicUnSubscribeOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Not Created Topic Unsubscribe Test------------");
        CoapDevice mockSubscriber = mock(CoapDevice.class);
        String topic = topicNamePrefix + "NotCreatedTopicUnSubscribeTest";
        // callback mock for subscriber
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapResponse answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapResponse resp = (CoapResponse) args[0];
                // assertion: if the response status is "BAD REQUEST"
                assertTrue(methodCheck(resp, ResponseStatus.BAD_REQUEST));
                return resp;
            }
        }).when(mockSubscriber).sendResponse(Mockito.anyObject());
        // unsubscribe topic (subscriber)
        SubscribeTopic(mockSubscriber, topic, Observe.UNSUBSCRIBE);
    }

    @Test // create subtopic under not created maintopic
    public void testSubTopicCreateUnderNotCreatedTopicOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Create Subtopic under Not Created Maintopic  ------------");
        String mainTopic = topicNamePrefix + "NotCreatedMain";
        String subTopic = topicNamePrefix + "NotCreatedSub";
        // create sub topic
        CreateSubTopic(mockDevice, mainTopic, subTopic);
        // assertion: if the response status is "BAD REQUEST"
        assertTrue(methodCheck(res, ResponseStatus.BAD_REQUEST));
    }

    @Test // create topic which has 'core.light' rt
    public void testTopicCreationWithRtOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Topic Creation with RT Test------------");
        String topicName = topicNamePrefix + "RtTest";
        String rt = "rt=core.light";
        CreateTopicWithRt(mockDevice, topicName, rt);
        // assertion: if the response status is "CREATED"
        assertTrue(methodCheck(res, ResponseStatus.CREATED));
    }

    @Test // test discover request with rt
    public void testDiscoverTopicWithRtOnDefaultRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------Topic Discover with Rt Test------------");
        String topicName = topicNamePrefix + "DiscoverRtTest";
        String topicNameWithoutRt = topicNamePrefix + "DiscoverRtTestWithoutRt";
        String rt = "rt=core.light";
        // create topic with rt
        CreateTopicWithRt(mockDevice, topicName, rt);
        // create topic
        CreateTopic(mockDevice, topicNameWithoutRt);
        // discover topic
        DiscoverTopicWithRt(rt);
        // assertion 1: if the response status is "CONTENT"
        // assertion 2: if topic list contains the topic with rt
        // assertion 3: if topic list contains no topics which does not have the
        // rt
        Cbor<HashMap<String, ArrayList<String>>> mArrayCbor = new Cbor<>();
        HashMap<String, ArrayList<String>> payloadData = mArrayCbor
                .parsePayloadFromCbor(res.getPayload(), HashMap.class);
        ArrayList<String> topicList = payloadData.get("topiclist");
        System.out.println("\ttopicList : " + topicList);
        assertTrue(methodCheck(res, ResponseStatus.CONTENT));
        assertTrue(topicList.contains("/.well-known/ocf/ps/" + topicName));
        assertFalse(topicList
                .contains("/.well-known/ocf/ps/" + topicNameWithoutRt));
    }

    private IRequest PublishTopicRequest(String topicName) {
        IRequest request = null;
        HashMap<String, Object> tags = new HashMap<String, Object>();
        HashMap<String, Object> message = new HashMap<String, Object>();
        message.put("status", "on");
        message.put("brightness", 20);
        tags.put("message", message);
        Cbor<HashMap<String, Object>> cbor = new Cbor<HashMap<String, Object>>();
        String uri = MQ_BROKER_URI + "/" + topicName;
        request = MessageBuilder.createRequest(RequestMethod.POST, uri, null,
                ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(tags));
        return request;
    }

    private IRequest CreateTopicRequest(String topicName) {
        IRequest request = null;
        request = MessageBuilder.createRequest(RequestMethod.PUT,
                MQ_BROKER_URI + "/" + topicName, null);
        return request;
    }

    private IRequest CreateSubTopicRequest(String topicName,
            String subTopicName) {
        IRequest request = null;
        String uri = MQ_BROKER_URI + "/" + topicName + "/" + subTopicName;
        request = MessageBuilder.createRequest(RequestMethod.PUT, uri, null);
        return request;
    }

    private IRequest CreateTopicWithRtRequest(String topicName, String type) {
        IRequest request = null;
        request = MessageBuilder.createRequest(RequestMethod.PUT,
                MQ_BROKER_URI + "/" + topicName, type);
        return request;
    }

    private IRequest DeleteTopicRequest(String topicName) {
        IRequest request = null;
        String uri = MQ_BROKER_URI + "/" + topicName;
        request = MessageBuilder.createRequest(RequestMethod.DELETE, uri, null);
        return request;
    }

    private IRequest SubscribeTopicRequest(String topicName) {
        IRequest request = null;
        String uri = MQ_BROKER_URI + "/" + topicName;
        request = MessageBuilder.createRequest(RequestMethod.GET, uri, null);
        return request;
    }

    private IRequest DiscoverTopicRequest() {
        IRequest request = null;
        request = MessageBuilder.createRequest(RequestMethod.GET, MQ_BROKER_URI,
                null);
        return request;
    }

    private IRequest DiscoverTopicWithRtRequest(String rt) {
        IRequest request = null;
        request = MessageBuilder.createRequest(RequestMethod.GET, MQ_BROKER_URI,
                rt);
        return request;
    }

    private CoapRequest ReadTopicRequest(String topicName) {
        IRequest request = null;
        String uri = MQ_BROKER_URI + "/" + topicName;
        request = MessageBuilder.createRequest(RequestMethod.GET, uri, null);
        CoapRequest mqRequest = (CoapRequest) request;
        return mqRequest;
    }

    private void CreateTopicWithRt(CoapDevice mockDevice, String topicName,
            String type) throws Exception {
        System.out.println("-----CreateTopic || topic : " + topicName);
        IRequest request = null;
        request = CreateTopicWithRtRequest(topicName, type);
        mqBrokerResource.onDefaultRequestReceived(mockDevice, request);
    }

    private void CreateTopic(CoapDevice mockDevice, String topicName)
            throws Exception {
        System.out.println("-----CreateTopic || topic : " + topicName);
        IRequest request = null;
        request = CreateTopicRequest(topicName);
        mqBrokerResource.onDefaultRequestReceived(mockDevice, request);
    }

    private void CreateSubTopic(CoapDevice mockDevice, String mainTopicName,
            String subTopicName) throws Exception {
        System.out.println("-----CreateSubTopic || topic : " + mainTopicName
                + " || subtopic : " + subTopicName);
        IRequest subTopicRequest = null;
        subTopicRequest = CreateSubTopicRequest(mainTopicName, subTopicName);
        mqBrokerResource.onDefaultRequestReceived(mockDevice, subTopicRequest);
    }

    private void PublishTopic(CoapDevice mockDevice, String topicName)
            throws Exception {
        System.out.println("-----PublishTopic : " + topicName);
        IRequest request = null;
        request = PublishTopicRequest(topicName);
        mqBrokerResource.onDefaultRequestReceived(mockDevice, request);
    }

    private void SubscribeTopic(CoapDevice mockSubscriber, String topicName,
            Observe observe) throws Exception {
        System.out.println("-----SubscribeTopic : " + topicName);
        IRequest requestSubscribe = null;
        requestSubscribe = SubscribeTopicRequest(topicName);
        CoapRequest mqRequest = (CoapRequest) requestSubscribe;
        mqRequest.setObserve(observe);
        mqBrokerResource.onDefaultRequestReceived(mockSubscriber, mqRequest);
    }

    private void DeleteTopic(CoapDevice mockDevice, String topicName)
            throws Exception {
        System.out.println("-----DeleteTopic : " + topicName);
        IRequest requestToDelete = null;
        requestToDelete = DeleteTopicRequest(topicName);
        mqBrokerResource.onDefaultRequestReceived(mockDevice, requestToDelete);
    }

    private void DeleteSubTopic(CoapDevice mockDevice, String topicName,
            String subTopicName) throws Exception {
        System.out.println("-----DeleteTopic : " + topicName);
        String deleteUri = topicName + "/" + subTopicName;
        IRequest requestToDelete = null;
        requestToDelete = DeleteTopicRequest(deleteUri);
        mqBrokerResource.onDefaultRequestReceived(mockDevice, requestToDelete);
    }

    private void DiscoverTopic() throws Exception {
        System.out.println("-----DiscoverTopic : ");
        IRequest requestToDiscover = null;
        requestToDiscover = DiscoverTopicRequest();
        mqBrokerResource.onDefaultRequestReceived(mockDevice,
                requestToDiscover);
    }

    private void DiscoverTopicWithRt(String rt) throws Exception {
        System.out.println("-----DiscoverTopicWithRt : ");
        IRequest requestToDiscover = null;
        requestToDiscover = DiscoverTopicWithRtRequest(rt);
        mqBrokerResource.onDefaultRequestReceived(mockDevice,
                requestToDiscover);
    }

    private void ReadTopic(String topicName) throws Exception {
        System.out.println("-----ReadTopic : " + topicName);
        CoapRequest readRequest = null;
        readRequest = ReadTopicRequest(topicName);
        mqBrokerResource.onDefaultRequestReceived(mockDevice, readRequest);
    }

    private boolean hashmapCheck(IResponse response, String propertyName) {
        Cbor<HashMap<String, Object>> mCbor = new Cbor<>();
        HashMap<String, Object> payloadData = mCbor
                .parsePayloadFromCbor(response.getPayload(), HashMap.class);
        if (payloadData.get(propertyName) != null)
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
}
