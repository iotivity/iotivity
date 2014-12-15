#ifndef _PlatformLayer_H_
#define _PlatformLayer_H_

#include <sstream>
#include <algorithm>
#include <iterator>
#include <assert.h>
#include <string>
#include <exception>
#include <set>
#include <stdint.h>
#include <string.h>
#include <list>
#include <vector>
#include <map>
#include <fstream>

#if defined(WIN32)

#include <Windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "../Outputs/sqlite3.lib")

#elif defined(LINUX)

#include <stdio.h>
#include <errno.h>
#include <sys/time.h>
#include <deque>
#include <dlfcn.h>
#include <semaphore.h>

#if defined(ANDROID)

#include <android/log.h>

#elif defined(TIZEN)

#endif

#endif


#define LOCATION_SSM_DB ":memory:"
#define RAPIDXML_STATIC_POOL_SIZE 4*1024
#define DEFAULT_PATH_SOFT_SENSORS "SoftSensorDescription.xml"
#define LOCATION_SSM_DB_DUMP ""
//#define LOCATION_SSM_DB_DUMP "myBackup.db"


#if defined(WIN32)

#define PRINT_LOG(strError) printf("[SSM] %s:%d %s\n", __FUNCTION__, __LINE__, strError)
#define REPORT_MESSAGE(tag, msg) printf("[%s] %s\n", tag, msg)

#elif defined(LINUX)

#if defined(ANDROID)

void ReportMessage(const char *tag, const char *msg);
#define REPORT_MESSAGE(tag, msg) ReportMessage(tag, msg)
#define PRINT_LOG(strError) __android_log_print(ANDROID_LOG_ERROR, "[SSM]", "%s:%d %s", __PRETTY_FUNCTION__, __LINE__, strError)

#elif defined(TIZEN)

#define REPORT_MESSAGE(tag, msg) printf("[%s] %s\n", tag, msg)
#define PRINT_LOG(strError) printf("[SSM] %s:%d %s\n", __FUNCTION__, __LINE__, strError)

#else //Default linux

#define REPORT_MESSAGE(tag, msg) printf("[%s] %s\n", tag, msg)
#define PRINT_LOG(strError) printf("[SSM] %s:%d %s\n", __FUNCTION__, __LINE__, strError)

#endif

#endif



#define SSM_VOID_ASSERT(Exp, STRErrorMsg) \
    { \
    if(!(Exp)) \
        { \
        PRINT_LOG(STRErrorMsg); \
        assert(0); \
        return; \
        } \
    }

#define SSM_RESULT_ASSERT(Exp, STRErrorMsg, Result) \
    { \
    if(!(Exp)) \
        { \
        PRINT_LOG(STRErrorMsg); \
        assert(0); \
        return Result; \
        } \
    }

#define SSM_CLEANUP_ASSERT(Exp) \
    { \
    if((res = (Exp)) != SSM_S_OK) \
        { \
        PRINT_LOG(GetSSMError(res)); \
        assert(0); \
        goto CLEANUP; \
        } \
    }

#define SSM_CLEANUP_COND_ASSERT(Exp, Cond, STRErrorMsg) \
    { \
    if(Exp != Cond) \
        { \
        PRINT_LOG(STRErrorMsg); \
        assert(0); \
        goto CLEANUP; \
        } \
    }

#define SSM_CLEANUP_NULL_ASSERT(Val) \
    { \
    if(!(Val)) \
        { \
        PRINT_LOG("NULL value"); \
        assert(Val); \
        goto CLEANUP; \
        } \
    }

#define CLEAN_STACKVARIABLE(val) memset(&val, 0, sizeof(val))
#define SAFE_RELEASE(p) {if(p != NULL){p->release();p = NULL;}else{;/*NULL*/}}
#define SAFE_DELETE(p) {if(p != NULL){delete p;p = NULL;}else{;/*NULL*/}}
#define SAFE_ARRAY_DELETE(p) {if(p != NULL){delete [] p;p = NULL;}else{;/*NULL*/}}

#include "ObjectManager.h"
#include "ThreadManager.h"
#include "SSMInterface/SSMModelDefinition.h"

typedef std::vector<std::string> StringVec;
typedef std::vector<int> IntVec;

/**
* @fn createInstance
* @brief Create instance using given OID (Object ID).
*         After using the IBase object, you must call Release method.
*
* @param [in] const OID& objectID - objectID
* @param [out] IBase** ppObject - reference pointer to get instance pointer
* @return SSMRESULT
* @warning
* @exception
* @see
*/
INTERFACE_DECLSPEC SSMRESULT CreateInstance(IN const OID &objectID, OUT IBase **ppObject);

/**
* @fn createGlobalInstance
* @brief Create global instance using given OID (Object ID).
*        The instance is managed by global instance map.
*         After using the IBase object, you must call Release method.
*
* @param [in] const OID& objectID - objectID
* @param [out] IBase** ppObject - reference pointer to get instance pointer
* @return SSMRESULT
* @warning
* @exception
* @see
*/
INTERFACE_DECLSPEC SSMRESULT CreateGlobalInstance(IN const OID &objectID, OUT IBase **ppObject);

INTERFACE_DECLSPEC SSMRESULT CreateGlobalInstanceRepo();

INTERFACE_DECLSPEC SSMRESULT DestroyGlobalInstanceRepo();

#endif
