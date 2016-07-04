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
package org.iotivity.cloud.mqserver.topic;

import java.util.ArrayList;
import java.util.HashMap;

import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.mqserver.Constants;
import org.iotivity.cloud.mqserver.MessageQueueUtils;
import org.iotivity.cloud.mqserver.kafka.KafkaConsumerWrapper;
import org.iotivity.cloud.mqserver.kafka.KafkaProducerWrapper;

public class Topic {

    private TopicManager           mTopicManager = null;

    private String                 mName         = null;
    private String                 mType         = null;
    private HashMap<String, Topic> mSubtopics    = null;

    private byte[]                 mLatestData   = null;

    private class TopicSubscriber {
        TopicSubscriber(Device subscriber, IRequest request) {
            mSubscriber = subscriber;
            mRequest = request;
        }

        public Device   mSubscriber;
        public IRequest mRequest;
    }

    private HashMap<String, TopicSubscriber> mSubscribers           = null;

    private KafkaProducerWrapper             mKafkaProducerOperator = null;
    private KafkaConsumerWrapper             mKafkaConsumerOperator = null;

    public Topic(String name, String type, TopicManager topicManager) {

        mTopicManager = topicManager;
        mName = name;
        mType = type;

        mSubtopics = new HashMap<>();
        mSubscribers = new HashMap<>();

        String kafka_zookeeper = topicManager.getKafkaZookeeper();
        String kafka_broker = topicManager.getKafkaBroker();

        mKafkaProducerOperator = new KafkaProducerWrapper(kafka_broker, name);
        mKafkaConsumerOperator = new KafkaConsumerWrapper(kafka_zookeeper,
                kafka_broker, this);
    }

    public String getName() {
        return mName;
    }

    public String getType() {
        return mType;
    }

    public IResponse handleCreateSubtopic(IRequest request) {

        String newTopicName = MessageQueueUtils.extractDataFromPayload(
                request.getPayload(), Constants.MQ_TOPIC);

        String newTopicType = new String();

        if (request.getUriQueryMap() != null) {
            newTopicType = request.getUriQueryMap().get("rt").get(0);
        }

        if (newTopicName == null) {
            return MessageBuilder.createResponse(request,
                    ResponseStatus.BAD_REQUEST);
        }

        if (getSubtopic(newTopicName) != null) {
            // topic already exists
            return MessageBuilder.createResponse(request,
                    ResponseStatus.BAD_REQUEST);
        }

        Topic newTopic = new Topic(mName + "/" + newTopicName, newTopicType,
                mTopicManager);

        if (mTopicManager.createTopic(newTopic) == false) {
            return MessageBuilder.createResponse(request,
                    ResponseStatus.INTERNAL_SERVER_ERROR);
        }

        mSubtopics.put(newTopicName, newTopic);

        return MessageBuilder.createResponse(
                request,
                ResponseStatus.CREATED,
                ContentFormat.APPLICATION_CBOR,
                MessageQueueUtils.buildPayload(Constants.MQ_LOCATION,
                        newTopic.getName()));
    }

    public IResponse handleRemoveSubtopic(IRequest request, String topicName) {

        Topic targetTopic = getSubtopic(topicName);

        // TODO check error
        if (targetTopic == null) {
            // topic doesn't exist
            return MessageBuilder.createResponse(request,
                    ResponseStatus.BAD_REQUEST);
        }

        if (mTopicManager.removeTopic(targetTopic) == false) {
            return MessageBuilder.createResponse(request,
                    ResponseStatus.INTERNAL_SERVER_ERROR);
        }

        mSubtopics.remove(topicName);

        return MessageBuilder.createResponse(request, ResponseStatus.DELETED);
    }

    public IResponse handleSubscribeTopic(Device srcDevice, IRequest request) {

        // get latest data from kafka if cosumer started for the first time
        if (mKafkaConsumerOperator.consumerStarted() == false) {

            ArrayList<byte[]> data = mKafkaConsumerOperator.getMessages();

            if (data != null && !data.isEmpty()) {
                mLatestData = data.get(data.size() - 1);
            }
        }

        if (mKafkaConsumerOperator.subscribeTopic() == false) {
            return MessageBuilder.createResponse(request,
                    ResponseStatus.INTERNAL_SERVER_ERROR);
        }

        synchronized (mSubscribers) {
            mSubscribers.put(request.getRequestId(), new TopicSubscriber(
                    srcDevice, request));
        }

        return MessageBuilder.createResponse(request, ResponseStatus.CONTENT,
                ContentFormat.APPLICATION_CBOR, mLatestData);
    }

    public IResponse handleUnsubscribeTopic(IRequest request) {

        synchronized (mSubscribers) {

            TopicSubscriber subscriber = mSubscribers.get(request
                    .getRequestId());

            if (subscriber == null) {
                // client is not a subscriber
                return MessageBuilder.createResponse(request,
                        ResponseStatus.BAD_REQUEST);
            }

            mSubscribers.remove(subscriber.mRequest.getRequestId());

            // if there's no more subscriber, stop subscribing topic
            // with kafka consumer
            if (mSubscribers.isEmpty()) {
                mKafkaConsumerOperator.unsubscribeTopic();
            }
        }

        return MessageBuilder.createResponse(request, ResponseStatus.CONTENT,
                ContentFormat.APPLICATION_CBOR, mLatestData);
    }

    public IResponse handlePublishMessage(IRequest request) {
        byte[] data = request.getPayload();

        if (mKafkaProducerOperator.publishMessage(data) == false) {
            return MessageBuilder.createResponse(request,
                    ResponseStatus.INTERNAL_SERVER_ERROR);
        }

        return MessageBuilder.createResponse(request, ResponseStatus.CHANGED);
    }

    public IResponse handleReadMessage(IRequest request) {
        // if consumer is not started, get data from kafka broker
        if (mKafkaConsumerOperator.consumerStarted() == false) {

            ArrayList<byte[]> data = mKafkaConsumerOperator.getMessages();

            if (data != null && !data.isEmpty()) {
                mLatestData = data.get(data.size() - 1);
            }
        }

        return MessageBuilder.createResponse(request, ResponseStatus.CONTENT,
                ContentFormat.APPLICATION_CBOR, mLatestData);
    }

    // callback from Kafka Consumer
    public void onMessagePublished(byte[] message) {

        mLatestData = message;

        notifyPublishedMessage();
    }

    // TODO check
    private Topic getSubtopic(String topicName) {

        if (mSubtopics.containsKey(topicName) == false) {
            return null;
        }

        return mSubtopics.get(topicName);
    }

    private void notifyPublishedMessage() {
        synchronized (mSubscribers) {
            for (TopicSubscriber subscriber : mSubscribers.values()) {
                subscriber.mSubscriber.sendResponse(MessageBuilder
                        .createResponse(subscriber.mRequest,
                                ResponseStatus.CONTENT,
                                ContentFormat.APPLICATION_CBOR, mLatestData));
            }
        }
    }
}