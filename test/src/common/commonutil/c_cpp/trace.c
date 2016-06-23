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

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "trace.h"
#include <memory.h>
#include <stdlib.h>
#include <cstdlib>

#define FN_SIZE 100
#define FN_DEFAULT "cyglog.%d"

/* Private variables.  */
static int level = 0;
static FILE *logfile = NULL;
static int cyg_profile_enabled = 0;
static char cyg_profile_filename[FN_SIZE + 1];

#ifdef __cplusplus
extern "C"
{

#endif
#include <dlfcn.h>
#include <memory.h>
/* Static functions. */
static FILE *openlogfile(const char *filename) __attribute__ ((no_instrument_function));
static void closelogfile(void) __attribute__ ((no_instrument_function));

/* Note that these are linked internally by the compiler. 
 * Don't call them directly! */
void __cyg_profile_func_enter(void *this_fn, void *call_site) __attribute__ ((no_instrument_function));
void __cyg_profile_func_exit(void *this_fn, void *call_site) __attribute__ ((no_instrument_function));

#ifdef __cplusplus
};
#endif

void __cyg_profile_func_enter(void *this_fn, void *call_site)
{
    Dl_info di;
    if (cyg_profile_enabled)
        if (logfile || openlogfile(cyg_profile_filename))

            if (dladdr(this_fn, &di))
            {
                char const* d_fname = di.dli_fname;
                char const* d_sname = di.dli_sname;

                if (d_fname != NULL && d_sname != NULL)
                {

                    setenv("ShellVar", d_sname, 1);
                    system("c++filt -n $ShellVar>>tmp.txt");

                    FILE *ptr_file;
                    char buf[256];
                    ptr_file = fopen("tmp.txt", "r");
                    char *bufTemp;
                    while (fgets(buf, sizeof(buf), ptr_file) != 0)

                    {
                        bufTemp = NULL;
                        bufTemp = buf;
                    }

                    fprintf(logfile, "+ %d %p %p %s %s\n", level++, this_fn, call_site, d_fname,
                            bufTemp);
                    fclose(ptr_file);

                }

                else
                {
                    fprintf(logfile, "+ %d %p %p\n", level++, this_fn, call_site);
                }

            }

    //fprintf(logfile, "+ %d %p %p\n", level++,
    //  this_fn, call_site);
}

void __cyg_profile_func_exit(void *this_fn, void *call_site)
{
    if (cyg_profile_enabled)
        if (logfile || openlogfile(cyg_profile_filename))
            fprintf(logfile, "- %d %p %p\n", level--, this_fn, call_site);
}

void cygprofile_enable(void)
{
    if (!cyg_profile_filename[0])
        cygprofile_setfilename(FN_DEFAULT);
    if (!openlogfile(cyg_profile_filename))
        return;
    cyg_profile_enabled = 1;
}

void cygprofile_disable(void)
{
    cyg_profile_enabled = 0;
}

int cygprofile_isenabled(void)
{
    return cyg_profile_enabled;
}

int cygprofile_setfilename(const char *filename)
{
    char const* ptr;

    if (cygprofile_isenabled())
        return -1;

    if (strlen(filename) > FN_SIZE)
        return -2;

    ptr = strstr(filename, "%d");
    if (ptr)
    {
        size_t len;
        len = ptr - filename;
        snprintf(cyg_profile_filename, len + 1, "%s", filename);
        snprintf(&cyg_profile_filename[len], FN_SIZE - len, "%d", getpid());
        len = strlen(cyg_profile_filename);
        snprintf(&cyg_profile_filename[len], FN_SIZE - len, "%s", ptr + 2);
    }
    else
        snprintf(cyg_profile_filename, FN_SIZE, "%s", filename);

    if (logfile)
        closelogfile();

    return 0;
}

char *
cygprofile_getfilename(void)
{
    if (!cyg_profile_filename[0])
        cygprofile_setfilename(FN_DEFAULT);
    return cyg_profile_filename;
}

static FILE *
openlogfile(const char *filename)
{
    static int complained = 0;
    FILE *file;

    if (complained)
        return NULL;

    if (logfile)
        return logfile;

    file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "WARNING: Can't open logfile '%s': %s\n", filename, strerror(errno));
        complained = 1;
        return NULL;
    }

    setlinebuf(file);
    logfile = file;

    return file;
}

static void closelogfile(void)
{
    if (logfile)
        fclose(logfile);
}
