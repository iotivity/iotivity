#include "command_parser.h"

typedef struct command_arg
{
    char    type;       /* Argument Type (ip, port, cipher, key etc.) (1 Byte) (0~255) */
    int     length;     /* Value length (2 Bytes) (0~65535) */
    unsigned char    value;      /* actual data (Length bytes) */
} carg;


typedef struct comm_command
{
    unsigned char    msgCode;    /* Command/ResponseCode (1 Byte) (0~255) */
    int     msgId;      /* Identifier of the message (to match Command-Response) (2 bytes) (0~65535) */
    unsigned char    nArg;       /* Number of arguments (1 Byte) (0~255) */
    carg    cArg;
} ccommand;

typedef enum argument_list
{
    IP4 = 1,
	IP6,
    PORT,
    CIPHER,
    CLIENT_IDENTITY,
	SERVER_IDENTITY,
	PASSWORD,
	KEY,
    TOTAL_COUNT
}argList;

#define DEFAULT_STRING_SIZE 50

extern int g_port;
extern int g_cipher;
extern unsigned char  g_server_ip[DEFAULT_STRING_SIZE];
extern unsigned char g_psk[DEFAULT_STRING_SIZE];
extern int g_psk_len;
extern unsigned char g_client_identity[DEFAULT_STRING_SIZE];
extern int g_client_identity_len;
extern unsigned char g_server_identity[DEFAULT_STRING_SIZE];
extern int g_server_identity_len;
extern unsigned char  g_password[DEFAULT_STRING_SIZE];

void getArgumentValue(carg cArg, unsigned char *str)
{
    int ret;
    unsigned char buffer[1024];
    memset(buffer,0,1024);
    memcpy(buffer,str,cArg.length);

    printf("buffer %s\n", buffer);
    int i;

    switch(cArg.type)
    {
        case IP4:
            sprintf(g_server_ip, "%d.%d.%d.%d\n", buffer[0], buffer[1], buffer[2], buffer[3]);
            printf("server ip: %s\n", g_server_ip);
            break;
        case IP6:
        /*
			printf("String contents: %s", str);
			printf("Argument length: %d\n", cArg.length);
			for(i=0; i<16; i++)
			{
				printf("%02x", buffer[i] & 0xff);
			}
			printf("\n");
*/
        	sprintf(g_server_ip, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n",
        			buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9],
					buffer[10], buffer[11], buffer[12], buffer[13], buffer[14], buffer[15]);
        	printf("server ipv6: %s\n", g_server_ip);

            break;
        case PORT:
            g_port = ((buffer[0]<<8)+(buffer[1]));
            printf("server port: %d\n", g_port);
            break;
        case CIPHER:
        	g_cipher = ((buffer[0]<<8)+(buffer[1]));
        	printf("selected cipher: %0x\n", g_port);
            break;
        case KEY:
        	strcpy(g_psk, buffer);
        	g_psk_len = strlen(g_psk);
            break;
        case CLIENT_IDENTITY:
        	strcpy(g_client_identity, buffer);
        	g_client_identity_len = strlen(g_client_identity);
        	printf("Client Identity: %s\n", g_client_identity);
            break;
        case SERVER_IDENTITY:
			strcpy(g_server_identity, buffer);
			g_server_identity_len = strlen(g_server_identity);
			printf("Server Identity: %s\n", g_server_identity);
			break;
        case PASSWORD:
			strcpy(g_password, buffer);
			printf("Password: %s\n", g_password);
			break;
        default:
            break;

    }
}

int parse_command(unsigned char *str)
{
    int i=0;

    ccommand scommand;

    int offset=0;
    scommand.msgCode = str[offset++];
    scommand.msgId = ((str[offset]<<8)+(str[++offset]));
    offset++;
    scommand.nArg =  str[offset++];

    printf("Message Command: %d\n", scommand.msgCode);
    printf("Message Id: %d\n", scommand.msgId);
    printf("Number of arg: %d\n", scommand.nArg);

    for(i=0;i<scommand.nArg;i++)
    {
        scommand.cArg.type = str[offset++];
        scommand.cArg.length = ((str[offset]<<8)+(str[++offset]));
        offset++;

        printf("arg type %d  and length %d\n", scommand.cArg.type, scommand.cArg.length);
        getArgumentValue(scommand.cArg, &str[offset]);

        offset += scommand.cArg.length;
    }

    return scommand.msgCode;
}
