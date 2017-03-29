/* *****************************************************************
 *
 * Copyright 2017 Microsoft
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

#ifndef OIC_OICPLATFORM_H_
#define OIC_OICPLATFORM_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define OIC_UUID_LENGTH     16

/**
 * Function returns a UUID that can be used for platform ID.
 *
 * @param[in] platformUuid  The generated UUID.
 *
 * @return true if successful.
 */
bool OICGetPlatformUuid(uint8_t platformUuid[OIC_UUID_LENGTH]);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif
