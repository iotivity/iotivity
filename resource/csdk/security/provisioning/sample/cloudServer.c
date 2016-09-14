#include "cloudCommon.h"
#include "logger.h"
#include "cloudResource.h"

#define TAG "cloudServer"

extern LEDResource LED;
extern char *gResourceUri;

int main(int argc, char *argv[])
{
    if (!parseCommandLineArguments(argc, argv))
    {
        return -1;
    }

    if (OC_STACK_OK != initPersistentStorage())
    {
        return -2;
    }

    if (OC_STACK_OK != initProcess(OC_CLIENT_SERVER))
    {
        OIC_LOG(ERROR, TAG, "initProcess error, exit\n");
        return -3;
    }

    LED.state = false;
    LED.power = 0;
    /*
     * Declare and create the example resource: LED
     */
    if (OC_STACK_OK != createLEDResource(gResourceUri, &LED))
    {
        return -4;
    }

    OCMode mode = OC_SERVER;
    if (OC_STACK_OK != startRequestsThread(&mode))
    {
        return -5;
    }

    startProcess();

    freeThreadResources();

    return 0;
}
