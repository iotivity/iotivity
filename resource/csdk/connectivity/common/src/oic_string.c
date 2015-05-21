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
#include "oic_string.h"

#include <string.h>
#include <assert.h>
#include "logger.h"
#include "oic_malloc.h"

#define TAG "OIC_STRING"
char *OICStrdup(const char *str)
{
    assert(str);

    // Allocate memory for original string length and 1 extra byte for '\0'
    size_t length = strlen(str);
    char *dup = (char *)OICMalloc(length + 1);
    if (NULL != dup)
    {
        memcpy(dup, str, length + 1);
    }
    else
    {
        assert(NULL != dup);
    }

    return dup;
}

char* OICStrcpy(char* dest, size_t destSize, const char* source)
{
    return OICStrcpyPartial(dest, destSize, source, destSize);
}

char* OICStrcat(char* dest, size_t destSize, const char* source)
{
    return OICStrcatPartial(dest, destSize, source, destSize);
}

static size_t min3(size_t a, size_t b, size_t c)
{
    return a < b ? (a < c ? a : c) : (b < c ? b: c);
}

char* OICStrcpyPartial(char* dest, size_t destSize, const char* source, size_t sourceLen)
{
    assert(dest);
    assert(source);

    if(destSize == 0 || sourceLen == 0)
    {
        return dest;
    }

    size_t limit = min3(destSize - 1, sourceLen, strlen(source));
    memcpy(dest, source, limit);
    dest[limit] = '\0';

    return dest;
}

char* OICStrcatPartial(char* dest, size_t destSize, const char* source, size_t sourceLen)
{
    assert(dest);
    assert(source);

    if(destSize == 0 || sourceLen == 0)
    {
        return dest;
    }

    size_t destLen = strlen(dest);

    if (destLen >= destSize)
    {
        return dest;
    }

    size_t limit = min3(destSize - destLen - 1, sourceLen, strlen(source));
    memcpy(dest + destLen, source, limit);
    dest[destLen + limit] = '\0';
    return dest;
}
