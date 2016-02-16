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

#ifndef PIN_CALLBACK_DEF_H_
#define PIN_CALLBACK_DEF_H_

#ifdef __cplusplus
 extern "C" {
#endif // __cplusplus

#define OXM_RANDOM_PIN_SIZE 8

/**
 * Function pointer to print pin code.
 */
typedef void (*GeneratePinCallback)(char* pinData, size_t pinSize);

/**
 * Function pointer to input pin code.
 */
typedef void (*InputPinCallback)(char* pinBuf, size_t bufSize);

/**
 * Function to setting generate PIN callback from user.
 *
 * @param pinCB implementation of generate PIN callback.
 */
void SetGeneratePinCB(GeneratePinCallback pinCB);

/**
 * Function to setting input PIN callback from user.
 *
 * @param pinCB implementation of input PIN callback.
 */
void SetInputPinCB(InputPinCallback pinCB);

/**
 * Function to generate random PIN.
 * This function will send generated PIN to user via callback.
 *
 * @param pinBuffer is the reference to the buffer to store the generated PIN data.
 * @param bufferSize is the size of buffer.
 *
 * @return ::OC_STACK_SUCCESS in case of success or other value in case of error.
 */
OCStackResult GeneratePin(char* pinBuffer, size_t bufferSize);

/**
 * Function to input PIN callback via input callback.
 *
 * @param[in,out] pinBuffer is the reference to the buffer to store the inputed PIN data.
 * @param[in] bufferSize is the size of buffer.
 *
 * @return ::OC_STACK_SUCCESS in case of success or other value in ccase of error.
 */
OCStackResult InputPin(char* pinBuffer, size_t bufferSize);

#ifdef __cplusplus
}
#endif

#endif //PIN_CALLBACK_DEF_H_
