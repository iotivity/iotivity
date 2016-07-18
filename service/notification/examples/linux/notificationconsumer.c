#include <stdio.h>
#include <unistd.h>

#include "ocstack.h"
#include "NSCommon.h"
#include "NSConsumerInterface.h"

#ifdef WITH_CLOUD
#include "NSConstants.h"
#include "NSConsumerCommon.h"
#include "cloud_connector.h"
#include "oic_malloc.h"

#define CLOUD_CONTEXT_VALUE 0x99
#define CLOUD_PRESENCE_SUBSCRIBE_QUERY ""          // refer to IoTivity Cloud Module Sample

#define CLOUD_HOST_ADDRESS ""                      // refer to IoTivity Cloud Module Sample
#define CLOUD_IOTIVITYNS_SESSION ""                // refer to IoTivity Cloud Module Sample
#endif

void onDiscoverNotification(NSProvider * provider)
{
    printf("notification resource discovered\n");
    printf("subscribe result %d\n", NSSubscribe(provider));
    printf("startSubscribing\n");
}

void onSubscriptionAccepted(NSProvider * provider)
{
    printf("Subscription accepted\n");
    printf("subscribed provider Id : %s\n", provider->providerId);
}

void onNotificationPosted(NSMessage * notification)
{
    printf("id : %lld\n", notification->messageId);
    printf("title : %s\n", notification->title);
    printf("content : %s\n", notification->contentText);
    printf("source : %s\n", notification->sourceName);
    NSConsumerSendSyncInfo(notification->providerId, notification->messageId, NS_SYNC_READ);
}

void onNotificationSync(NSSyncInfo * sync)
{
    printf("Sync ID : %lld\n", sync->messageId);
    printf("Sync STATE : %d\n", sync->state);
}

#ifdef WITH_CLOUD
OCStackApplicationResult handleLoginoutCB(void *ctx,
        OCDoHandle handle,
        OCClientResponse *clientResponse)
{
    (void)handle;
    if (ctx != (void *)CLOUD_CONTEXT_VALUE)
    {
        NS_LOG(DEBUG, "Invalid Login/out callback received");
    }

    NS_LOG(DEBUG, "Login/out response received");

    if (clientResponse->payload != NULL &&
        clientResponse->payload->type == PAYLOAD_TYPE_REPRESENTATION)
    {
        NS_LOG(DEBUG, "PAYLOAD_TYPE_REPRESENTATION received");

        OCRepPayloadValue *val = ((OCRepPayload *)clientResponse->payload)->values;

        while (val)
        {
            val = val->next;
        }
        NS_LOG(DEBUG, "Get payload values");

        OCDevAddr * addr = NULL;
        addr = (OCDevAddr *) OICMalloc(sizeof(OCDevAddr));
        memcpy(addr, clientResponse->addr, sizeof(OCDevAddr));

        NSTask * task = NSMakeTask(TASK_EVENT_CONNECTED_TCP, addr);

        NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(task, OC_STACK_KEEP_TRANSACTION, NSOICFree(addr));
        NSConsumerPushEvent(task);
    }

    return OC_STACK_KEEP_TRANSACTION;
}
#endif

int main(void)
{

    printf("start Iotivity\n");
    if (OCInit1(OC_CLIENT, OC_DEFAULT_FLAGS, OC_DEFAULT_FLAGS) != OC_STACK_OK)
    {
        printf("OCInit fail\n");
        return 0;
    }

    NSConsumerConfig cfg;
    cfg.discoverCb = onDiscoverNotification;
    cfg.acceptedCb = onSubscriptionAccepted;
    cfg.messageCb = onNotificationPosted;
    cfg.syncInfoCb = onNotificationSync;


    printf("start notification consumer service\n");
    NSResult ret = NSStartConsumer(cfg);
    if(ret != NS_OK)
    {
        printf("error discoverNoti %d\n", ret);
    }

#ifdef WITH_CLOUD
    NS_LOG(DEBUG, "process OCCloudLogin...");
    OCCloudLogin(CLOUD_HOST_ADDRESS, CLOUD_IOTIVITYNS_SESSION, handleLoginoutCB);
    NS_LOG(DEBUG, "OCCloudLogin return");
#endif

    while (true)
    {
        usleep(2000);
        if(OCProcess() != OC_STACK_OK)
        {
            OCStop();
            break;
        }
    }

    return 0;
}
