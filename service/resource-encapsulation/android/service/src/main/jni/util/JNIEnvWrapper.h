/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

#ifndef RCS_JNI_ENV_WRAPPER_H_
#define RCS_JNI_ENV_WRAPPER_H_

#include <jni.h>

#include <exception>

#include "JavaLocalRef.h"

/**
 * @class JavaException
 */
class JavaException: public std::exception
{
};

/*
 * @class JNIEnvWrapper
 */
class JNIEnvWrapper
{
public:
    JNIEnvWrapper() noexcept : m_env{ } {}
    JNIEnvWrapper(JNIEnv* env) noexcept : m_env{ env } {}

    JNIEnvWrapper& operator=(JNIEnv* env) noexcept
    {
        m_env = env;
        return *this;
    }

    /**
     *  check the object is same or not
     *  compare the lhs and rhs object
     */
    jboolean IsSameObject(jobject lhs, jobject rhs)
    {
        return m_env->IsSameObject(lhs, rhs);
    }

    /* get object class
     * @param obj   object
     */
    jclass GetObjectClass(jobject obj)
    {
        auto ret = m_env->GetObjectClass(obj);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /**
     * This method is to find the class
     * @param name    name of class to find
     * @return the class
     */
    jclass FindClass(const char* name)
    {
        auto ret = m_env->FindClass(name);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /**
     * This method is to find class as global reference
     * @param name   name of reference to find
     * @return the new reference class
     */
    jclass FindClassAsGlobalRef(const char* name)
    {
        JavaLocalClass cls{ m_env, FindClass(name) };

        return NewGlobalRef(cls);
    }

    /**
     * This method is to create new global reference
     * @pram obj   new object to create
     * @return the new object
     */
    jobject NewGlobalRef(jobject obj)
    {
        auto ret = m_env->NewGlobalRef(obj);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    template<typename T>
    T NewGlobalRef(T obj)
    {
        auto ret = static_cast< T >(m_env->NewGlobalRef(obj));
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    template<typename T>
    T NewGlobalRef(const JavaLocalRef< T >& obj)
    {
        auto ret = static_cast< T >(m_env->NewGlobalRef(obj));
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /** This method is to delete global reference */
    void DeleteGlobalRef(jobject obj)
    {
        m_env->DeleteGlobalRef(obj);
    }

    /**
     * This method is to create new object
     */
    jobject NewObject(jclass cls, jmethodID ctor, ...)
    {
        va_list args;
        va_start(args, ctor);
        auto ret = m_env->NewObjectV(cls, ctor,args);
        va_end(args);

        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /** new string UTF */
    jstring NewStringUTF(const char* str)
    {
        auto ret = m_env->NewStringUTF(str);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /** get string UTF chars */
    const char* GetStringUTFChars(jstring str, jboolean* isCopy)
    {
        auto ret = m_env->GetStringUTFChars(str, isCopy);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /** release string UTF chars */
    void ReleaseStringUTFChars(jstring str, const char* chars)
    {
        m_env->ReleaseStringUTFChars(str, chars);
        if (m_env->ExceptionCheck()) throw JavaException();
    }

    /** get constructor Id */
    jmethodID GetConstructorID(jclass cls, const char* sig)
    {
        auto ret = m_env->GetMethodID(cls, "<init>", sig);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /** get method Id */
    jmethodID GetMethodID(jclass cls, const char* name, const char* sig)
    {
        auto ret = m_env->GetMethodID(cls, name, sig);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /** get static method Id */
    jmethodID GetStaticMethodID(jclass cls, const char* name, const char* sig)
    {
        auto ret = m_env->GetStaticMethodID(cls, name, sig);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /** get field Id */
    jfieldID GetFieldID(jclass cls, const char* name, const char* sig)
    {
        auto ret = m_env->GetFieldID(cls, name, sig);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /** get static field Id */
    jfieldID GetStaticFieldID(jclass cls, const char* name, const char* sig)
    {
        auto ret = m_env->GetStaticFieldID(cls, name, sig);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /** get static object field */
    jobject GetStaticObjectField(jclass cls, jfieldID fieldId)
    {
        auto ret = m_env->GetStaticObjectField(cls, fieldId);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /* get static object filed */
    jobject GetStaticObjectField(jclass cls, const char* name, const char* sig)
    {
        return GetStaticObjectField(cls, GetStaticFieldID(cls, name, sig));
    }

    /** get Int filed */
    jint GetIntField(jobject obj, jfieldID fieldId)
    {
        auto ret = m_env->GetIntField(obj, fieldId);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /** get long field */
    jlong GetLongField(jobject obj, jfieldID fieldId)
    {
        auto ret = m_env->GetLongField(obj, fieldId);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /** set long field */
    void SetLongField(jobject obj, jfieldID fieldId, jlong val)
    {
        m_env->SetLongField(obj, fieldId, val);
        if (m_env->ExceptionCheck()) throw JavaException();
    }

    /** get object field */
    jobject GetObjectField(jobject obj, jfieldID fieldId)
    {
        auto ret = m_env->GetObjectField(obj, fieldId);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /** call static Int method */
    jint CallStaticIntMethod(jclass cls, jmethodID methodId, ...)
    {
        va_list args;
        va_start(args, methodId);
        auto ret = m_env->CallStaticIntMethodV(cls, methodId, args);
        va_end(args);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /** call static object method */
    jobject CallStaticObjectMethod(jclass cls, jmethodID methodId, ...)
    {
        va_list args;
        va_start(args, methodId);
        auto ret = m_env->CallStaticObjectMethodV(cls, methodId, args);
        va_end(args);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /** call void method */
    void CallVoidMethod(jobject obj, jmethodID methodId, ...)
    {
         va_list args;
         va_start(args, methodId);
         m_env->CallVoidMethodV(obj, methodId, args);
         va_end(args);
         if (m_env->ExceptionCheck()) throw JavaException();
    }

    /** call boolean method */
    jboolean CallBooleanMethod(jobject obj, jmethodID methodId, ...)
    {
        va_list args;
        va_start(args, methodId);
        auto ret = m_env->CallBooleanMethodV(obj, methodId, args);
        va_end(args);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /** call Int method */
    jint CallIntMethod(jobject obj, jmethodID methodId, ...)
    {
         va_list args;
         va_start(args, methodId);
         auto ret = m_env->CallIntMethod(obj, methodId, args);
         va_end(args);
         if (m_env->ExceptionCheck()) throw JavaException();
         return ret;
    }

     /** call double method */
     jdouble CallDoubleMethod(jobject obj, jmethodID methodId, ...)
     {
          va_list args;
          va_start(args ,methodId);
          auto ret = m_env->CallDoubleMethod(obj, methodId, args);
          va_end(args);
          if (m_env->ExceptionCheck()) throw JavaException();
          return ret;
     }

    /** call object method */
    jobject CallObjectMethod(jobject obj, jmethodID methodId, ...)
    {
        va_list args;
        va_start(args, methodId);
        auto ret = m_env->CallObjectMethodV(obj, methodId, args);
        va_end(args);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /*
     * create new boolean array
     *
     * @param len   size of array
     */
    jbooleanArray NewBooleanArray(jsize len)
    {
        auto ret = m_env->NewBooleanArray(len);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /*
     * create new Int array
     *
     * @param len   size of array
     */
    jintArray NewIntArray(jsize len)
    {
        auto ret = m_env->NewIntArray(len);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /*
     * create new double array
     *
     * @param len   size of array
     */
    jdoubleArray NewDoubleArray(jsize len)
    {
        auto ret = m_env->NewDoubleArray(len);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /*
     * create new byte array
     *
     * @param len   size of array
     */
    jbyteArray NewByteArray(jsize len)
    {
        auto ret = m_env->NewByteArray(len);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

  /* new object array */
  jobjectArray NewObjectArray(jsize len, jclass cls, jobject init)
    {
        auto ret = m_env->NewObjectArray(len, cls, init);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /**
     * get array length
     *
     * @param array   find length of this array
     *
     * @return size of array
     */
    jsize GetArrayLength(jarray array)
    {
        auto ret = m_env->GetArrayLength(array);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /** get object array element */
    jobject GetObjectArrayElement(jobjectArray array, jsize index)
    {
        auto ret = m_env->GetObjectArrayElement(array, index);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /** get byte array element */
    jbyte *GetByteArrayElements(jbyteArray array, jboolean *value)
    {
        auto ret = m_env->GetByteArrayElements(array, value);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /** set object array element */
    void SetObjectArrayElement(jobjectArray array, jsize index, jobject val)
    {
        m_env->SetObjectArrayElement(array, index, val);
        if (m_env->ExceptionCheck()) throw JavaException();
    }

    /** set boolean array region */
    void SetBooleanArrayRegion(jbooleanArray array, jsize start, jsize len, const jboolean* buf)
    {
        m_env->SetBooleanArrayRegion(array, start, len, buf);
        if (m_env->ExceptionCheck()) throw JavaException();
    }

    /** set Integer array region */
    void SetIntArrayRegion(jintArray array, jsize start, jsize len, const jint* buf)
    {
        m_env->SetIntArrayRegion(array, start, len, buf);
        if (m_env->ExceptionCheck()) throw JavaException();
    }

    /** set double array region */
    void SetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len, const jdouble* buf)
    {
        m_env->SetDoubleArrayRegion(array, start, len, buf);
        if (m_env->ExceptionCheck()) throw JavaException();
    }

    /** set byte array region */
    void SetByteArrayRegion(jbyteArray array, jsize start, jsize len, const jbyte *buf)
    {
        m_env->SetByteArrayRegion(array, start, len, buf);
        if (m_env->ExceptionCheck()) throw JavaException();
    }

    /** get primitive array critical */
    void* GetPrimitiveArrayCritical(jarray array, jboolean* isCopy)
    {
        auto ret = m_env->GetPrimitiveArrayCritical(array, isCopy);
        if (m_env->ExceptionCheck()) throw JavaException();
        return ret;
    }

    /** release primitive array critical */
    void ReleasePrimitiveArrayCritical(jarray array, void* carray, int mode)
    {
        m_env->ReleasePrimitiveArrayCritical(array, carray, mode);
        if (m_env->ExceptionCheck()) throw JavaException();
    }

    /** release byte array element */
    void ReleaseByteArrayElements(jbyteArray array, jbyte* byteArray, int mode)
    {
        m_env->ReleaseByteArrayElements(array, byteArray, mode);
        if (m_env->ExceptionCheck()) throw JavaException();
    }

    /** throw exception */
    void ThrowNew(jclass cls, const char* msg)
    {
        m_env->ThrowNew(cls, msg);
        throw JavaException();
    }

    /** exception describe */
    void ExceptionDescribe() const noexcept
    {
        m_env->ExceptionDescribe();
    }

    /** exception clear */
    void ExceptionClear() noexcept
    {
        m_env->ExceptionClear();
    }

    /** exception check */
    jboolean ExceptionCheck() const noexcept
    {
        return m_env->ExceptionCheck();
    }

    operator bool() const noexcept
    {
        return m_env != nullptr;
    }

    /** get environment */
    JNIEnv* get() const
    {
        return m_env;
    }

private:
    JNIEnv* m_env;
};



#endif // RCS_JNI_ENV_WRAPPER_H_
