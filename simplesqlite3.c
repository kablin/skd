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
#include <net/if.h>

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
int GetIp();


int main(int argc, char *argv[])
{
	system("clear");
	pthread_t COMRthread,COM2Rthread, Beepthread;
	struct ser_conf Ser1;
	void *tretr,*tretr2,*tretr3;
	Ser1.SerCFlags = 0;
	Ser1.SerCFlags = B9600;
	Ser1.SerCFlags |= CS8;
	Ser1.SerIFlags = 0;
	OpenDb("");
	self_ip =(char*)malloc(50*sizeof(char));

FILE *f;
f=fopen("params.txt","r");
fscanf(f,"%d",&COM_1);
fscanf(f,"%d",&COM_2);
fscanf(f,"%d",&in_output);
fscanf(f,"%d",&out_output);
fscanf(f,"%d",&enter_com);
fscanf(f,"%d",&use_id_in_log);
HOST =(char*)malloc(50*sizeof(char));
fscanf(f,"%s",HOST);

PAGE =(char*)malloc(50*sizeof(char));
fscanf(f,"%s",PAGE);

fprintf(stderr,"w====");
GetIp();


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
	pthread_create(&Beepthread,NULL,(pthread_startroutine_t)BEEP_thread,(void *) COM_2);


	//SendHTMLMsg("test");
	unsigned char * cart="[FFFFFFFF]";

	unsigned int n=0;

	while (n!=1)
	{
	        	scanf("%d",&n);
	        	if (n==2) {PrintResult(cart,1); n=0;}
	        	if (n==3) {PrintResult(cart,0); n=0;}
	}



	MaincikleStop=1;

	pthread_join(COMRthread, &tretr);
	pthread_join(COM2Rthread, &tretr2);
	pthread_join(Beepthread, &tretr3);
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



int GetIp()
{

	 struct ifreq ifr;
	    memset(&ifr, 0, sizeof(ifr));
	    strcpy(ifr.ifr_name, "eth0");

	    int s = socket(AF_INET, SOCK_DGRAM, 0);
	    ioctl(s, SIOCGIFADDR, &ifr);
	    close(s);

	    struct sockaddr_in *sa = (struct sockaddr_in*)&ifr.ifr_addr;

	    strcpy(self_ip,inet_ntoa(sa->sin_addr));

	    printf("addr = %s\n", self_ip);
	    return 0;


}

