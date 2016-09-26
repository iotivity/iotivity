#include "IBtestappCommonUtil.h"

std::mutex IBtestappCommonUtil::s_mutex;
bool IBtestappCommonUtil::isTestAppCommonUtilServerResponsed = false;
IBtestappCommonUtil* IBtestappCommonUtil::s_IBtestappCommonUtilInstance = NULL;

IBtestappCommonUtil::IBtestappCommonUtil()
{

}

IBtestappCommonUtil* IBtestappCommonUtil::getInstance(void)
{
        if (s_IBtestappCommonUtilInstance == NULL)
        {
                s_mutex.lock();

                s_IBtestappCommonUtilInstance = new IBtestappCommonUtil();

                s_mutex.unlock();
        }

       return s_IBtestappCommonUtilInstance;
}

IBtestappCommonUtil::~IBtestappCommonUtil()
{
        if(s_IBtestappCommonUtilInstance != NULL)
        {
                delete s_IBtestappCommonUtilInstance;
                s_IBtestappCommonUtilInstance = NULL;
        }
}


void IBtestappCommonUtil::waitForServerResponse()
{
    isTestAppCommonUtilServerResponsed = false;

    void* ret = NULL;
    pthread_t serverlistiningThread;
    pthread_create(&serverlistiningThread, NULL, executeThread, NULL);
    pthread_join (serverlistiningThread, &ret);
}

void*  IBtestappCommonUtil::executeThread(void *arg)
{
    int second = 0;

    cout << "\nWaiting For Server Response..........................................................\n";

    while(!isTestAppCommonUtilServerResponsed)
    {
        sleep(1);
        if (++second == IBTESTAPP_CALLBACK_TIMEOUT)
        {
            cout << "\nTimeout For Server Response!Please Try Again\n\n";
            break;
        }
    }

    isTestAppCommonUtilServerResponsed = false;
    pthread_exit(NULL);
}
