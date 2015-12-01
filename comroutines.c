
/*CloseTripod * comroutines.c
 *
 *  Created on: Aug 23, 2013
 *      Author: kablin
 */





#define _REENTRANT
#define _MULTI_THREADED
#include <stdio.h>
#include <stdlib.h>
//#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <termio.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <signal.h>
#include <syslog.h>
#include <stddef.h>
#include <crc16ccit.h>
#include <errno.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <hwdapi.h>
#include <LocalDB.h>
#include <comroutines.h>

//#include "RStest.h"


static pthread_mutex_t LogLocker  =PTHREAD_MUTEX_INITIALIZER ;
char WaitForTurn = 0;
char TripodIsOpenin = 0;
char TripodIsOpenout = 0;
pthread_t Closetrip;
int cikle=0;
void Log (char * data)
{
	pthread_mutex_lock(&LogLocker);
	puts(data);
	pthread_mutex_unlock(&LogLocker);
}
void NullHandler()
{
	
}
void TimerCloseInTread()
{
	struct sigaction sa;
	memset(&sa,0,sizeof(sa));
	sa.sa_handler=&NullHandler;
	sigaction(SIGUSR2,&sa,NULL);
	Log("Start timer");
	sigset_t sigs;
	sigemptyset(&sigs);
	sigaddset(&sigs, SIGUSR2);
	if(pthread_sigmask(SIG_UNBLOCK, &sigs, NULL) != 0) {
		WriteLogDb( "TimerCloseInTread pthread_sigmask() failed\n");
	}
	struct timeval begin;
	gettimeofday(&begin,NULL);
	int rezsl=usleep(5000000);
	if (rezsl!=0)
	{
		Log("Stop Timer killed");
		return;
	}
	WaitForTurn = 0;
	if (TripodIsOpenin==1) {
		set_do_buf(1,0); Log("Close in by timer");
	}
	if (TripodIsOpenout==1) {
		set_do_buf(2,0);  Log("Close out by timer");
	}
	TripodIsOpenin = 0;
	TripodIsOpenout = 0;
	char x[10];
	cikle++;
	sprintf(x,"%d",cikle);
	puts (x);
	return;
}

void CloseTripod(char In)
{
	WaitForTurn = 0;
	pthread_kill(Closetrip,SIGUSR2);
	if (In==1)
	{
		TripodIsOpenin = 0;
		set_do_buf(1,0);
	}
	else
	{
		TripodIsOpenout = 0;
		set_do_buf(2,0);
	}
}
void OpenTripod(char In)
{
 	pthread_kill(Closetrip,SIGUSR2);
    WaitForTurn = 1;
    if (In==1)
    {
	   TripodIsOpenin = 1;
	   set_do_buf(1,1);

      	pthread_create(&Closetrip,NULL,TimerCloseInTread,NULL);
       	pthread_detach(Closetrip);
   }
   else
   {
	   TripodIsOpenout = 1;
	   set_do_buf(2,1);
	   pthread_create(&Closetrip,NULL,TimerCloseInTread,NULL);
	   pthread_detach(Closetrip);

   };
   return;
}
int CardReaded(char *Card,unsigned char com)
{
	time_t x=0;
	time(&x);
    char Enter = 0;
    char* Direction="0";


 /*   char * s=malloc(100*sizeof(char));
    		sprintf(s,"%d  ",com);
    		puts(s);
    		free (s);*/




    if (com==2)
	{
		Enter = 1;
		Direction="1";
	}

    char* Access=FindCard(Card);
    // Доступ разрешен
    if((atoi(Access)==1) &&( ((TripodIsOpenin ==0)&&(TripodIsOpenout ==0))|| ((TripodIsOpenin ==1)&&(Enter==1))||(((TripodIsOpenout ==1)&&(Enter==0)))))
    {
    	Log("Access OK");
        pthread_kill(Closetrip,SIGUSR2);
    	OpenTripod(Enter);
    }
    else if (atoi(Access)==1)
    {
    	Log("Turniket is busy");
    	WriteLogDb("Turniket is busy");
	}
    // Даоступ запрещен
    else if (atoi(Access)==0)
    {
    	Log("NO Access");
    	CloseTripod(Enter);
	}
	// Неизвестная карта
	else
	{
		 Log("Unknown CARD");
		 WriteLogDb("Unknown CARD");
	  	 CloseTripod(Enter);
	}
	WriteEntranceLogDb((char*)Card,Direction,Access);
    return 0;
}
