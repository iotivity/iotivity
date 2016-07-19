/* dtls -- a very basic DTLS implementation
 *
 * Copyright (C) 2011--2014 Olaf Bergmann <bergmann@tzi.org>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _DTLS_SESSION_H_
#define _DTLS_SESSION_H_

#include <string.h>

#include "dtls_config.h"
#include "tinydtls.h"
#include "global.h"

#ifdef WITH_CONTIKI
#include "ip/uip.h"
typedef struct {
  unsigned char size;
  uip_ipaddr_t addr;
  unsigned short port;
  int ifindex;
} session_t;

#else /* WITH_CONTIKI */

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#ifdef HAVE_WS2TCPIP_H
#include <ws2tcpip.h>
#endif
#include <stdint.h>

typedef struct {
  socklen_t size;		/**< size of addr */
#ifdef _MSC_VER
  __declspec(align(8))
#endif
  union {
    struct sockaddr     sa;
    struct sockaddr_storage st;
    struct sockaddr_in  sin;
    struct sockaddr_in6 sin6;
  } addr;
  uint8_t ifindex;
} session_t;
#endif /* WITH_CONTIKI */

/** 
 * Resets the given session_t object @p sess to its default
 * values.  In particular, the member rlen must be initialized to the
 * available size for storing addresses.
 * 
 * @param sess The session_t object to initialize.
 */
void dtls_session_init(session_t *sess);

/**
 * Compares the given session objects. This function returns @c 0
 * when @p a and @p b differ, @c 1 otherwise.
 */
int dtls_session_equals(const session_t *a, const session_t *b);

#endif /* _DTLS_SESSION_H_ */
