/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#ifndef CYG_PROFILE_H
#define CYG_PROFILE_H

/* Public functions.  */

#ifdef __cplusplus
extern "C" {
#endif

/* Enable/disable CygProfiler.  */
void cygprofile_enable (void)
	__attribute__ ((no_instrument_function));
void cygprofile_disable (void)
	__attribute__ ((no_instrument_function));

/* Tell whether CygProfiler is enabled/disabled.  */
int cygprofile_isenabled (void)
	__attribute__ ((no_instrument_function));

/* Set filename of a logfile.  */
int cygprofile_setfilename (const char *filename)
	__attribute__ ((no_instrument_function));

/* Query for a filename of a logfile.  */
char *cygprofile_getfilename (void)
	__attribute__ ((no_instrument_function));

#ifdef __cplusplus
};
#endif

#endif
