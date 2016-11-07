/*
 * simplesqlite3.h
 *
 *  Created on: Nov 24, 2015
 *      Author: kablin
 */

#ifndef SIMPLESQLITE3_H_
#define SIMPLESQLITE3_H_



#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <termio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <signal.h>
#include <syslog.h>
#include <errno.h>
#include <time.h>
#include <memory.h>


/*#define COM1            0
#define COM2            1
#define COM3            2
#define COM4            3*/
#define BUFFERSIZE 100
#define RANGE 0xFF


unsigned int COM_1;
unsigned int COM_2;

extern int MaincikleStop;
extern unsigned char response[BUFFERSIZE];
extern unsigned char response2[BUFFERSIZE];
struct ser_conf				// structure for config. options from config. file
{
unsigned long SerCFlags;
unsigned long SerIFlags;
};
extern struct serv_file Serv_conf;
int config_errors(int r);
int com_Init ( unsigned char Port, struct ser_conf *ConfigStruct);
int com_Read(unsigned char Port, unsigned char *bytes);
int com2_Read(unsigned char Port, unsigned char *bytes);
void  com_Uninit  ( unsigned char Port );
int COM_read_thread(unsigned char Port);
int COM2_read_thread(unsigned char Port);
int BEEP_thread(unsigned char Port);
typedef void	*(*pthread_startroutine_t) __P((void *));
void PrintResult(char *buf,unsigned char com);









#endif /* SIMPLESQLITE3_H_ */
