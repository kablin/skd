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

unsigned int Sleepdelay;
unsigned int DIin;
unsigned int DIout;

unsigned int DOin;
unsigned int DOout;

unsigned int comin;
unsigned int comout;


int main(int argc, char *argv[])
{

	FILE *f;
	f=fopen("params.txt","r");
	fscanf(f,"%d",&Sleepdelay);
	fscanf(f,"%d",&DIin);
	fscanf(f,"%d",&DIout);
	fscanf(f,"%d",&DOin);
	fscanf(f,"%d",&DOout);

	fscanf(f,"%d",&comin);
	fscanf(f,"%d",&comout);
	fclose(f);




	system("clear");
	pthread_t COMRthread,COM2Rthread;
	struct ser_conf Ser1;
	void *tretr,*tretr2;
	Ser1.SerCFlags = 0;
	Ser1.SerCFlags = B9600;
	Ser1.SerCFlags |= CS8;
	Ser1.SerIFlags = 0;
	OpenDb("");

	set_do_buf(DOin,0);
	set_do_buf(DOout,0);

	if(com_Init(comin, &Ser1))
	{
		pthread_create(&COMRthread, NULL,(pthread_startroutine_t)COM_read_thread,(void *) comin);

	}
	else {
			fprintf(stderr,"Could not create COM%d read thread \n\n", comin+1);
		}
	if(com_Init(comout, &Ser1))
	{
		pthread_create(&COM2Rthread,NULL,(pthread_startroutine_t)COM2_read_thread,(void *) comout);
	}
	else {
		fprintf(stderr,"Could not create COM%d read thread \n\n", comout+1);
	}


	unsigned char * cart="[FFFFFFFF]";

	unsigned int n=0;

	while (n!=1)
	{
	        	scanf("%d",&n);
	        	if (n==2) {PrintResult(cart,1); n=0;}
	}



	MaincikleStop=1;

	pthread_join(COMRthread, &tretr);
	pthread_join(COM2Rthread, &tretr2);
	CloseDb();
	exit(0);
}

void PrintResult(char *buf,unsigned char com)
{
	char *to =  calloc(10,sizeof(char));
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

