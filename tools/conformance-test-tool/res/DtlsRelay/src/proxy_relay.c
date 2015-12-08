#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <event2/event.h>
#include <sysexits.h>
#include <unistd.h>
#include <assert.h>

#include "dtls_client.h"

#define MSG_SIZE 1500

struct event_base* base;
struct event* server_event;

struct sockaddr_in control_proxy;
struct sockaddr_in data_proxy;

int g_port;
int g_cipher;
unsigned char  g_server_ip[DEFAULT_STRING_SIZE];
unsigned char g_psk[DEFAULT_STRING_SIZE];
int g_psk_len;
unsigned char g_client_identity[DEFAULT_STRING_SIZE];
int g_client_identity_len;
unsigned char g_server_identity[DEFAULT_STRING_SIZE];
int g_server_identity_len;
unsigned char  g_password[DEFAULT_STRING_SIZE];

int data_fd;
socklen_t g_client_len;

struct sockaddr_in g_client_addr;

void sendData(uint8* data,size_t len)
{
    int error = sendto(data_fd, data, len, 0, (struct sockaddr*) &g_client_addr, g_client_len);
}

static void data_handler(evutil_socket_t fd, short which, void* arg)
{
	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	char msg[MSG_SIZE];
	int msgLen;

	msgLen = recvfrom(fd, msg, MSG_SIZE, 0, (struct sockaddr*) &client, &len);
	g_client_addr = client;
	g_client_len=len;

	if(msgLen)
	{
		send_message(msg, msgLen,fd, sendData);
	}
}

static void control_handler(evutil_socket_t fd, short which, void* arg)
{
    int ret, on = 1;
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    unsigned char msg[MSG_SIZE];
    int msgLen;
    struct event* cliEvent;
    struct event* srvEvent;

    msgLen = recvfrom(fd, msg, MSG_SIZE, 0, (struct sockaddr*) &client, &len);

    printf("Receive %d bytes from client at control port\n", msgLen);

    if(msgLen)
    {
    	printf("Incoming message: ");

    	int i = 0;
    	for(i = 0; i < msgLen; i++)
    	{
    		printf("%0x ", msg[i]);
    	}
    	printf("\n");

    	printf("Parsing the message\n");

    	int msgCode = parse_command(msg);

    	if(msgCode == 1)
    	{
    		int server_fd = open_connection(g_server_ip, g_port, g_cipher);

			server_event = event_new(base, server_fd, EV_READ | EV_PERSIST, periodic_read, NULL);
			event_add(server_event, NULL);
    	}
    	else if(msgCode == 2)
    	{
    		close_connection();
    		event_del(server_event);
    	}
    }
}

int main(int argc, char** argv)
{
    int control_fd;

    int ret, ch, on = 1;
    struct event* control_event;
    struct event* data_event;

    short connection_port = 12345;
    short data_port = 12346;

    printf("Staring Dtls Relay ..................\n");

    base = event_base_new();
        if (base == NULL)
	{
		perror("event_base_new failed");
		exit(EXIT_FAILURE);
	}

    printf("Creating Command Socket ..................\n");

    control_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (control_fd < 0)
    {
        perror("Command Socket Creation Failed");
        exit(EXIT_FAILURE);
    }

    memset(&control_proxy, 0, sizeof(control_proxy));
    control_proxy.sin_family = AF_INET;
    control_proxy.sin_addr.s_addr = htonl(INADDR_ANY);
    control_proxy.sin_port = htons(connection_port);

    setsockopt(control_fd, SOL_SOCKET, SO_REUSEADDR, &on, (socklen_t) sizeof(on));

    ret = bind(control_fd, (struct sockaddr*) &control_proxy, sizeof(control_proxy));
    if (ret < 0)
    {
        perror("control address bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Creating Data Socket ..................\n");

    data_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (data_fd < 0)
    {
        perror("Data Socket Creation Failed");
        exit(EXIT_FAILURE);
    }

    memset(&data_proxy, 0, sizeof(data_proxy));
    data_proxy.sin_family = AF_INET;
    data_proxy.sin_addr.s_addr = htonl(INADDR_ANY);
    data_proxy.sin_port = htons(data_port);

    ret = bind(data_fd, (struct sockaddr*) &data_proxy, sizeof(data_proxy));
	if (ret < 0)
	{
		perror("data address bind failed");
		exit(EXIT_FAILURE);
	}

    control_event = event_new(base, control_fd, EV_READ | EV_PERSIST, control_handler, NULL);
    if (control_event == NULL)
    {
        perror("control event failed for mainEvent");
        exit(EXIT_FAILURE);
    }

    event_add(control_event, NULL);

    data_event = event_new(base, data_fd, EV_READ | EV_PERSIST, data_handler, NULL);
	if (data_event == NULL)
	{
		perror("data event failed for mainEvent");
		exit(EXIT_FAILURE);
	}

	event_add(data_event, NULL);

    event_base_dispatch(base);

    return 0;
}
