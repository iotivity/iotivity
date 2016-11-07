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

import org.iotivity.cloud.mqserver.kafka.KafkaCommonWrapper;

public class TopicManager {

    private ArrayList<Topic>   mTopics              = new ArrayList<>();

    // for Kafka
    String                     mKafkaZookeeper      = null;
    String                     mKafkaBroker         = null;

    private KafkaCommonWrapper mKafkaCommonOperator = null;

    public boolean createTopic(Topic topic) {

        if (mKafkaCommonOperator.createTopic(topic.getName()) == false) {
            return false;
        }

        synchronized (mTopics) {
            mTopics.add(topic);
        }

        return true;
    }

    public boolean removeTopic(Topic topic) {

        return removeTopics(topic.getName());
    }

    public ArrayList<String> getTopicList() {

        ArrayList<String> topicList = new ArrayList<>();

        synchronized (mTopics) {
            for (Topic topic : mTopics) {
                topicList.add(topic.getName());
            }
        }

        return topicList;
    }

    public ArrayList<String> getTopicListByType(String type) {

        ArrayList<String> topicList = new ArrayList<>();

        synchronized (mTopics) {

            for (Topic topic : mTopics) {
                if (type.equals(topic.getType())) {
                    topicList.add(topic.getName());
                }
            }
        }

        return topicList;
    }

    public Topic getTopic(String topicName) {

        Topic foundTopic = null;

        synchronized (mTopics) {

            for (Topic topic : mTopics) {
                if (topicName.equals(topic.getName())) {
                    foundTopic = topic;
                    break;
                }
            }
        }

        return foundTopic;
    }

    public void setKafkaInformation(String zookeeper, String broker) {
        mKafkaZookeeper = zookeeper;
        mKafkaBroker = broker;

        mKafkaCommonOperator = new KafkaCommonWrapper(zookeeper, broker);
    }

    public String getKafkaZookeeper() {
        return mKafkaZookeeper;
    }

    public String getKafkaBroker() {
        return mKafkaBroker;
    }

    private boolean removeTopics(String topicName) {

        synchronized (mTopics) {

            for (Topic topic : new ArrayList<>(mTopics)) {

                if (topic.getName().startsWith(topicName)) {

                    if (mKafkaCommonOperator.deleteTopic(topic.getName()) == false) {
                        return false;
                    }

                    mTopics.remove(topic);
                }
            }
        }

        return true;
    }

}