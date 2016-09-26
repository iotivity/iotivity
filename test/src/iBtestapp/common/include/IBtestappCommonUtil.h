#ifndef INCLUDE_IBTESTAPP_COMMONUTIL_H
#define INCLUDE_IBTESTAPP_COMMONUTIL_H

#include <iostream>
#include <signal.h>
#include <string>
#include <map>
#include <cstdlib>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <unistd.h>

#define IBTESTAPP_CALLBACK_TIMEOUT 10

using namespace std;

class IBtestappCommonUtil
{
private:
        static IBtestappCommonUtil* s_IBtestappCommonUtilInstance;
        static std::mutex s_mutex;

public:
        IBtestappCommonUtil(void);
        ~IBtestappCommonUtil(void);

        static bool isTestAppCommonUtilServerResponsed;

    /**
     * @brief 	function for getting class instance
     * @return	class instance
     */
        static IBtestappCommonUtil* getInstance(void);

        static void * executeThread(void *threadID);
        static void waitForServerResponse();
};

#endif
