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


#ifdef SYSTEM_WINDOWS

#include "NSSemaphore_windows.h"
#include "NSConstants.h"
#include <stdio.h>
#include <winsock2.h>

int sem_init (sem_t * sem, int pshared, unsigned int value)
{
    sem = (sem_t *) OICMalloc(sizeof(sem_t));
    NS_VERIFY_NOT_NULL(sem, (int)-1);

    sem->semaphore = CreateSemaphore(NULL, pshared, value, NULL);
    NS_VERIFY_NOT_NULL(sem->semaphore, (int)-1);

    return 0;
}
int sem_post (sem_t * sem)
{
    NS_VERIFY_NOT_NULL(sem, (int)-1);

    ReleaseSemaphore(sem->semaphore, 1, NULL);

    return 0;
}
int sem_wait (sem_t * sem)
{
    NS_VERIFY_NOT_NULL(sem, (int)-1);

    WaitForSingleObject(sem->semaphore, 0L);

    return 0;
}

#endif
