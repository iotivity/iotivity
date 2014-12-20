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

#ifndef WEBSERVICE_REST_ENGINE_EXPORT_H_
#define WEBSERVICE_REST_ENGINE_EXPORT_H_
#pragma once
#if defined(COMPONENT_BUILD)
#if defined(WIN32)
#if defined(REST_ENGINE_IMPLEMENTATION)
#define REST_ENGINE_EXPORT __declspec(dllexport)
#else
#define REST_ENGINE_EXPORT __declspec(dllimport)
#endif  // defined(COMPONENT_RESOURCE_HANDLER_IMPLEMENTATION)
#else  // defined(WIN32)
#define REST_ENGINE_EXPORT __attribute__((visibility("default")))
#endif
#else  // defined(COMPONENT_BUILD)
#define REST_ENGINE_EXPORT
#endif
#endif  // WEBSERVICE_REST_ENGINE_EXPORT_H_

