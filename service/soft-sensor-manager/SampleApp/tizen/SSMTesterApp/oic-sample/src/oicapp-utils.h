/*
 * Copyright (c) 2010 Samsung Electronics, Inc.
 * All rights reserved.
 *
 * This software is a confidential and proprietary information
 * of Samsung Electronics, Inc. ("Confidential Information").  You
 * shall not disclose such Confidential Information and shall use
 * it only in accordance with the terms of the license agreement
 * you entered into with Samsung Electronics.
 */
#ifndef __OICAPP_UTILS_H__
#define __OICAPP_UTILS_H__

#include "oicapp-test.h"

#ifdef __cplusplus
extern "C"
{
#endif

void oicapp_fail_popup(oicapp_data *ad, char *title, char *text, int timeout);
void oicapp_util_put_msg(oicapp_data *ad, const char *msg);
char *oicapp_util_wifi();

/*
int oicapp_client_start(oicapp_data *ad);
void oicapp_client_stop();
int oicapp_client_put(int power, int level);

int oicapp_server_start(oicapp_data *ad);
void oicapp_server_stop();
void oicapp_server_put(int power, int level);
*/


#ifdef __cplusplus
}
#endif

#endif //__OICAPP_UTILS_H__


