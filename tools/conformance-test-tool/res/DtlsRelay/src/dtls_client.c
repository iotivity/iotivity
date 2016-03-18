#include "tinydtls.h" 

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <event2/event.h>
#include <netinet/in.h>

#include "debug.h"
#include "dtls.h"

#include "dtls_client.h"

static char g_rcv[2000];

extern unsigned char g_psk[DEFAULT_STRING_SIZE];
extern int g_psk_len;
extern unsigned char g_client_identity[DEFAULT_STRING_SIZE];
extern int g_client_identity_len;
extern unsigned char g_server_identity[DEFAULT_STRING_SIZE];
extern int g_server_identity_len;
extern unsigned char  g_password[DEFAULT_STRING_SIZE];

int g_server_fd;

static session_t g_dst;
static dtls_context_t *g_dtls_context = NULL;

fun_ptr     g_fptr;

#define PSK_ID_MAXLEN 256
#define PSK_MAXLEN 256

#define XOR_BUF(in, out, bufSize)\
do{\
    size_t i=0;\
    for(i=0; i< (bufSize); i++)\
    {\
        (out)[i] = (in)[i] ^ (out)[i];\
    }\
}while(0)\

static int isLittle()
{
    static int a = 1;
    static int flag = -1;
    if (flag == -1)
    {
        if (  ((uint8_t *)&a)[0]  == 0x1) // little
            flag = 1;
        else
            flag = 0;
    }
    return flag;
}

static void GetBigEndianBuf(uint8_t *buf, int num)
{
    uint8_t *nBuf = (uint8_t *)&num;
    if ( isLittle() == 1 )
    {
        size_t i = 0;
        for (i = 0; i < sizeof(int); i++)
        {
            buf[i] = nBuf[ sizeof(int) - i - 1];
        }
    }
    else
    {
        memcpy(buf, nBuf, sizeof(int));
    }
}

int DeriveCryptoKeyFromPassword(const unsigned char *passwd, size_t pLen,
                                const uint8_t *salt, const size_t saltLen,
                                const size_t iterations,
                                const size_t keyLen, uint8_t *derivedKey)
{
    int res = 0;
    uint8_t buf[DTLS_HMAC_DIGEST_SIZE];
    uint8_t uBuf[DTLS_HMAC_DIGEST_SIZE];

    size_t nBlocks = 0;
    size_t nOctetInLastBlock = 0;

    nBlocks = (size_t)ceil ((double)keyLen / (double)DTLS_HMAC_DIGEST_SIZE);
    nOctetInLastBlock = keyLen - (nBlocks - 1) * DTLS_HMAC_DIGEST_SIZE;

    dtls_hmac_context_t *ctx = NULL;
    ctx = dtls_hmac_new( (const unsigned char *)passwd, pLen);
    if (NULL == ctx)
    {
        printf("DTLS HMAC Context is NULL\n");
        goto bail;
    }

    size_t i = 1;
    size_t idx = 0; //index for buffer
    size_t counter = 0;
    while (i != nBlocks + 1)
    {
        counter = 0 ;
        dtls_hmac_init(ctx, (const unsigned char *)passwd, pLen);
        while (counter != iterations)
        {
            if (counter == 0)
            {
                uint8_t intBuf[4] = {0x00, 0x00, 0x00, 0x00};
                dtls_hmac_update(ctx, salt, saltLen);
                GetBigEndianBuf(intBuf, i);
                dtls_hmac_update(ctx, intBuf, 4);

                int len = dtls_hmac_finalize(ctx, buf);
                if (DTLS_HMAC_DIGEST_SIZE != len)
                {
                	printf("DTLS HMAC is failed\n");
                    res = -1;
                }
                memcpy(uBuf, buf, DTLS_HMAC_DIGEST_SIZE);
            }
            else
            {
                dtls_hmac_init(ctx, (const unsigned char *)passwd, pLen);
                dtls_hmac_update(ctx, buf, DTLS_HMAC_DIGEST_SIZE);
                int len = dtls_hmac_finalize(ctx, buf);
                if (DTLS_HMAC_DIGEST_SIZE != len)
                {
                	printf("DTLS HMAC is failed\n");
                    res = -1;
                }
                XOR_BUF(buf, uBuf, DTLS_HMAC_DIGEST_SIZE);
            }
            counter++;
        }


        if (i == nBlocks)
        {
            memcpy(derivedKey + idx, uBuf, nOctetInLastBlock);
        }
        else
        {
            memcpy(derivedKey + idx, uBuf, DTLS_HMAC_DIGEST_SIZE);
            idx += DTLS_HMAC_DIGEST_SIZE;
        }
        i++;
    }

bail:
    dtls_hmac_free(ctx);
    return res;
}

static int generate_owner_psk(const uint8_t* label, const size_t labelLen,
        const uint8_t* rsrcServerDeviceID, const size_t rsrcServerDeviceIDLen,
        const uint8_t* provServerDeviceID, const size_t provServerDeviceIDLen,
        uint8_t* ownerPSK, const size_t ownerPSKSize)
{
	int ret = dtls_prf_with_current_keyblock(
			g_dtls_context, (session_t*)(&g_dst), label, labelLen,
			rsrcServerDeviceID, rsrcServerDeviceIDLen, provServerDeviceID, provServerDeviceIDLen, ownerPSK, ownerPSKSize);

	return ret;
}

static int get_psk_info(struct dtls_context_t *ctx,
	    const session_t *session,
	    dtls_credentials_type_t type,
	    const unsigned char *id, size_t id_len,
	    unsigned char *result, size_t result_length)
{
  switch (type)
  {
  	  case DTLS_PSK_IDENTITY:

//          if (result_length < sizeof(g_client_identity))
//          {
//              printf("ERROR : Wrong value for result for storing IDENTITY\n");
//              printf("length: result=%d g_client_identity=%d\n", result_length, sizeof(g_client_identity));
//              return -1;
//          }

          memcpy(result, g_client_identity, g_client_identity_len);
          return g_client_identity_len;

  	  case DTLS_PSK_KEY:

  		  if ((id_len == sizeof(g_server_identity)) && memcmp(g_server_identity, id, id_len) == 0)
		  {
				if (result_length < sizeof(g_psk))
				{
					printf("ERROR : Wrong value for result for storing RS_CLIENT_PSK\n");
					return -1;
				}

			  memcpy(result, g_psk, g_psk_len);
			  return g_psk_len;
		  }
  		  else		//remove this in future
  		  {
  			  memcpy(result, g_psk, g_psk_len);
  			  return g_psk_len;
  		  }

  		  return -1;

  	  default:
  		  printf("unsupported request type: %d\n", type);
  }

  return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
}

static void try_send(struct dtls_context_t *ctx, session_t *dst, char* msg)
{
	int len = strlen(msg);

	printf("Calling dtls_write\n");

	int res = dtls_write(ctx, dst, (uint8 *)msg, len);

	if (res >= 0)
	{
		memmove(msg, msg + res, len - res);
		len -= res;
	}

	printf("Writing Ended\n");
}

static int read_from_peer(struct dtls_context_t *ctx, session_t *session, uint8 *data, size_t len)
{
	size_t i;

	(void)ctx;
	(void)session;

	printf("Read from peer. Sending data to client\n");

	(*g_fptr)(data,len);

	printf("Data send to client is complete\n");

	return 0;
}

static int send_to_peer(struct dtls_context_t *ctx, session_t *session, uint8 *data, size_t len)
{
	int fd = *(int *)dtls_get_app_data(ctx);

	return sendto(fd, data, len, MSG_DONTWAIT, &session->addr.sa, session->size);
}

static int dtls_handle_read(struct dtls_context_t *ctx)
{
	int fd;
	int len;

	session_t session;

	fd = *(int *)dtls_get_app_data(ctx);

	if (!fd)
	{
		return -1;
	}

	memset(&session, 0, sizeof(session_t));
	session.size = sizeof(session.addr);
	len = recvfrom(fd, g_rcv, 2000, 0, &session.addr.sa, &session.size);

	if (len < 0)
	{
		//perror("recvfrom");
		return -1;
	}
	else
	{
//#define DEBUG_MODE
#ifdef DEBUG_MODE
		dtls_dsrv_log_addr(DTLS_LOG_DEBUG, "peer", &session);
		dtls_debug_dump("bytes from peer", g_rcv, len);
#endif
	}

	return dtls_handle_message(ctx, &session, g_rcv, len);
}

static int resolve_address(const char *server, int port, struct sockaddr *dst)
{
	struct addrinfo *res, *ainfo;
	struct addrinfo hints;
	static char addrstr[256];
	char portstr[10];
	int error;

	memset(addrstr, 0, sizeof(addrstr));
	if (server && strlen(server) > 0)
	{
		memcpy(addrstr, server, strlen(server));
	}
	else
	{
		memcpy(addrstr, "localhost", 9);
	}

	sprintf(portstr, "%d", port);

	printf("Server: %s\n", server);
	printf("addrstr: %s\n", addrstr);
	printf("portstr: %s\n", portstr);

	memset ((char *)&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
//	hints.ai_protocol = 0;
	hints.ai_protocol=IPPROTO_UDP;
//	hints.ai_flags = AI_ADDRCONFIG;


	error = getaddrinfo(addrstr, portstr, &hints, &res);

	if (error != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
		return error;
	}

	for (ainfo = res; ainfo != NULL; ainfo = ainfo->ai_next)
	{
		switch (ainfo->ai_family)
		{
			case AF_INET6:
			case AF_INET:

				memcpy(dst, ainfo->ai_addr, ainfo->ai_addrlen);
				return ainfo->ai_addrlen;
			default:
				;
		}
	}

	freeaddrinfo(res);

	return -1;
}

static dtls_handler_t cb =
{
	.write = send_to_peer,
	.read  = read_from_peer,
	.event = NULL,
	.get_psk_info = get_psk_info
};

void periodic_read(evutil_socket_t fd, short which, void* arg)
{
	fd_set rfds, wfds;
	struct timeval timeout;

	FD_ZERO(&rfds);
	FD_ZERO(&wfds);

	FD_SET(fileno(stdin), &rfds);
	FD_SET(g_server_fd, &rfds);

	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	int result = select(g_server_fd+1, &rfds, &wfds, 0, &timeout);

	if(result > 0)
	{
		dtls_handle_read(g_dtls_context);
	}
}

int open_connection(char* ip, int port, int cipher)
{
	printf("Dtls open_connection IN\n");

	int result;
	int opt, res;

	fd_set rfds, wfds;

	struct timeval timeout;

	dtls_cipher_t selected_cipher = cipher;

	dtls_cipher_enable_t ecdh_anon_enalbe = DTLS_CIPHER_DISABLE;

	if(selected_cipher == TLS_ECDH_anon_WITH_AES_128_CBC_SHA_256)
	{
		ecdh_anon_enalbe = DTLS_CIPHER_ENABLE;
	}

	if(selected_cipher == TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA_256)
	{
		g_psk_len = 16;
		DeriveCryptoKeyFromPassword(g_password, strlen(g_password), g_client_identity, 16, 1000, g_psk_len, g_psk);
		int i;
		printf("pre-shared key: ");
		for(i = 0; i < g_psk_len; i++)
		{
			printf("%02x ", g_psk[i]);
		}
		printf("\n");
	}
	else if(selected_cipher == TLS_PSK_WITH_AES_128_CCM_8)
	{
		if(g_psk_len == -1)
		{
			printf("Private data not found for cipher TLS_PSK_WITH_AES_128_CCM_8\n");
			return -1;
		}
	}

	printf("Calling dtls_init\n");

	dtls_init();

#ifdef DEBUG_MODE
	log_t log_level = DTLS_LOG_DEBUG;
	dtls_set_log_level(log_level);
#endif

	memset(&g_dst, 0, sizeof(session_t));

	printf("ip address: %s\n", ip);

	res = resolve_address(ip, port, &g_dst.addr.sa);

	if (res < 0)
	{
		printf("failed to resolve address\n");
		//return -1;
	}

	g_dst.size = res;

	if(strlen(ip) > 16)
	{
		g_dst.addr.sin6.sin6_port = htons(port);
	}
	else
	{
		g_dst.addr.sin.sin_port = htons(port);
	}

	g_server_fd = socket(g_dst.addr.sa.sa_family, SOCK_DGRAM, 0);

	if (g_server_fd < 0)
	{
		printf("Unable to Create Socket\n");
		return -1;
	}

	printf("Setting dtls_context\n");

	g_dtls_context = dtls_new_context(&g_server_fd);

	if (!g_dtls_context)
	{
		printf("cannot create context\n");
		return -1;
	}

	printf("Selecting dtls_cipher\n");

	dtls_select_cipher(g_dtls_context, selected_cipher);

    dtls_enables_anon_ecdh(g_dtls_context, ecdh_anon_enalbe);

    printf("Setting dtls_handler\n");

	dtls_set_handler(g_dtls_context, &cb);

	printf("Calling dtls_connect\n");

	dtls_connect(g_dtls_context, &g_dst);

	int i;

	for(i = 0; i < 6; )
	{
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);

		FD_SET(fileno(stdin), &rfds);
		FD_SET(g_server_fd, &rfds);

		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		result = select(g_server_fd+1, &rfds, &wfds, 0, &timeout);

		if(result > 0)
		{
			dtls_handle_read(g_dtls_context);
			i++;
		}
	}

	printf("Dtls option Connection Successful\n");

	if(selected_cipher == TLS_ECDH_anon_WITH_AES_128_CBC_SHA_256)
	{
		char label[] = "oic.sec.doxm.jw";
		g_psk_len = 16;
		int ret = generate_owner_psk(label, strlen(label),
				g_client_identity, g_client_identity_len, g_server_identity, g_server_identity_len, g_psk, g_psk_len);

		if(!ret)
		{
			printf("generate_owner_psk returns %d\n", ret);
			return -1;
		}
	}

	printf("Dtls option Connection Out\n");

	return g_server_fd;
}

int send_message(char* str, int str_len,int data_fd, fun_ptr fptr)
{
	printf("Dtls Send Message In\n");

	str[str_len] = 0;

	g_fptr = fptr;

	try_send(g_dtls_context, &g_dst, str);

	printf("Dtls Send Message Out\n");

	return 1;
}

int close_connection()
{
	if(dtls_close(g_dtls_context, &g_dst) < 0)
	{
		return 0;
	}

	dtls_free_context(g_dtls_context);

	return 1;
}
