//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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



#ifndef SUBSCRIPTIONCONFIG_H_
#define SUBSCRIPTIONCONFIG_H_

#if defined(_WIN32) // MS Compiler

#ifdef STATIC_LIB
#       define SUBSCRIPTIONAPI
#else

#   if defined(SUBSCRIPTION_EXPORTS)
#       define SUBSCRIPTIONAPI __declspec(dllexport)
#   else
#       define SUBSCRIPTIONAPI __declspec(dllimport)
#   endif

#endif

#elif defined(__GNUG__)   // GCC
#   define SUBSCRIPTIONAPI __attribute__((visibility("default")))

#elif defined(__ARMCC_VERSION)  // ARM Compiler (RVCT 3.1)
#   define SUBSCRIPTIONAPI

#else
#   define SUBSCRIPTIONAPI

#endif


#endif /* SUBSCRIPTIONCONFIG_H_ */
