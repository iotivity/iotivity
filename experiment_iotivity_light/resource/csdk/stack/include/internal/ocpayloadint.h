//******************************************************************
//
// Copyright 2016 Intel Corporation All Rights Reserved.
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

#ifndef PAYLOADINT_H
#define PAYLOADINT_H

// Device payload
OCStackResult OCBuildDevicePayload(uint8_t *buf, size_t *size, OCDeviceInfo *deviceInfo);

// Platform payload
OCStackResult OCBuildPlatformPayload(uint8_t *buf, size_t *size, OCPlatformInfo *platformInfo);

// Discovery payload
OCStackResult OCBuildDiscoveryPayload(uint8_t *buf, size_t *size, OCResource *resourceList[], int count);

bool IncludeThisResource(OCResource *resource, char *rtFilter, char *ifFilter);

#ifdef WITH_PRESENCE
// Presence payload
OCStackResult OCBuildPresencePayload(uint8_t *buf, size_t *size, uint32_t seqNum,
                                     uint32_t maxAge, OCPresenceTrigger trigger,
                                     const char *resourceType);
#endif

void *plMalloc(size_t size); 
void *plCalloc(size_t nmemb, size_t size); 
char *plStrdup(const char *str);

#endif // PAYLOADINT_H
