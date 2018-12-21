/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef _NS_CONSUMER_QUEUE_H_
#define _NS_CONSUMER_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdbool.h>

/** notification consumer queue object structure */
typedef struct _NSConsumerQueueObject
{
    void * data; /**< data */
    struct _NSConsumerQueueObject * next; /**< pointer to next node */
} NSConsumerQueueObject;

/** notification consumer queue structure */
typedef struct
{
    int size; /**< size of queue */
    NSConsumerQueueObject * head; /**< pointer to first node */
    NSConsumerQueueObject * tail; /**< pointer to last node */
} NSConsumerQueue;

/**
 * create queue
 *
 * @return new created queue
 */
NSConsumerQueue * NSCreateQueue(void);

/**
 * destroy queue
 *
 * @param queue to destroy
 */
void NSDestroyQueue(NSConsumerQueue *);

/**
 * push consumer queue
 *
 * @param head pointer of queue
 * @param queue object to push
 *
 * @return true if push, otherwise false
 */
bool NSPushConsumerQueue(NSConsumerQueue *, NSConsumerQueueObject *);

/**
 * pop queue object
 *
 * @param head of queue
 *
 * @return queue object
 */
NSConsumerQueueObject * NSPopQueue(NSConsumerQueue *);

/**
 * get queue size
 *
 * @param head of queue
 *
 * @return size of queue
 */
int NSGetQueueSize(NSConsumerQueue *);

/**
 * check queue is empty or not
 *
 * @param head of queue
 *
 * @return true or false
 */
bool NSIsQueueEmpty(NSConsumerQueue *);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NS_CONSUMER_QUEUE_H_
