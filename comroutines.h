/*
 * comroutines.h
 *
 *  Created on: Aug 23, 2013
 *      Author: kablin
 */

#ifndef COMROUTINES_H_
#define COMROUTINES_H_

#include <pthread.h>

/*
#define COM1            0
#define COM2            1
#define COM3            2
#define COM4            3
*/

#define BUFFERSIZE 100
#define RANGE 0xFF

extern int debug;

extern void Log ( char * data);
//extern void SaveLog (char * data);
//extern void iSaveLog ( int data, char * sdata);
//extern void tSaveLog ( struct timeval begin, struct timeval end, char * sdata);

//extern pthread_t WriteCikle;
//extern pthread_t ReadCikle;

//static extern  pthread_mutex_t LogLocker;

extern unsigned char response[BUFFERSIZE];





extern int tmp1;
extern int tmp2;
//extern unsigned char packet[BUFFERSIZE];

/*struct ser_conf				// structure for config. options from config. file
{
unsigned long SerCFlags;
unsigned long SerIFlags;
};*/

extern struct serv_file Serv_conf;
extern int test();

int config_errors(int r);
int CardReaded(char *Card,unsigned char com);

extern void SendHTMLMsg(char *data);
//extern int com_Init ( unsigned char Port);

//int com_Write( void * bytes, int numbytes);
//int com_Read( unsigned char *bytes);
//void     com_Uninit  ( unsigned char Port );

//int COM_read_thread();
//void COM_write_thread();


extern unsigned int in_output;
extern unsigned int out_output;
extern unsigned int enter_com;
extern  char* HOST;
extern char* PAGE;

#endif


