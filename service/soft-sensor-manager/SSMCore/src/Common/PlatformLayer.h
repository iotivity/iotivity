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

#define LOCATION_SSM_DB ":memory:"
#define RAPIDXML_STATIC_POOL_SIZE 4*1024

#if defined(WIN32)
#include <Windows.h>
#pragma comment(lib, "../Outputs/sqlite3.lib")

#elif defined(LINUX)
#include <stdio.h>
#include <errno.h>
#include <sys/time.h>
#include <deque>
#include <dlfcn.h>

#elif defined(ANDROID)
#include <android/log.h>

#elif defined(TIZEN)
#include <FBase.h>

#endif

#if defined(WIN32)

#define MODEL_DIRECTORY "../Outputs/"

#define HIGH_LOCATION "../Outputs/HighContextDictionary.xml"
#define USER_DATA_LOCATION "../Outputs/UserDataDictionary.xml"

//#define LOCATION_SSM_DB_DUMP "myBackup.db"
#define LOCATION_SSM_DB_DUMP ""

#elif defined(TIZEN)

#define MODEL_DIRECTORY "/home/developer/ssm/"

#define HIGH_LOCATION "/home/developer/ssm/HighContextDictionary.xml"
#define LOW_LOCATION "/home/developer/ssm/LowContextDictionary.xml"
#define SENSOR_LOCATION "/home/developer/ssm/SensorDictionary.xml"
#define USER_DATA_LOCATION "/home/developer/ssm/UserDataDictionary.xml"

#define LOCATION_SSM_DB_DUMP ""

#elif defined(ANDROID)

#define MODEL_DIRECTORY "/data/data/com.sec.android.ssmcore/files/"

#define HIGH_LOCATION "/data/data/com.sec.android.ssmcore/files/HighContextDictionary.xml"
#define LOW_LOCATION "/data/data/com.sec.android.ssmcore/files/LowContextDictionary.xml"
#define SENSOR_LOCATION "/data/data/com.sec.android.ssmcore/files/SensorDictionary.xml"
#define USER_DATA_LOCATION "/data/data/com.sec.android.ssmcore/files/UserDataDictionary.xml"

#define LOCATION_SSM_DB_DUMP ""

#elif defined(LINUX)

//#define MODEL_DIRECTORY "/home/iotivity/Desktop/Project/Iotivity-Candidate/oic-service/Data_Management/SoftSensorManager/Outputs/"

//#define HIGH_LOCATION "/home/iotivity/Desktop/Project/Iotivity-Candidate/oic-service/Data_Management/SoftSensorManager/Outputs/HighContextDictionary.xml"
#define LOW_LOCATION "/home/iotivity/Desktop/LowContextDictionary.xml"
#define SENSOR_LOCATION "/home/iotivity/Desktop/SensorDictionary.xml"
#define USER_DATA_LOCATION "/home/iotivity/Desktop/UserDataDictionary.xml"

#define LOCATION_SSM_DB_DUMP ""

#endif

#if defined(WIN32) || defined(LINUX)

#define REPORT_MESSAGE(tag, msg) {printf("[%s] %s\n", tag, msg);}
#define LOGV(...)  printf(__VA_ARGS__)
#define LOGW(...)  printf(__VA_ARGS__)
#define LOGE(...)  printf(__VA_ARGS__)

#define SSM_VOID_ASSERT(Exp, STRErrorMsg) \
	{ \
	if(!(Exp)) \
		{ \
		printf("[SSM] %s:%d Return message: %s\n", __FUNCTION__, __LINE__, STRErrorMsg); \
		assert(0); \
		return; \
		} \
	}

#define SSM_RESULT_ASSERT(Exp, STRErrorMsg, Result) \
	{ \
	if(!(Exp)) \
		{ \
		printf("[SSM] %s:%d Return message: %s\n", __FUNCTION__, __LINE__, STRErrorMsg); \
		assert(0); \
		return Result; \
		} \
	}

#define SSM_CLEANUP_ASSERT(Exp) \
	{ \
	if((res = (Exp)) != SSM_S_OK) \
		{ \
		printf("[SSM] %s:%d Return message: %s\n", __FUNCTION__, __LINE__, GetSSMError(res)); \
		assert(0); \
		goto CLEANUP; \
		} \
	}

#define SSM_CLEANUP_COND_ASSERT(Exp, Cond, STRErrorMsg) \
	{ \
	if(Exp != Cond) \
		{ \
		printf("[SSM] %s:%d Return message: %s\n", __FUNCTION__, __LINE__, STRErrorMsg); \
		assert(0); \
		goto CLEANUP; \
		} \
	}

#define SSM_CLEANUP_NULL_ASSERT(Val) \
	{ \
	if(!(Val)) \
		{ \
		printf("[SSM] %s:%d Return message: NULL value\n", __FUNCTION__, __LINE__); \
		assert(Val); \
		goto CLEANUP; \
		} \
	}

#elif defined(ANDROID)

void ReportMessage(const char *tag, const char *msg);
#define REPORT_MESSAGE(tag, msg) {ReportMessage(tag, msg);}
#define LOG_TAG2 "JACK"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG2, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG2, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG2, __VA_ARGS__)

#define SSM_VOID_ASSERT(Exp, STRErrorMsg) \
	{ \
		if(!(Exp)) \
		{ \
			__android_log_print(ANDROID_LOG_ERROR, "[SSM]", "%s:%d Return message: %s", __PRETTY_FUNCTION__, __LINE__, STRErrorMsg); \
			assert(0); \
			return; \
		} \
	}

#define SSM_RESULT_ASSERT(Exp, STRErrorMsg, Result) \
	{ \
		if(!(Exp)) \
		{ \
			__android_log_print(ANDROID_LOG_ERROR, "[SSM]", "%s:%d Return message: %s", __PRETTY_FUNCTION__, __LINE__, STRErrorMsg); \
			assert(0); \
			return Result; \
		} \
	}

#define SSM_CLEANUP_ASSERT(Exp) \
	{ \
		if((res = (Exp)) != SSM_S_OK) \
		{ \
			__android_log_print(ANDROID_LOG_ERROR, "[SSM]", "%s:%d Return message: %s", __PRETTY_FUNCTION__, __LINE__, GetSSMError(res)); \
			assert(0); \
			goto CLEANUP; \
		} \
	}

#define SSM_CLEANUP_COND_ASSERT(Exp, Cond, STRErrorMsg) \
	{ \
		if(Exp != Cond) \
		{ \
			__android_log_print(ANDROID_LOG_ERROR, "[SSM]", "%s:%d Return message: %s", __PRETTY_FUNCTION__, __LINE__, STRErrorMsg); \
			assert(0); \
			goto CLEANUP; \
		} \
	}

#define SSM_CLEANUP_NULL_ASSERT(Val) \
	{ \
		if(!(Val)) \
		{ \
			__android_log_print(ANDROID_LOG_ERROR, "[SSM]", "%s:%d Return message: NULL value", __PRETTY_FUNCTION__, __LINE__); \
			assert(Val); \
			goto CLEANUP; \
		} \
	}

#elif defined(TIZEN)

#define LOGV(...)  AppLog(__VA_ARGS__)
#define LOGW(...)  AppLog(__VA_ARGS__)
#define LOGE(...)  AppLog(__VA_ARGS__)
#define REPORT_MESSAGE(tag, msg)

#define SSM_VOID_ASSERT(Exp, STRErrorMsg) \
	{ \
	if(!(Exp)) \
		{ \
		AppLog("%s", STRErrorMsg); \
		assert(0); \
		return; \
		} \
	}

#define SSM_RESULT_ASSERT(Exp, STRErrorMsg, Result) \
	{ \
	if(!(Exp)) \
		{ \
		AppLog("%s", STRErrorMsg); \
		assert(0); \
		return Result; \
		} \
	}

#define SSM_CLEANUP_ASSERT(Exp) \
	{ \
	if((res = (Exp)) != SSM_S_OK) \
		{ \
		AppLog("%s", GetSSMError(res)); \
		assert(0); \
		goto CLEANUP; \
		} \
	}

#define SSM_CLEANUP_COND_ASSERT(Exp, Cond, STRErrorMsg) \
	{ \
	if(Exp != Cond) \
		{ \
		AppLog("%s", STRErrorMsg); \
		assert(0); \
		goto CLEANUP; \
		} \
	}

#define SSM_CLEANUP_NULL_ASSERT(Val) \
	{ \
	if(!(Val)) \
		{ \
		AppLog("NULL value"); \
		assert(Val); \
		goto CLEANUP; \
		} \
	}

#endif

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
*		  After using the IBase object, you must call Release method.
*
* @param [in] const OID& objectID - objectID
* @param [out] IBase** ppObject - reference pointer to get instance pointer
* @return SSMRESULT
* @warning      
* @exception    
* @see 
*/
INTERFACE_DECLSPEC SSMRESULT CreateInstance(IN const OID& objectID, OUT IBase** ppObject);

/**
* @fn createGlobalInstance
* @brief Create global instance using given OID (Object ID).
*        The instance is managed by global instance map.
*		  After using the IBase object, you must call Release method.
*
* @param [in] const OID& objectID - objectID
* @param [out] IBase** ppObject - reference pointer to get instance pointer
* @return SSMRESULT
* @warning      
* @exception    
* @see 
*/
INTERFACE_DECLSPEC SSMRESULT CreateGlobalInstance(IN const OID& objectID, OUT IBase** ppObject);

INTERFACE_DECLSPEC SSMRESULT CreateGlobalInstanceRepo();

INTERFACE_DECLSPEC SSMRESULT DestroyGlobalInstanceRepo();

#endif
