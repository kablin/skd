/*
 *
 *
 * досттуп по времени - некст аксес
 * лог
 * нормальное завершение
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <LocalDB.h>
#include <stddef.h>
#include "crc16ccit.h"
#include "comroutines.h"


#include <memory.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <hwdapi.h>


#include "simplesqlite3.h"


int MaincikleStop=0;
int main(int argc, char *argv[])
{
	system("clear");
	pthread_t COMRthread,COM2Rthread;
	struct ser_conf Ser1;
	void *tretr,*tretr2;
	Ser1.SerCFlags = 0;
	Ser1.SerCFlags = B9600;
	Ser1.SerCFlags |= CS8;
	Ser1.SerIFlags = 0;
	OpenDb("");

FILE *f;
f=fopen("params.txt","r");
fscanf(f,"%d",&COM_1);
fscanf(f,"%d",&COM_2);
fscanf(f,"%d",&in_output);
fscanf(f,"%d",&out_output);
fscanf(f,"%d",&enter_com);
HOST =(char*)malloc(50*sizeof(char));
fscanf(f,"%s",HOST);

PAGE =(char*)malloc(50*sizeof(char));
fscanf(f,"%s",PAGE);



fprintf(stderr,"h====");









	set_do_buf(in_output,0);
	set_do_buf(out_output,0);

	if(com_Init(COM_1, &Ser1))
	{
		pthread_create(&COMRthread, NULL,(pthread_startroutine_t)COM_read_thread,(void *) COM_1);

	}
	else {
			fprintf(stderr,"Could not create COM%d read thread \n\n", COM_1+1);
		}
	if(com_Init(COM_2, &Ser1))
	{
		pthread_create(&COM2Rthread,NULL,(pthread_startroutine_t)COM2_read_thread,(void *) COM_2);
	}
	else {
		fprintf(stderr,"Could not create COM%d read thread \n\n", COM_2+1);
	}



	//SendHTMLMsg("test");
	unsigned char * cart="[FFFFFFFF]";

	unsigned int n=0;

	while (n!=1)
	{
	        	scanf("%d",&n);
	        	if (n==2) {PrintResult(cart,1); n=0;}
	}



	MaincikleStop=1;

	pthread_kill(COMRthread, &tretr);
	pthread_kill(COM2Rthread, &tretr2);
	CloseDb();
	exit(0);
}

void PrintResult(char *buf,unsigned char com)
{
	char *to =  calloc(9,sizeof(char));
	strncpy(to, buf+6, 2);
	strncpy(to+2, buf+4, 2);
	to[4]=':';
	strncpy(to+5, buf+2, 2);
	strncpy(to+7, buf, 2);
	printf("Port:%d Card:%s\n",com, to);
	WriteLogDb(to);
	CardReaded(to,com);
	free(to);
}

