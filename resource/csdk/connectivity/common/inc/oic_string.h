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
#ifndef OIC_STRING_H_
#define OIC_STRING_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/**
 * Duplicates the source string and returns it.
 *
 * NOTE: Caller needs to clear this memory by calling OICFree.
 *
 * @param str - Original valid string which needs to be duplicated
 *
 * @return
 *     on success, a pointer to the duplicated string
 *     on failure, a null pointer is returned
 */
char *OICStrdup(const char *str);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif /* OIC_STRING_H_ */
