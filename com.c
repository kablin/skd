/*
 * com.c
 *
 *  Created on: Nov 24, 2015
 *      Author: kablin
 */

#define _REENTRANT
#define _MULTI_THREADED
#include "simplesqlite3.h"
#define COM_MAX  4
typedef struct
{
  int                   fd;
  struct termios        oldtio;
  struct termios        newtio;
  unsigned char         buf;
} T_COMPORT;

unsigned char response[BUFFERSIZE];
unsigned char response2[BUFFERSIZE];
static char           * coms[COM_MAX]  = {"/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3"};
static T_COMPORT      * ports[COM_MAX] = {NULL,NULL,NULL,NULL};


static pthread_mutex_t PortLocker  =PTHREAD_MUTEX_INITIALIZER ;


int com_Init ( unsigned char Port, struct ser_conf *ConfigStruct)
{
  if( !ports[Port] )
  if( (ports[Port] = malloc( sizeof(T_COMPORT) )) != NULL )
  {

    ports[Port]->fd = open( coms[Port], O_RDWR | O_NOCTTY | O_EXCL | O_NONBLOCK );

    if( ports[Port]->fd > 0 )
    {

      tcgetattr( ports[Port]->fd, &ports[Port]->oldtio );
      bzero( &ports[Port]->newtio, sizeof(struct termios) );
      ports[Port]->newtio.c_cflag = ConfigStruct->SerCFlags | CLOCAL | CREAD;
      ports[Port]->newtio.c_iflag = ConfigStruct->SerIFlags | IGNPAR;
      ports[Port]->newtio.c_oflag = 0;
      ports[Port]->newtio.c_lflag = 0;
      ports[Port]->newtio.c_cc[VTIME] = 5;
      ports[Port]->newtio.c_cc[VMIN]  = 1;
      tcflush( ports[Port]->fd, TCIFLUSH );
      tcsetattr( ports[Port]->fd, TCSANOW, &ports[Port]->newtio );
      fprintf(stderr,"COM Init OK %d %s\n", ports[Port]->fd, coms[Port]);

      return 1;
    }
  }
		fprintf(stderr,"COM Init Err %s\n", coms[Port]);
  return 0;
}
int com_Read  (unsigned char Port,unsigned char *bytes)
{
	int count=0;
	while(MaincikleStop!=1)
	{
		struct timeval timeout;
    	int rc, sfd;
		timeout.tv_sec = 0;
    	timeout.tv_usec = 1000;
    	fd_set fds;
    	sfd = ports[Port]->fd;
    	FD_ZERO(&fds);
    	FD_SET(sfd, &fds);
		rc = select(sfd+1, &fds, NULL, NULL, &timeout);
		if(rc==0)
		{
			if(count!=0)
			{
				pthread_mutex_lock(&PortLocker);
				PrintResult((char*)bytes,Port);
				pthread_mutex_unlock(&PortLocker);
				memset(bytes,0,strlen((char*)bytes));
				count=0;
			}
		}
		else
		{
		 rc = read(sfd, bytes+count, BUFFERSIZE-count);
   	     if(rc==-1)
		     {
				printf("Reqd -1! %s\n", strerror(errno));
		     }
		     else if(rc==0)
		     {
		    	 count=0;
     		 }
		     else
		     {
    			count += rc;
			 }
		}
	}
	return 0;
 }
int com2_Read  (unsigned char Port,unsigned char *bytes)
{
	int count=0;
	while(MaincikleStop!=1)
	{
		struct timeval timeout;
    	int rc, sfd;
		timeout.tv_sec = 0;
    	timeout.tv_usec = 1000;
    	fd_set fds;
    	sfd = ports[Port]->fd;
    	FD_ZERO(&fds);
    	FD_SET(sfd, &fds);
		rc = select(sfd+1, &fds, NULL, NULL, &timeout);
		if(rc==0)
		{
			if(count!=0)
			{
				 pthread_mutex_lock(&PortLocker);
				 PrintResult((char*)bytes,Port);
				 pthread_mutex_unlock(&PortLocker);
				 memset(bytes,0,strlen((char*)bytes));
				 count=0;
			}
		}
		else
		{
			 rc = read(sfd, bytes+count, BUFFERSIZE-count);

    	     if(rc==-1)
		     {
				printf("Reqd -1! %s\n", strerror(errno));
		     }
		     else if(rc==0)
		     {
		    	 count=0;
     		 }
		     else
		     {
    			count += rc;
			 }
		}
	}
	return 0;
 }
void com_Uninit  ( unsigned char Port )
{
  if( ports[Port] )
  {
    tcsetattr( ports[Port]->fd, TCSANOW, &ports[Port]->oldtio );
    close( ports[Port]->fd );
    free( ports[Port] );
    ports[Port] = NULL;
  }
}

int COM_read_thread(unsigned char Port)
{
		sigset_t sigs;
		int count;

		sigemptyset(&sigs);
		sigaddset(&sigs, SIGINT);
		sigaddset(&sigs, SIGTERM);

		if(pthread_sigmask(SIG_BLOCK, &sigs, NULL) != 0) {
				fprintf(stderr, "pthread_sigmask() failed\n");
		}

		count = com_Read(Port, response);//, BUFFERSIZE, 1000, 800);
		return count;
}
int COM2_read_thread(unsigned char Port)
{
		sigset_t sigs;
		int count;

		sigemptyset(&sigs);
		sigaddset(&sigs, SIGINT);
		sigaddset(&sigs, SIGTERM);

		if(pthread_sigmask(SIG_BLOCK, &sigs, NULL) != 0) {
				fprintf(stderr, "pthread_sigmask() failed\n");
		}

		count = com2_Read(Port, response2);//, BUFFERSIZE, 1000, 800);
		return count;
}

