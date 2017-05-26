/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
#include <iostream>
#include <list>
#include <PayloadConverter.h>
#include <PropertyBundle.h>
#include <CommonException.h>
#include "oic_malloc.h"
#include "oic_string.h"
#include "octypes.h"
#include "ocpayload.h"

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * @class PayloadConverter
             *
             * @brief This class contains a set of functions to convert a PropertyBundle or
             * OCRepPayload.
             */
            class PayloadConverter
            {
            public:
                OCRepPayload* getPayload() const;
                void setPayload(const OCPayload* payload);
                void setPayload(const OCRepPayload* rep);
                std::list<PropertyBundle>& getPropertybundles();
                void addPropertyBundle(const PropertyBundle& rep);

            private:
                OCRepPayload* createPayload(const PropertyBundle& bundle) const;
                void createPayloadArray(const PropertyBundle& bundle, OCRepPayload* payload,
                                        std::string key, PropertyType type) const;
                void createPropertyBundle(PropertyBundle& bundle, const OCRepPayload* pl);
                void createPropertyBundleArray(PropertyBundle& bundle, const OCRepPayloadValue* pl);
                size_t calcArrayDepth(const size_t dimensions[MAX_REP_ARRAY_DEPTH]);
                template<typename T>
                void payload_array_helper(PropertyBundle& bundle, const OCRepPayloadValue* pl,
                                          size_t depth);
                template<typename T>
                T payload_array_helper_copy(size_t index, const OCRepPayloadValue* pl);

            private:
                std::list<PropertyBundle> m_bundles;
            };

            OCPayload* Converter::convertBundleToPayload(const PropertyBundle& bundle)
            {
                PayloadConverter converter;
                converter.addPropertyBundle(bundle);
                return reinterpret_cast<OCPayload*>(converter.getPayload());
            }

            bool Converter::convertPayloadToBundle(const OCPayload *payload, PropertyBundle& bundle)
            {
                if (payload == NULL || payload->type != PAYLOAD_TYPE_REPRESENTATION)
                {
                    return false;
                }

                // Convert OCPayload to PropertyBundle.
                PayloadConverter converter;
                converter.setPayload(payload);

                // If there is no bundle in coverter, start and end are the same.
                std::list<PropertyBundle>::iterator iter;
                iter = converter.getPropertybundles().begin();
                if (iter != converter.getPropertybundles().end())
                {
                    // first one is considered the root.
                    bundle = static_cast<PropertyBundle&> (*iter);
                }
                return true;
            }

            std::list<PropertyBundle>& PayloadConverter::getPropertybundles()
            {
                return m_bundles;
            }

            void PayloadConverter::addPropertyBundle(const PropertyBundle& bundle)
            {
                m_bundles.push_back(bundle);
            }

            void PayloadConverter::setPayload(const OCPayload* payload)
            {
                if (payload == NULL)
                {
                    return;
                }

                switch(payload->type)
                {
                    case PAYLOAD_TYPE_REPRESENTATION:
                        setPayload(reinterpret_cast<const OCRepPayload*>(payload));
                        break;
                    default:
                        throw CommonException("Invalid Payload type in setPayload");
                        break;
                }
            }

            void PayloadConverter::setPayload(const OCRepPayload* payload)
            {
                const OCRepPayload* pl = payload;
                while (pl)
                {
                    PropertyBundle cur;
                    createPropertyBundle(cur, pl);
                    this->addPropertyBundle(cur);

                    pl = pl->next;
                }
            }

            OCRepPayload* PayloadConverter::getPayload() const
            {
                OCRepPayload* root = NULL;

                std::list<PropertyBundle>::const_iterator iter = m_bundles.begin();
                while (iter != m_bundles.end())
                {
                    if (!root)
                    {
                        root = createPayload(*iter);
                    }
                    else
                    {
                        OCRepPayloadAppend(root, createPayload(*iter));
                    }
                    ++iter;
                }
                return root;
            }

            void PayloadConverter::createPropertyBundle(PropertyBundle& bundle,
                                                        const OCRepPayload* pl)
            {
                OCRepPayloadValue* val = pl->values;

                while (val)
                {
                    switch (val->type)
                    {
                        case OCREP_PROP_INT:
                            bundle.setValue<int>(std::string(val->name), val->i);
                            break;
                        case OCREP_PROP_DOUBLE:
                            bundle.setValue<double>(val->name, val->d);
                            break;
                        case OCREP_PROP_BOOL:
                            bundle.setValue<bool>(val->name, val->b);
                            break;
                        case OCREP_PROP_STRING:
                            bundle.setValue<std::string>(val->name, val->str);
                            break;
                        case OCREP_PROP_BYTE_STRING:
                            {
                                PropByteString data(val->ocByteStr.bytes, val->ocByteStr.len);
                                bundle.setValue<PropByteString>(val->name, data);
                            }
                            break;
                        case OCREP_PROP_OBJECT:
                            {
                                PropertyBundle value;
                                createPropertyBundle(value, val->obj);
                                bundle.setValue<PropertyBundle>(val->name, value);
                            }
                            break;
                        case OCREP_PROP_ARRAY:
                            createPropertyBundleArray(bundle, val);
                            break;
                        default:
                            throw CommonException("Invalid Payload type in createPropertyBundle");
                    }
                    val = val->next;
                }
            }

            // TODO: we should allow multiple depth.
            size_t PayloadConverter::calcArrayDepth(const size_t dimensions[MAX_REP_ARRAY_DEPTH])
            {
                if (dimensions[0] == 0)
                {
                    throw CommonException("Invalid calcArrayDepth");
                }
                return 1;
            }

            template<typename T>
            T PayloadConverter::payload_array_helper_copy(size_t index,
                                                          const OCRepPayloadValue* pl)
            {
                throw CommonException("Unsupported array type");
            }

            template<>
            int PayloadConverter::payload_array_helper_copy<int>(size_t index,
                                                                 const OCRepPayloadValue* pl)
            {
                return pl->arr.iArray[index];
            }

            template<>
            double PayloadConverter::payload_array_helper_copy<double>(size_t index,
                                                                const OCRepPayloadValue* pl)
            {
                return pl->arr.dArray[index];
            }

            template<>
            bool PayloadConverter::payload_array_helper_copy<bool>(size_t index,
                                                                   const OCRepPayloadValue* pl)
            {
                return pl->arr.bArray[index];
            }

            template<>
            std::string PayloadConverter::payload_array_helper_copy<std::string>(size_t index,
                                                                    const OCRepPayloadValue* pl)
            {
                if (pl->arr.strArray[index])
                {
                    return std::string(pl->arr.strArray[index]);
                }
                else
                {
                    return std::string();
                }
            }

            template<>
            PropByteString PayloadConverter::payload_array_helper_copy<PropByteString>(
                                                        size_t index, const OCRepPayloadValue *pl)
            {
                PropByteString result;
                if (pl->arr.ocByteStrArray[index].len)
                {
                    result.bytes = (pl->arr.ocByteStrArray[index]).bytes;
                    result.len = (pl->arr.ocByteStrArray[index]).len;
                }
                return result;
            }

            template<>
            PropertyBundle PayloadConverter::payload_array_helper_copy<PropertyBundle>(
                                                         size_t index, const OCRepPayloadValue* pl)
            {
                PropertyBundle bundle;
                if (pl->arr.objArray[index])
                {
                    createPropertyBundle(bundle, pl->arr.objArray[index]);
                }
                return bundle;
            }

            template <typename T>
            void PayloadConverter::payload_array_helper(PropertyBundle& bundle,
                                                        const OCRepPayloadValue* pl,
                                                        size_t depth)
            {
                if (depth == 1)
                {
                    std::list<T> val;
                    typename std::list<T>::iterator iter = val.begin();
                    for (size_t i = 0; i < pl->arr.dimensions[0]; ++i)
                    {
                        val.push_back(payload_array_helper_copy<T>(i, pl));
                    }
                    bundle.setValue(std::string(pl->name), val);
                }
                else
                {
                    throw CommonException("Unsupported depth in payload_array_helper");
                }
            }

            void PayloadConverter::createPropertyBundleArray(PropertyBundle& bundle,
                                                             const OCRepPayloadValue* pl)
            {
                size_t depth = calcArrayDepth(pl->arr.dimensions);;
                switch (pl->arr.type)
                {
                    case OCREP_PROP_INT:
                        payload_array_helper<int>(bundle, pl, depth);
                        break;
                    case OCREP_PROP_DOUBLE:
                        payload_array_helper<double>(bundle, pl, depth);
                        break;
                    case OCREP_PROP_BOOL:
                        payload_array_helper<bool>(bundle, pl, depth);
                        break;
                    case OCREP_PROP_STRING:
                        payload_array_helper<std::string>(bundle, pl, depth);
                        break;
                    case OCREP_PROP_BYTE_STRING:
                        payload_array_helper<PropByteString>(bundle, pl, depth);
                        break;
                    case OCREP_PROP_OBJECT:
                        payload_array_helper<PropertyBundle>(bundle, pl, depth);
                        break;
                    default:
                        throw CommonException("Invalid array type in createPropertyBundleArray");
                }
            }

            OCRepPayload* PayloadConverter::createPayload(const PropertyBundle& bundle) const
            {
                OCRepPayload* root = OCRepPayloadCreate();
                if (!root)
                {
                    throw CommonException("Memory allocation failed");
                }

                std::map<std::string, PropertyValue*>::const_iterator iter;
                iter = bundle.m_values.begin();
                while (iter != bundle.m_values.end())
                {
                    PropertyType type = iter->second->getType();
                    switch (type)
                    {
                        case Integer:
                            OCRepPayloadSetPropInt(root, iter->first.c_str(),
                                                   ((PropertyValue_<int>*)iter->second)
                                                    ->getValue());
                            break;
                        case Double:
                            OCRepPayloadSetPropDouble(root, iter->first.c_str(),
                                                      ((PropertyValue_<double>*)iter->second)
                                                       ->getValue());
                            break;
                        case Boolean:
                            OCRepPayloadSetPropBool(root, iter->first.c_str(),
                                                    ((PropertyValue_<bool>*)iter->second)
                                                     ->getValue());
                            break;
                        case String:
                            OCRepPayloadSetPropString(root, iter->first.c_str(),
                                                      ((PropertyValue_<std::string>*)iter->second)
                                                       ->getValue().c_str());
                            break;
                        case ByteString:
                            {
                                PropByteString bytesData = ((PropertyValue_<PropByteString>*)
                                                            iter->second)->getValue();
                                OCByteString blob;
                                blob.bytes = bytesData.bytes;
                                blob.len = sizeof(bytesData.bytes);
                                OCRepPayloadSetPropByteString(root, iter->first.c_str(), blob);
                            }
                            break;
                        case PropertyBundle_Object:
                            OCRepPayloadSetPropObjectAsOwner(root, iter->first.c_str(),
                                                    createPayload(((PropertyValue_<PropertyBundle>*)
                                                                  iter->second)->getValue()));
                            break;
                        case Integer_Array:
                        case Double_Array:
                        case Boolean_Array:
                        case String_Array:
                        case ByteString_Array:
                        case PropertyValue_Array:
                            createPayloadArray(bundle, root, iter->first, type);
                            break;
                        case Unknown:
                            throw CommonException("Invalid Payload type in createPayload");
                    }
                    ++iter;
                }

                return root;
            }

            struct get_payload_array
            {
                size_t dimensions[MAX_REP_ARRAY_DEPTH];
                size_t root_size;
                size_t dimTotal;
                void* array;

                template <typename T>
                size_t root_size_calc()
                {
                    return sizeof(T);
                }

                template<typename T>
                void copy_to_array(T item, void* array, size_t pos)
                {
                    ((T*)array)[pos] = item;
                }

                template <typename T>
                void allocate_memory(std::list<T> value)
                {
                    root_size = root_size_calc<T>();
                    dimensions[0] = value.size();
                    dimensions[1] = 0;
                    dimensions[2] = 0;
                    dimTotal = calcDimTotal(dimensions);

                    array = (void*) OICMalloc(dimTotal * root_size);

                    typename std::list<T>::iterator iter = value.begin();
                    for (size_t i = 0; i < dimensions[0]; ++i,++iter)
                    {
                        copy_to_array(*iter, array, i);
                    }
                }
            };

            template<>
            size_t get_payload_array::root_size_calc<int>()
            {
                return sizeof(int64_t);
            }

            template<>
            size_t get_payload_array::root_size_calc<std::string>()
            {
                return sizeof(char*);
            }

            template<typename T>
            void copy_to_array(T item, void* array, size_t pos)
            {
                ((T*)array)[pos] = item;
            }

            template<>
            void get_payload_array::copy_to_array(int item, void* array, size_t pos)
            {
                ((int64_t*)array)[pos] = item;
            }

            template<>
            void get_payload_array::copy_to_array(std::string item, void* array, size_t pos)
            {
                ((char**)array)[pos] = OICStrdup(item.c_str());
            }

            template<>
            void get_payload_array::copy_to_array(PropByteString item, void *array, size_t pos)
            {
                ((OCByteString *)array)[pos].bytes = item.bytes;
                ((OCByteString *)array)[pos].len = item.len;
            }

            template<>
            void get_payload_array::copy_to_array(PropertyBundle item, void* array, size_t pos)
            {
                PayloadConverter converter;
                converter.addPropertyBundle(item);
                ((OCRepPayload**)array)[pos] = converter.getPayload();
            }

            void PayloadConverter::createPayloadArray(const PropertyBundle& bundle,
                                                      OCRepPayload* payload,
                                                      std::string key, PropertyType type) const
            {
                get_payload_array arr;
                std::map<std::string, PropertyValue*>::const_iterator iter;
                iter = bundle.m_values.find(key);
                if (iter == bundle.m_values.end())
                {
                    throw CommonException("Invalid property key in createPayloadArray");
                }

                switch (type)
                {
                    case Integer_Array:
                        arr.allocate_memory(((PropertyValue_<std::list<int> >*)
                                            iter->second)->getValue());
                        OCRepPayloadSetIntArrayAsOwner(payload, key.c_str(),
                                                       (int64_t*)arr.array,
                                                       arr.dimensions);
                        break;
                    case Double_Array:
                        arr.allocate_memory(((PropertyValue_<std::list<double> >*)
                                             iter->second)->getValue());
                        OCRepPayloadSetDoubleArrayAsOwner(payload, key.c_str(),
                                                          (double*)arr.array,
                                                          arr.dimensions);
                        break;
                    case Boolean_Array:
                        arr.allocate_memory(((PropertyValue_<std::list<bool> >*)
                                            iter->second)->getValue());
                        OCRepPayloadSetBoolArrayAsOwner(payload, key.c_str(),
                                                        (bool*)arr.array,
                                                        arr.dimensions);
                        break;
                    case String_Array:
                        arr.allocate_memory(((PropertyValue_<std::list<std::string> >*)
                                             iter->second)->getValue());
                        OCRepPayloadSetStringArrayAsOwner(payload, key.c_str(),
                                                          (char**)arr.array,
                                                          arr.dimensions);
                        break;
                    case ByteString_Array:
                        arr.allocate_memory(((PropertyValue_<std::list<PropByteString> >*)
                                            iter->second)->getValue());
                        OCRepPayloadSetByteStringArrayAsOwner(payload, key.c_str(),
                                                        (OCByteString *)arr.array, arr.dimensions);
                        break;
                    case PropertyValue_Array:
                        arr.allocate_memory(((PropertyValue_<std::list<PropertyBundle> >*)
                                            iter->second)->getValue());
                        OCRepPayloadSetPropObjectArrayAsOwner(payload, key.c_str(),
                                                        (OCRepPayload**)arr.array, arr.dimensions);
                        break;
                    default:
                        throw CommonException("Invalid Payload type in createPayloadArray");
                }
            }
        }
    }
}
