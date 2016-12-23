/* ****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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


#ifndef _NS_SEMAPHORE_WINDOWS_H_
#define _NS_SEMAPHORE_WINDOWS_H_

#ifdef SYSTEM_WINDOWS
#include <winsock2.h>

typedef struct
{
    HANDLE semaphore;
} sem_t;


int sem_init (sem_t *__sem, int pshared, unsigned int value);
int sem_post (sem_t *__sem);
int sem_wait (sem_t *__sem);

#endif

#endif // _NS_SEMAPHORE_WINDOWS_H_
