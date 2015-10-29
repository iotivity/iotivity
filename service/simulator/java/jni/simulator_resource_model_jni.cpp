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

#include "simulator_resource_model_jni.h"
#include "simulator_error_codes.h"
#include "simulator_exceptions_jni.h"
#include "simulator_utils_jni.h"
#include "jni_string.h"

class JniTypeInfo
{
    public:
        JniTypeInfo(JNIEnv *env) : m_env(env) {}

        SimulatorResourceModel::TypeInfo toCpp(jobject jAttributeValue)
        {
            static jmethodID typeInfoMID = m_env->GetMethodID(gSimulatorClassRefs.attributeValueCls,
                                           "typeInfo", "()Lorg/oic/simulator/AttributeValue$TypeInfo;");
            static jfieldID typeFID = m_env->GetFieldID(gSimulatorClassRefs.attributeTypeInfoCls,
                                      "mType", "Lorg/oic/simulator/AttributeValue$ValueType;");
            static jfieldID baseTypeFID = m_env->GetFieldID(gSimulatorClassRefs.attributeTypeInfoCls,
                                          "mBaseType", "Lorg/oic/simulator/AttributeValue$ValueType;");
            static jfieldID depthFID = m_env->GetFieldID(gSimulatorClassRefs.attributeTypeInfoCls,
                                       "mDepth", "I");

            jobject jTypeInfo = m_env->CallObjectMethod(jAttributeValue, typeInfoMID);
            jobject jType = m_env->GetObjectField(jTypeInfo, typeFID);
            jobject jBaseType = m_env->GetObjectField(jTypeInfo, baseTypeFID);
            jint jDepth = m_env->GetIntField(jTypeInfo, depthFID);

            return SimulatorResourceModel::TypeInfo(getValueType(jType),
                                                    getValueType(jBaseType), jDepth);
        }

    private:
        SimulatorResourceModel::ValueType getValueType(jobject jValueType)
        {
            static jmethodID ordinalMID = m_env->GetMethodID(
                                              gSimulatorClassRefs.attributeValueTypeCls, "ordinal", "()I");

            int ordinal = m_env->CallIntMethod(jValueType, ordinalMID);
            return SimulatorResourceModel::ValueType(ordinal);
        }

        JNIEnv *m_env;
};

class ValueConverterJava : public boost::static_visitor<jobject>
{
    public:
        ValueConverterJava(JNIEnv *env) : m_env(env) {}

        jobject operator ()(const int &value)
        {
            static jmethodID integerCtor =
                m_env->GetMethodID(gSimulatorClassRefs.integerCls, "<init>", "(I)V");
            return m_env->NewObject(gSimulatorClassRefs.integerCls,
                                    integerCtor, value);
        }

        jobject operator ()(const double &value)
        {
            static jmethodID doubleCtor =
                m_env->GetMethodID(gSimulatorClassRefs.doubleCls, "<init>", "(D)V");
            return m_env->NewObject(gSimulatorClassRefs.doubleCls,
                                    doubleCtor, value);
        }

        jobject operator ()(const bool &value)
        {
            static jmethodID booleanCtor =
                m_env->GetMethodID(gSimulatorClassRefs.booleanCls, "<init>", "(Z)V");
            return m_env->NewObject(gSimulatorClassRefs.booleanCls,
                                    booleanCtor, value);
        }

        jobject operator ()(const std::string &value)
        {
            jstring stringValue = m_env->NewStringUTF(value.c_str());
            return static_cast<jobject>(stringValue);
        }

        jobject operator ()(const SimulatorResourceModel &value)
        {
            return simulatorResourceModelToJava(m_env, const_cast<SimulatorResourceModel &>(value));
        }

        template <typename T>
        jobject operator ()(const std::vector<T> &values)
        {
            jobjectArray jArray = m_env->NewObjectArray(values.size(), getClass(values), nullptr);
            if (!jArray)
                return nullptr;

            for (size_t index = 0; index < values.size(); index++)
            {
                jobject element = operator()(values[index]);
                m_env->SetObjectArrayElement(jArray, index, element);
            }

            return jArray;
        }

        template <typename T>
        jobject operator ()(const std::vector<std::vector<T>> &values)
        {
            jobjectArray jArray = m_env->NewObjectArray(values.size(), getClass(values), nullptr);
            if (!jArray)
                return nullptr;

            for (size_t index = 0; index < values.size(); index++)
            {
                jobject element = operator()(values[index]);
                m_env->SetObjectArrayElement(jArray, index, element);
            }

            return jArray;
        }

        template <typename T>
        jobject operator ()(const std::vector<std::vector<std::vector<T>>> &values)
        {
            jobjectArray jArray = m_env->NewObjectArray(values.size(), getClass(values), nullptr);
            if (!jArray)
                return nullptr;

            for (size_t index = 0; index < values.size(); index++)
            {
                jobject element = operator()(values[index]);
                m_env->SetObjectArrayElement(jArray, index, element);
            }

            return jArray;
        }

    private:
        jclass getClass(const std::vector<int> &)
        {
            return gSimulatorClassRefs.integer1DArrayCls;
        }

        jclass getClass(const std::vector<std::vector<int>> &)
        {
            return gSimulatorClassRefs.integer2DArrayCls;
        }

        jclass getClass(const std::vector<std::vector<std::vector<int>>> &)
        {
            return gSimulatorClassRefs.integer3DArrayCls;
        }

        jclass getClass(const std::vector<double> &)
        {
            return gSimulatorClassRefs.double1DArrayCls;
        }

        jclass getClass(const std::vector<std::vector<double>> &)
        {
            return gSimulatorClassRefs.double2DArrayCls;
        }

        jclass getClass(const std::vector<std::vector<std::vector<double>>> &)
        {
            return gSimulatorClassRefs.double3DArrayCls;
        }

        jclass getClass(const std::vector<bool> &)
        {
            return gSimulatorClassRefs.boolean1DArrayCls;
        }

        jclass getClass(const std::vector<std::vector<bool>> &)
        {
            return gSimulatorClassRefs.boolean2DArrayCls;
        }

        jclass getClass(const std::vector<std::vector<std::vector<bool>>> &)
        {
            return gSimulatorClassRefs.boolean3DArrayCls;
        }

        jclass getClass(const std::vector<std::string> &)
        {
            return gSimulatorClassRefs.string1DArrayCls;
        }

        jclass getClass(const std::vector<std::vector<std::string>> &)
        {
            return gSimulatorClassRefs.string2DArrayCls;
        }

        jclass getClass(const std::vector<std::vector<std::vector<std::string>>> &)
        {
            return gSimulatorClassRefs.string3DArrayCls;
        }

        jclass getClass(const std::vector<SimulatorResourceModel> &)
        {
            return gSimulatorClassRefs.simulatorResModel1DArrayCls;
        }

        jclass getClass(const std::vector<std::vector<SimulatorResourceModel>> &)
        {
            return gSimulatorClassRefs.simulatorResModel2DArrayCls;
        }

        jclass getClass(const std::vector<std::vector<std::vector<SimulatorResourceModel>>> &)
        {
            return gSimulatorClassRefs.simulatorResModel3DArrayCls;
        }

        JNIEnv *m_env;
};

class ValueConverterCpp
{
    public:
        ValueConverterCpp(JNIEnv *env, SimulatorResourceModel::TypeInfo &typeInfo,
                          jobject &value) : m_env(env), m_typeInfo(typeInfo), m_value(value) {}

        void convert()
        {
            switch (m_typeInfo.baseType())
            {
                case SimulatorResourceModel::ValueType::INTEGER:
                    return handleByDepth<int>();
                case SimulatorResourceModel::ValueType::DOUBLE:
                    return handleByDepth<double>();
                case SimulatorResourceModel::ValueType::BOOLEAN:
                    return handleByDepth<bool>();
                case SimulatorResourceModel::ValueType::STRING:
                    return handleByDepth<std::string>();
                case SimulatorResourceModel::ValueType::RESOURCE_MODEL:
                    return handleByDepth<SimulatorResourceModel>();
                case SimulatorResourceModel::ValueType::VECTOR:
                case SimulatorResourceModel::ValueType::UNKNOWN:
                    break;
            }
        }

        SimulatorResourceModel::ValueVariant get()
        {
            return std::move(m_result);
        }

    private:
        template <typename T>
        void handleByDepth()
        {
            if (0 == m_typeInfo.depth())
            {
                T value;
                getValue(m_value, value);
                m_result = value;
            }
            else if (1 == m_typeInfo.depth())
            {
                std::vector<T> value;
                getValue(m_value, value);
                m_result = value;
            }
            else if (2 == m_typeInfo.depth())
            {
                std::vector<std::vector<T>> value;
                getValue(m_value, value);
                m_result = value;
            }
            else if (3 == m_typeInfo.depth())
            {
                std::vector<std::vector<std::vector<T>>> value;
                getValue(m_value, value);
                m_result = value;
            }
        }

        void getValue(jobject &jValue, int &value)
        {
            static jmethodID intValueMID = m_env->GetMethodID(
                                               gSimulatorClassRefs.integerCls, "intValue", "()I");

            jint temp = m_env->CallIntMethod(jValue, intValueMID);
            value = temp;
        }

        void getValue(jobject &jValue, std::vector<int> &value)
        {
            jobjectArray array = (jobjectArray) jValue;
            size_t length = m_env->GetArrayLength(array);
            for (size_t i = 0; i < length; i++)
            {
                jobject jElement = m_env->GetObjectArrayElement(array, i);

                int element;
                getValue(jElement, element);
                value.push_back(element);
            }
        }

        void getValue(jobject &jValue, std::vector<std::vector<int>> &value)
        {
            jobjectArray array = (jobjectArray) jValue;
            size_t length = m_env->GetArrayLength(array);
            for (size_t i = 0; i < length; i++)
            {
                jobject jElement = m_env->GetObjectArrayElement(array, i);

                std::vector<int> element;
                getValue(jElement, element);
                value.push_back(element);
            }
        }

        void getValue(jobject &jValue, std::vector<std::vector<std::vector<int>>> &value)
        {
            jobjectArray array = (jobjectArray) jValue;
            size_t length = m_env->GetArrayLength(array);
            for (size_t i = 0; i < length; i++)
            {
                jobject jElement = m_env->GetObjectArrayElement(array, i);

                std::vector<std::vector<int>> element;
                getValue(jElement, element);
                value.push_back(element);
            }
        }

        void getValue(jobject &jValue, double &value)
        {
            static jmethodID doubleValueMID = m_env->GetMethodID(
                                                  gSimulatorClassRefs.doubleCls, "doubleValue", "()D");

            value = m_env->CallDoubleMethod(jValue, doubleValueMID);
        }

        void getValue(jobject &jValue, std::vector<double> &value)
        {
            jobjectArray array = (jobjectArray) jValue;
            size_t length = m_env->GetArrayLength(array);
            for (size_t i = 0; i < length; i++)
            {
                jobject jElement = m_env->GetObjectArrayElement(array, i);

                double element;
                getValue(jElement, element);
                value.push_back(element);
            }
        }

        void getValue(jobject &jValue, std::vector<std::vector<double>> &value)
        {
            jobjectArray array = (jobjectArray) jValue;
            size_t length = m_env->GetArrayLength(array);
            for (size_t i = 0; i < length; i++)
            {
                jobject jElement = m_env->GetObjectArrayElement(array, i);

                std::vector<double> element;
                getValue(jElement, element);
                value.push_back(element);
            }
        }

        void getValue(jobject &jValue, std::vector<std::vector<std::vector<double>>> &value)
        {
            jobjectArray array = (jobjectArray) jValue;
            size_t length = m_env->GetArrayLength(array);
            for (size_t i = 0; i < length; i++)
            {
                jobject jElement = m_env->GetObjectArrayElement(array, i);

                std::vector<std::vector<double>> element;
                getValue(jElement, element);
                value.push_back(element);
            }
        }

        void getValue(jobject &jValue, bool &value)
        {
            static jmethodID boolValueMID = m_env->GetMethodID(
                                                gSimulatorClassRefs.booleanCls, "booleanValue", "()Z");

            value = m_env->CallBooleanMethod(jValue, boolValueMID);
        }

        void getValue(jobject &jValue, std::vector<bool> &value)
        {
            jobjectArray array = (jobjectArray) jValue;
            size_t length = m_env->GetArrayLength(array);
            for (size_t i = 0; i < length; i++)
            {
                jobject jElement = m_env->GetObjectArrayElement(array, i);

                bool element;
                getValue(jElement, element);
                value.push_back(element);
            }
        }

        void getValue(jobject &jValue, std::vector<std::vector<bool>> &value)
        {
            jobjectArray array = (jobjectArray) jValue;
            size_t length = m_env->GetArrayLength(array);
            for (size_t i = 0; i < length; i++)
            {
                jobject jElement = m_env->GetObjectArrayElement(array, i);

                std::vector<bool> element;
                getValue(jElement, element);
                value.push_back(element);
            }
        }

        void getValue(jobject &jValue, std::vector<std::vector<std::vector<bool>>> &value)
        {
            jobjectArray array = (jobjectArray) jValue;
            size_t length = m_env->GetArrayLength(array);
            for (size_t i = 0; i < length; i++)
            {
                jobject jElement = m_env->GetObjectArrayElement(array, i);

                std::vector<std::vector<bool>> element;
                getValue(jElement, element);
                value.push_back(element);
            }
        }

        void getValue(jobject &jValue, std::string &value)
        {
            jstring stringValue = (jstring) jValue;
            JniString jniValue(m_env, stringValue);
            value = jniValue.get();
        }

        void getValue(jobject jValue, std::vector<std::string> &value)
        {
            jobjectArray array = (jobjectArray) jValue;
            size_t length = m_env->GetArrayLength(array);
            for (size_t i = 0; i < length; i++)
            {
                jobject jElement = m_env->GetObjectArrayElement(array, i);

                std::string element;
                getValue(jElement, element);
                value.push_back(element);
            }
        }

        void getValue(jobject &jValue, std::vector<std::vector<std::string>> &value)
        {
            jobjectArray array = (jobjectArray) jValue;
            size_t length = m_env->GetArrayLength(array);
            for (size_t i = 0; i < length; i++)
            {
                jobject jElement = m_env->GetObjectArrayElement(array, i);

                std::vector<std::string> element;
                getValue(jElement, element);
                value.push_back(element);
            }
        }

        void getValue(jobject &jValue, std::vector<std::vector<std::vector<std::string>>> &value)
        {
            jobjectArray array = (jobjectArray) jValue;
            size_t length = m_env->GetArrayLength(array);
            for (size_t i = 0; i < length; i++)
            {
                jobject jElement = m_env->GetObjectArrayElement(array, i);

                std::vector<std::vector<std::string>> element;
                getValue(jElement, element);
                value.push_back(element);
            }
        }

        void getValue(jobject &jValue, SimulatorResourceModel &value)
        {
        }

        void getValue(jobject &jValue, std::vector<SimulatorResourceModel> &value)
        {
        }

        void getValue(jobject &jValue, std::vector<std::vector<SimulatorResourceModel>> &value)
        {
        }

        void getValue(jobject &jValue, std::vector<std::vector<std::vector<SimulatorResourceModel>>> &value)
        {
        }

        JNIEnv *m_env;
        SimulatorResourceModel::TypeInfo &m_typeInfo;
        jobject &m_value;
        SimulatorResourceModel::ValueVariant m_result;
};

class JniAttributeValue
{
    public:
        static jobject toJava(JNIEnv *env, SimulatorResourceModel::Attribute &attribute)
        {
            auto value = attribute.getValue();
            return toJava(env, value);
        }

        static jobject toJava(JNIEnv *env, SimulatorResourceModel::ValueVariant &value)
        {
            ValueConverterJava converter(env);
            jobject jValue =  boost::apply_visitor(converter, value);

            static jmethodID attrValueCtor = env->GetMethodID(
                                                 gSimulatorClassRefs.attributeValueCls, "<init>", "(Ljava/lang/Object;)V");

            return env->NewObject(gSimulatorClassRefs.attributeValueCls, attrValueCtor, jValue);
        }

        static SimulatorResourceModel::ValueVariant toCpp(JNIEnv *env, jobject &jAttributeValue)
        {
            static jmethodID getMID = env->GetMethodID(gSimulatorClassRefs.attributeValueCls,
                                      "get", "()Ljava/lang/Object;");

            SimulatorResourceModel::TypeInfo typeInfo = JniTypeInfo(env).toCpp(jAttributeValue);
            jobject jValue = env->CallObjectMethod(jAttributeValue, getMID);

            ValueConverterCpp converter(env, typeInfo, jValue);
            converter.convert();
            return converter.get();
        }
};

class JniAttributeProperty
{
    public:
        static jobject toJava(JNIEnv *env,
                              SimulatorResourceModel::AttributeProperty &property)
        {
            if (SimulatorResourceModel::AttributeProperty::Type::RANGE == property.type())
            {
                static jmethodID propertyCtor = env->GetMethodID(
                                                    gSimulatorClassRefs.attributePropertyCls, "<init>", "(DD)V");

                return env->NewObject(gSimulatorClassRefs.attributePropertyCls, propertyCtor,
                                      property.min(), property.max());
            }
            else
            {
                static jmethodID propertyCtor = env->GetMethodID(
                                                    gSimulatorClassRefs.attributePropertyCls, "<init>", "([Lorg/oic/simulator/AttributeValue;)V");

                jobjectArray jValueSet = env->NewObjectArray(property.valueSetSize(),
                                         gSimulatorClassRefs.attributeValueCls, nullptr);
                int index = 0;
                for (auto &value : property.valueSet())
                {
                    jobject jValue = JniAttributeValue::toJava(env, value);
                    env->SetObjectArrayElement(jValueSet, index++, jValue);
                }

                return env->NewObject(gSimulatorClassRefs.attributePropertyCls, propertyCtor,
                                      jValueSet);
            }

            return nullptr;
        }

        static SimulatorResourceModel::AttributeProperty toCpp(JNIEnv *env, jobject jAttributeProperty)
        {
            static jfieldID typeFID = env->GetFieldID(gSimulatorClassRefs.attributePropertyCls,
                                      "mType", "Lorg/oic/simulator/AttributeProperty$Type;");
            static jfieldID minFID = env->GetFieldID(gSimulatorClassRefs.attributePropertyCls,
                                     "mMin", "D");
            static jfieldID maxFID = env->GetFieldID(gSimulatorClassRefs.attributePropertyCls,
                                     "mMax", "D");
            static jfieldID valueSetFID = env->GetFieldID(gSimulatorClassRefs.attributePropertyCls,
                                          "mValueSet", "[Lorg/oic/simulator/AttributeValue;");
            static jmethodID ordinalMID = env->GetMethodID(
                                              gSimulatorClassRefs.attributePropertyTypeCls, "ordinal", "()I");

            jobject jType = env->GetObjectField(jAttributeProperty, typeFID);
            jdouble jMin = env->GetDoubleField(jAttributeProperty, minFID);
            jdouble jMax = env->GetDoubleField(jAttributeProperty, maxFID);
            jobjectArray jValueSet = (jobjectArray) env->GetObjectField(jAttributeProperty, valueSetFID);

            int ordinal = env->CallIntMethod(jType, ordinalMID);
            switch (SimulatorResourceModel::AttributeProperty::Type(ordinal))
            {
                case SimulatorResourceModel::AttributeProperty::Type::RANGE:
                    {
                        return SimulatorResourceModel::AttributeProperty(jMin, jMax);
                    }

                case SimulatorResourceModel::AttributeProperty::Type::VALUE_SET:
                    {
                        std::vector<SimulatorResourceModel::ValueVariant> valueSet;
                        size_t length = env->GetArrayLength(jValueSet);
                        for (size_t i = 0; i < length; i++)
                        {
                            jobject jAttributeValue = env->GetObjectArrayElement(jValueSet, i);
                            valueSet.push_back(JniAttributeValue::toCpp(env, jAttributeValue));
                        }

                        return SimulatorResourceModel::AttributeProperty(valueSet);
                    }
            }

            return SimulatorResourceModel::AttributeProperty();
        }
};

static jobject createHashMap(JNIEnv *env)
{
    static jmethodID hashMapCtor = env->GetMethodID(
                                       gSimulatorClassRefs.hashMapCls, "<init>", "()V");
    return env->NewObject(gSimulatorClassRefs.hashMapCls, hashMapCtor);
}

static void addEntryToHashMap(JNIEnv *env, jobject mapobj, jobject key, jobject value)
{
    if (!mapobj || !key || !value)
        return;

    static jmethodID hashMapPutMethod = env->GetMethodID(gSimulatorClassRefs.hashMapCls,
                                        "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    env->CallObjectMethod(mapobj, hashMapPutMethod, key, value);
}

jobject simulatorResourceModelToJava(JNIEnv *env, SimulatorResourceModel &resModel)
{
    jobject attributesMap = createHashMap(env);
    jobject propertiesMap = createHashMap(env);
    if (!attributesMap || !propertiesMap)
        return nullptr;

    for (auto &attributeEntry : resModel.getAttributes())
    {
        jstring jAttrName = env->NewStringUTF((attributeEntry.first).c_str());
        jobject jAttributeValue = JniAttributeValue::toJava(env, attributeEntry.second);
        addEntryToHashMap(env, attributesMap, jAttrName, jAttributeValue);

        jobject jAttributeProperty = JniAttributeProperty::toJava(env, attributeEntry.second.getProperty());
        addEntryToHashMap(env, propertiesMap, jAttrName, jAttributeProperty);
    }

    static jmethodID simulatorResourceModelCtor = env->GetMethodID(
                gSimulatorClassRefs.simulatorResourceModelCls, "<init>", "(Ljava/util/Map;Ljava/util/Map;)V");

    return env->NewObject(gSimulatorClassRefs.simulatorResourceModelCls,
                          simulatorResourceModelCtor, attributesMap, propertiesMap);
}

jobject simulatorResourceAttributeToJava(JNIEnv *env, SimulatorResourceModel::Attribute &attribute)
{
    static jmethodID simulatorResAttributeCtor = env->GetMethodID(
                gSimulatorClassRefs.simulatorResourceAttributeCls, "<init>",
                "(Ljava/lang/String;Lorg/oic/simulator/AttributeValue;Lorg/oic/simulator/AttributeProperty;)V");

    jstring jAttrName = env->NewStringUTF(attribute.getName().c_str());
    jobject jAttributeValue = JniAttributeValue::toJava(env, attribute);
    jobject jAttributeProperty = JniAttributeProperty::toJava(env, attribute.getProperty());

    return env->NewObject(gSimulatorClassRefs.simulatorResourceAttributeCls,
                          simulatorResAttributeCtor, jAttrName, jAttributeValue, jAttributeProperty);
}

bool simulatorResourceModelToCpp(JNIEnv *env, jobject jResModel, SimulatorResourceModel &resModel)
{
    if (!jResModel)
        return false;

    static jfieldID valuesFID = env->GetFieldID(gSimulatorClassRefs.simulatorResourceModelCls,
                                "mValues", "Ljava/util/Map;");
    static jfieldID propertiesFID = env->GetFieldID(gSimulatorClassRefs.simulatorResourceModelCls,
                                    "mProperties", "Ljava/util/Map;");
    static jmethodID entrySetMID = env->GetMethodID(gSimulatorClassRefs.mapCls, "entrySet",
                                   "()Ljava/util/Set;");
    static jmethodID iteratorMID = env->GetMethodID(gSimulatorClassRefs.setCls, "iterator",
                                   "()Ljava/util/Iterator;");
    static jmethodID hasNextMID = env->GetMethodID(gSimulatorClassRefs.iteratorCls, "hasNext",
                                  "()Z");
    static jmethodID nextMID = env->GetMethodID(gSimulatorClassRefs.iteratorCls, "next",
                               "()Ljava/lang/Object;");
    static jmethodID getKeyMID = env->GetMethodID(gSimulatorClassRefs.mapEntryCls, "getKey",
                                 "()Ljava/lang/Object;");
    static jmethodID getValueMID = env->GetMethodID(gSimulatorClassRefs.mapEntryCls, "getValue",
                                   "()Ljava/lang/Object;");

    jobject jValues = env->GetObjectField(jResModel, valuesFID);
    jobject jProperties = env->GetObjectField(jResModel, propertiesFID);

    if (jValues)
    {
        jobject entrySet = env->CallObjectMethod(jValues, entrySetMID);
        jobject iterator = env->CallObjectMethod(entrySet, iteratorMID);
        if (entrySet && iterator)
        {
            while (env->CallBooleanMethod(iterator, hasNextMID))
            {
                jobject entry = env->CallObjectMethod(iterator, nextMID);
                jstring key = (jstring) env->CallObjectMethod(entry, getKeyMID);
                jobject value = env->CallObjectMethod(entry, getValueMID);
                resModel.add(JniString(env, key).get(), JniAttributeValue::toCpp(env, value));

                env->DeleteLocalRef(entry);
                env->DeleteLocalRef(key);
                env->DeleteLocalRef(value);
            }
        }
    }

    if (jProperties)
    {
        jobject entrySet = env->CallObjectMethod(jProperties, entrySetMID);
        jobject iterator = env->CallObjectMethod(entrySet, iteratorMID);
        if (entrySet && iterator)
        {
            while (env->CallBooleanMethod(iterator, hasNextMID))
            {
                jobject entry = env->CallObjectMethod(iterator, nextMID);
                jstring key = (jstring) env->CallObjectMethod(entry, getKeyMID);
                jobject value = env->CallObjectMethod(entry, getValueMID);
                resModel.setAttributeProperty(JniString(env, key).get(),
                                              JniAttributeProperty::toCpp(env, value));

                env->DeleteLocalRef(entry);
                env->DeleteLocalRef(key);
                env->DeleteLocalRef(value);
            }
        }
    }

    return true;
}

bool simulatorResourceAttributeToCpp(JNIEnv *env, jobject jAttribute,
                                     SimulatorResourceModel::Attribute &attribute)
{
    if (!jAttribute)
        return false;

    static jfieldID nameFID = env->GetFieldID(gSimulatorClassRefs.simulatorResourceAttributeCls,
                              "mName", "Ljava/lang/String;");
    static jfieldID valueFID = env->GetFieldID(gSimulatorClassRefs.simulatorResourceAttributeCls,
                               "mValue", "Lorg/oic/simulator/AttributeValue;");
    static jfieldID propertyFID = env->GetFieldID(gSimulatorClassRefs.simulatorResourceAttributeCls,
                                  "mProperty", "Lorg/oic/simulator/AttributeProperty;");

    jstring jAttributeName = (jstring) env->GetObjectField(jAttribute, nameFID);
    jobject jAttributeValue = env->GetObjectField(jAttribute, valueFID);
    jobject jAttributeProperty = env->GetObjectField(jAttribute, propertyFID);

    if (!jAttributeName || !jAttributeValue)
        return false;

    JniString attrName(env, jAttributeName);
    SimulatorResourceModel::ValueVariant value = JniAttributeValue::toCpp(env, jAttributeValue);

    attribute.setName(attrName.get());
    attribute.setValue(value);
    if (jAttributeProperty)
    {
        SimulatorResourceModel::AttributeProperty property = JniAttributeProperty::toCpp(env,
                jAttributeProperty);
        attribute.setProperty(property);
    }

    return true;
}

bool AttributeValueToCpp(JNIEnv *env, jobject jAttributeValue,
                         SimulatorResourceModel::ValueVariant &value)
{
    if (!jAttributeValue)
        return false;

    value = JniAttributeValue::toCpp(env, jAttributeValue);
    return true;
}

