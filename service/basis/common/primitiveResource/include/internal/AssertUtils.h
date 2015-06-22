//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef __PRIMITIVERESOURCE_ASSERTUTILS_H
#define __PRIMITIVERESOURCE_ASSERTUTILS_H

#include <cstdint>

#include <memory>

#include <octypes.h>
#include <OCException.h>

#include <PrimitiveException.h>

namespace OIC
{
    namespace Service
    {
        namespace Detail
        {
            struct NotOCStackResult;

            template< typename RET, typename FUNC, typename ENABLER = void, typename ...PARAMS >
            struct EnableIfReturnTypeIs;

            template< typename FUNC, typename ...PARAMS >
            struct EnableIfReturnTypeIs< OCStackResult, FUNC,
                    typename std::enable_if<
                            std::is_same< typename std::result_of< FUNC(PARAMS...) >::type,
                                    OCStackResult >::value >::type, PARAMS... >
            {
                using type = void;
            };

            template< typename FUNC, typename ...PARAMS >
            struct EnableIfReturnTypeIs< NotOCStackResult, FUNC,
                    typename std::enable_if<
                            !std::is_same< typename std::result_of< FUNC(PARAMS...) >::type,
                                    OCStackResult >::value >::type, PARAMS... >
            {
                using type = typename std::result_of< FUNC(PARAMS...) >::type;
            };


            template< typename A, typename B, typename ENABLER = void >
            struct EnableIfTypeIs;

            template< typename A >
            struct EnableIfTypeIs< A, OCStackResult,
                    typename std::enable_if< std::is_same< A, OCStackResult >::value >::type >
            {
                using type = void;
            };

            template< typename A >
            struct EnableIfTypeIs< A, NotOCStackResult,
                    typename std::enable_if< !std::is_same< A, OCStackResult >::value >::type >
            {
                using type = A;
            };


            template< typename T, typename = typename std::enable_if<
                    std::is_class<T>::value && std::is_pointer<T>::value>::type >
            struct EnableIfClassPointer
            {
                using type = void;
            };

            template< typename T, typename = typename std::enable_if<std::is_class<T>::value > >
             struct EnableIfClass
             {
                 using type = void;
             };
        }

        inline void expectOCStackResult(OCStackResult actual, OCStackResult expected)
        {
            if (actual != expected)
            {
                throw PlatformException(actual);
            }
        }

        inline void expectOCStackResultOK(OCStackResult actual)
        {
            expectOCStackResult(actual, OC_STACK_OK);
        }

        template< typename FUNC,
            typename = typename std::enable_if<std::is_function<FUNC>::value>::type,
            typename ...PARAMS >
        typename Detail::EnableIfReturnTypeIs< OCStackResult, FUNC, PARAMS... >::type
        invokeOC(FUNC&& fn, PARAMS&& ...params)
        {
            try
            {
                expectOCStackResultOK(fn(std::forward< PARAMS >(params)...));
            }
            catch (OC::OCException& e)
            {
                throw PlatformException(e.code());
            }
        }

        template< typename FUNC,
            typename = typename std::enable_if<std::is_function<FUNC>::value>::type,
            typename ...PARAMS >
        typename Detail::EnableIfReturnTypeIs< Detail::NotOCStackResult, FUNC, PARAMS... >::type
        invokeOC(FUNC&& fn, PARAMS&& ...params)
        {
            try
            {
                return fn(std::forward< PARAMS >(params)...);
            }
            catch (OC::OCException& e)
            {
                throw PlatformException(e.code());
            }
        }

        template< typename OBJ, typename = typename Detail::EnableIfClassPointer<OBJ>::type,
                typename FUNC, typename ...PARAMS >
        inline auto invokeOC(OBJ&& obj, FUNC&& fn, PARAMS&& ...params) ->
            typename Detail::EnableIfTypeIs<
                decltype((obj->*fn)(std::forward< PARAMS >(params)...)), OCStackResult>::
                type
        {
            try
            {
                expectOCStackResultOK(obj->*fn(std::forward< PARAMS >(params)...));
            }
            catch (OC::OCException& e)
            {
                throw PlatformException(e.code());
            }
        }

        template< typename OBJ, typename = typename Detail::EnableIfClassPointer<OBJ>::type,
                typename FUNC, typename ...PARAMS >
        inline auto invokeOC(OBJ&& obj, FUNC&& fn, PARAMS&& ...params) ->
                typename Detail::EnableIfTypeIs<
                    decltype((obj->*fn)(std::forward< PARAMS >(params)...)),
                    Detail::NotOCStackResult>::
                    type
        {
            try
            {
                obj->*fn(std::forward< PARAMS >(params)...);
            }
            catch (OC::OCException& e)
            {
                throw PlatformException(e.code());
            }
        }

        template< typename OBJ, typename = typename Detail::EnableIfClass<OBJ>::type,
                typename FUNC, typename ...PARAMS >
        inline auto invokeOC(const std::shared_ptr< OBJ >& obj, FUNC&& fn, PARAMS&& ...params) ->
                typename Detail::EnableIfTypeIs<
                    decltype((obj.get()->*fn)(std::forward< PARAMS >(params)...)), OCStackResult>::
                    type
        {
            try
            {
                expectOCStackResultOK((obj.get()->*fn)(std::forward< PARAMS >(params)...));
            }
            catch (OC::OCException& e)
            {
                throw PlatformException(e.code());
            }
        }

        template< typename OBJ, typename = typename Detail::EnableIfClass< OBJ >::type,
                typename FUNC, typename ...PARAMS >
        inline auto invokeOC(const std::shared_ptr<OBJ>& obj, FUNC&& fn, PARAMS&& ...params) ->
            typename Detail::EnableIfTypeIs<
                   decltype((obj.get()->*fn)(std::forward< PARAMS >(params)...)),
                   Detail::NotOCStackResult>::
                   type
        {
            try
            {
                return (obj.get()->*fn)(std::forward< PARAMS >(params)...);
            }
            catch (OC::OCException& e)
            {
                throw PlatformException(e.code());
            }
        }

    }
}

#endif // __PRIMITIVERESOURCE_ASSERTUTILS_H
