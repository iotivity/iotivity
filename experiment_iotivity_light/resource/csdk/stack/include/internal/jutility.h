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

// utility macros

#ifndef UTILITY_MACROS
#define UTILITY_MACROS

#define LDECLTYPE(x) __typeof(x)

#define LL_APPEND(head, add)                      \
{                                                 \
  LDECLTYPE(head) _tmp;                           \
  (add)->next = NULL;                             \
  if (head) {                                     \
    for (_tmp = head; _tmp->next; _tmp = _tmp->next) { } \
    _tmp->next = (add);                           \
  } else {                                        \
    (head) = (add);                               \
  }                                               \
}

#define LL_DELETE(head, del)                      \
{                                                 \
  LDECLTYPE(head) _tmp;                           \
  if ((head) == (del)) {                          \
    (head) = (head)->next;                        \
  } else {                                        \
    for (_tmp = head; _tmp->next && (_tmp->next != (del)); _tmp = _tmp->next) \
    { }                                           \
    if (_tmp->next) {                             \
      _tmp->next = ((del)->next);                 \
    }                                             \
  }                                               \
}

#define LL_FOREACH(head, el)                      \
  for ((el) = (head); (el); (el) = (el)->next)

#define LL_FOREACH_SAFE(head, el)                 \
  LDECLTYPE(el) _tmp;                             \
  for ((el) = (head); (el) && (_tmp = (el)->next, 1); (el) = _tmp)

#endif // UTILITY_MACROS
