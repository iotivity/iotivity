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

#ifndef JNI_VECTOR_H_
#define JNI_VECTOR_H_

#include <jni.h>
#include <vector>
#include <string>

class JniVector
{
    public:
        JniVector(JNIEnv *env);

        template <typename TYPE>
        std::vector<TYPE> toCpp(jobject vector)
        {
            std::vector<TYPE> result;
            jmethodID sizeMethodID = m_env->GetMethodID(m_vectorCls, "size", "()I");

            int size = m_env->CallIntMethod(vector, sizeMethodID);
            addElementsCpp(vector, size, result);
            return result;
        }

        jobject toJava(std::vector<std::string> &cppVector);
        jobject toJava();

    private:
        void addElementsCpp(jobject vector, int size, std::vector<int> &result);
        void addElementsCpp(jobject vector, int size, std::vector<double> &result);
        void addElementsCpp(jobject vector, int size, std::vector<std::string> &result);

        JNIEnv *m_env;
        jclass m_vectorCls;
};

#endif
