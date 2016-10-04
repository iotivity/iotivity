#include "cloudCommon.h"
#include "logger.h"

#define TAG "cloudClient"

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

    OCMode mode = OC_CLIENT;
    if (OC_STACK_OK != startRequestsThread(&mode))
    {
        return -4;
    }

    startProcess();

    freeThreadResources();

    return 0;
}
