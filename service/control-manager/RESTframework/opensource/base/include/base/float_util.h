// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_FLOAT_UTIL_H_
#define BASE_FLOAT_UTIL_H_
#pragma once

#include "build/build_config.h"

#include <float.h>
#include <math.h>

#if defined(OS_SOLARIS)
#include <ieeefp.h>
#endif

namespace base {

inline bool IsFinite(const double& number) {
#if defined(OS_POSIX)
// uclibc does not support finite function, we can use isfinite instead
// reference: http://repository.timesys.com/buildsources/f/firefox/firefox-14.0.1/firefox-14.0.1-uclibc.patch
#if defined(__UCLIBC__)
  return isfinite(number) != 0;
#else
  return finite(number) != 0;
#endif
#elif defined(OS_WIN)
  return _finite(number) != 0;
#endif
}

}  // namespace base

#endif  // BASE_FLOAT_UTIL_H_
