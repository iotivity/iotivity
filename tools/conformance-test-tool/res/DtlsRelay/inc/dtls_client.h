#include "global.h"

#define DEFAULT_STRING_SIZE 20
//#define DEBUG_MODE

typedef void (*fun_ptr)(uint8 *data, size_t len);
int open_connection(char* ip, int port, int cipher);
int send_message(char* hex_stream, int hex_stream_len,int data_fd, fun_ptr fptr);
int close_connection();
void periodic_read(evutil_socket_t fd, short which, void* arg);
