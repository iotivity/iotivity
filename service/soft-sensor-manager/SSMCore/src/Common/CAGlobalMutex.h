/******************************************************************
*
* Copyright 2014 Samsung Electronics All Rights Reserved.
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
#ifndef __CA_GLOBALMUTEX_H__
#define __CA_GLOBALMUTEX_H__

#include "ThreadManager.h"


#define CA_GLOBALMUTEX_LOCK 	CAutoMutex<CCAGlobalMutex::MutexType> g_locker(CCAGlobalMutex::get())

class CCAGlobalMutex
{
public:	
	typedef CSimpleMutex MutexType;
	
	static MutexType& get()
	{
		static MutexType m_mutex;
		
		return m_mutex;
	}
};


#endif // __CA_GLOBALMUTEX_H__

