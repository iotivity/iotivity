#include <stdio.h>

#include <unistd.h>
#include "NSCommon.h"
#include "NSConsumerInterface.h"
#include "ocstack.h"

void onNotificationPosted(NSProvider * provider, NSMessage * notification)
{
    printf("Notification from : %s:%d\n", ((OCDevAddr *)provider->mUserData)->addr,
            ((OCDevAddr *)provider->mUserData)->port);
    printf("id : %s\n", notification->mId);
    printf("title : %s\n", notification->mTitle);
    printf("content : %s\n", notification->mContentText);
//    NSDropNSObject(notification);
    NSRead(notification);
}

void onDiscoverNotification(NSProvider * provider)
{
    printf("notification resource discovered\n");
    printf("subscribe result %d\n", NSSubscribeProvider(provider));
    printf("startSubscribing\n");
}

void onNotificationSync(NSProvider * provider, NSSync * sync)
{
    printf("Sync from : %s:%d\n", ((OCDevAddr *)provider->mUserData)->addr,
            ((OCDevAddr *)provider->mUserData)->port);
    printf("Sync ID : %s\n", sync->mMessageId);
    printf("Sync STATE : %d\n", sync->mState);
}

int main(void)
{

    printf("start Iotivity\n");
    if (OCInit1(OC_CLIENT, OC_DEFAULT_FLAGS, OC_DEFAULT_FLAGS) != OC_STACK_OK)
    {
        printf("OCInit fail\n");
        return 0;
    }



    printf("start notification consumer service\n");
    NSResult ret = NSStartConsumer(
            onDiscoverNotification,
            onNotificationPosted,
            onNotificationSync);
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
