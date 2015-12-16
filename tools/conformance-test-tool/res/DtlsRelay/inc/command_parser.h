/*
 * com_parser.h
 *
 *  Created on: Nov 5, 2015
 *      Author: mushfiqul
 */

#ifndef COM_PARSER_H_
#define COM_PARSER_H_

int parse_command(unsigned char *str, int* msgCode, int* msgId);
int get_private_data(int msg_code, int msg_id, unsigned char data[]);

#endif /* COM_PARSER_H_ */
