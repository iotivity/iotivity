#include <stdio.h>

#include <unistd.h>
#include "NSCommon.h"
#include "NSConsumerInterface.h"
#include "ocstack.h"

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
