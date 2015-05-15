///*
//* //******************************************************************
//* //
//* // Copyright 2015 Intel Corporation.
//* //
//* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//* //
//* // Licensed under the Apache License, Version 2.0 (the "License");
//* // you may not use this file except in compliance with the License.
//* // You may obtain a copy of the License at
//* //
//* //      http://www.apache.org/licenses/LICENSE-2.0
//* //
//* // Unless required by applicable law or agreed to in writing, software
//* // distributed under the License is distributed on an "AS IS" BASIS,
//* // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//* // See the License for the specific language governing permissions and
//* // limitations under the License.
//* //
//* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//*/
//
//#include "JniListenerManager.h"
//
//#include "JniOnGetListener.h"
//
//template<class T>
//T* JniListenerManager<T>::addListener(JNIEnv* env, jobject jListener)
//{
//	T *onEventListener = NULL;
//
//	m_mapMutex.lock();
//
//	for (auto it = m_listenerMap.begin(); it != m_listenerMap.end(); ++it)
//	{
//		if (env->IsSameObject(jListener, it->first))
//		{
//			auto refPair = it->second;
//			onEventListener = refPair.first;
//			refPair.second++;
//			it->second = refPair;
//			m_listenerMap.insert(*it);
//			LOGD("OnEventListener: ref. count is incremented");
//			break;
//		}
//	}
//
//	if (!onEventListener)
//	{
//		onEventListener = new T(env, jListener, this);
//		jobject jgListener = env->NewGlobalRef(jListener);
//
//		m_listenerMap.insert(std::pair<jobject, std::pair<T*, int>>(jgListener, std::pair<T*, int>(onEventListener, 1)));
//		LOGD("OnEventListener: new listener");
//	}
//
//	m_mapMutex.unlock();
//
//	return onEventListener;
//}
//
//template<class T>
//void JniListenerManager<T>::removeListener(JNIEnv* env, jobject jListener)
//{
//	m_mapMutex.lock();
//
//	bool isFound = false;
//
//	for (auto it = m_listenerMap.begin(); it != m_listenerMap.end(); ++it)
//	{
//		if (env->IsSameObject(jListener, it->first))
//		{
//			auto refPair = it->second;
//			if (refPair.second > 1)
//			{
//				refPair.second--;
//				it->second = refPair;
//				m_listenerMap.insert(*it);
//				LOGI("OnEventListener: ref. count is decremented");
//			}
//			else
//			{
//				env->DeleteGlobalRef(it->first);
//				T* listener = refPair.first;
//				delete listener;
//				m_listenerMap.erase(it);
//
//				LOGI("OnEventListener is removed");
//			}
//
//			isFound = true;
//			break;
//		}
//	}
//
//	if (!isFound)
//	{
//		ThrowOcException(JNI_EXCEPTION, "OnEventListener isn't found");
//	}
//
//	m_mapMutex.unlock();
//}
//
