
/*CloseTripod * comroutines.c
 *
 *  Created on: Aug 23, 2013
 *      Author: kablin
 */


//jjjjjjjj


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
#include <netdb.h>


#define PORT_Socket 80
#define USERAGENT "HTMLGET 1.0"



static pthread_mutex_t LogLocker  =PTHREAD_MUTEX_INITIALIZER ;
char WaitForTurn = 0;
char TripodIsOpenin = 0;
char TripodIsOpenout = 0;
pthread_t Closetrip;
int cikle=0;

 unsigned int in_output;
 unsigned int out_output;
 unsigned int enter_com;
 char* HOST;
 char* PAGE;
 char* self_ip;
 //unsigned int use_id_in_log;







 void SendHTMLMsg(char* data);



int create_tcp_socket()
{
  int sock;
  if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
    perror("Can't create TCP socket");
    return 0;
  }
  return sock;
}


char* get_ip(char *host)
{
  struct hostent *hent;
  int iplen = 15; //XXX.XXX.XXX.XXX
  char *ip = (char *)malloc(iplen+1);
  memset(ip, 0, iplen+1);
  if((hent = gethostbyname(host)) == NULL)
  {
    herror("Can't get IP");
    return"";
  }
  if(inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, iplen) == NULL)
  {
    perror("Can't resolve host");
    return"";
  }
  return ip;
}

char *build_get_query(char *host, char *page)
{
  char *query;
  char *getpage = page;
  char *tpl = "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
  if(getpage[0] == '/'){
    getpage = getpage + 1;
    fprintf(stderr,"Removing leading \"/\", converting %s to %s\n", page, getpage);
  }
  // -5 is to consider the %s %s %s in tpl and the ending \0
  query = (char *)malloc(strlen(host)+strlen(getpage)+strlen(USERAGENT)+strlen(tpl)-5);
  sprintf(query, tpl, getpage, host, USERAGENT);
  return query;
}


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
	int rezsl=usleep(2000000);
	if (rezsl!=0)
	{
		Log("Stop Timer killed");
		return;
	}
	WaitForTurn = 0;
	if (TripodIsOpenin==1) {
		set_do_buf(in_output,0); Log("Close in by timer");
	}
	if (TripodIsOpenout==1) {
		set_do_buf(out_output,0);  Log("Close out by timer");
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
		set_do_buf(in_output,0);
	}
	else
	{
		TripodIsOpenout = 0;
		set_do_buf(out_output,0);
	}
}
void OpenTripod(char In)
{
 	pthread_kill(Closetrip,SIGUSR2);
    WaitForTurn = 1;
    if (In==1)
    {
	   TripodIsOpenin = 1;
	   set_do_buf(in_output,1);
      	pthread_create(&Closetrip,NULL,TimerCloseInTread,NULL);
       	pthread_detach(Closetrip);
   }
   else
   {

	   TripodIsOpenout = 1;
	   set_do_buf(out_output,1);
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
    char* Direction="out";
    if (com==enter_com)
	{
		Enter = 1;
		Direction="in";
	}

    char* Access=FindCard(Card);
    // Доступ разрешен
    if((atoi(Access)==2) &&( ((TripodIsOpenin ==0)&&(TripodIsOpenout ==0))|| ((TripodIsOpenin ==1)&&(Enter==1))||(((TripodIsOpenout ==1)&&(Enter==0)))))
    {
    	Log("Access OK");
        pthread_kill(Closetrip,SIGUSR2);
    	OpenTripod(Enter);
    }
    else if (atoi(Access)==2)
    {
    	Log("Turniket is busy");
    	WriteLogDb("Turniket is busy");
	}
    // Даоступ запрещен
    else if (atoi(Access)==3)
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



void SendHTMLMsg(char* data)
{
	 struct sockaddr_in* remote;
	  int sock;
	  int tmpres;
	  char* ip;
	  char* get;
	//  char buf[BUFSIZ+1];
	//  char* host;

      char* param="?error=";
Log(data);

	//  page = "test.php?error=555555";

	  char * page=malloc((strlen(PAGE)+strlen(param)+strlen(data))*sizeof(char));

	  strcpy(page,PAGE);
	  strcat(page,param);
	  strcat(page,data);

	  sock = create_tcp_socket();
	  ip = get_ip(HOST);
	 // fprintf(stderr, "IP is %s\n", ip);
	  remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
	  remote->sin_family = AF_INET;
	  tmpres = inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));
	  if( tmpres < 0)
	  {
	    perror("Can't set remote->sin_addr.s_addr");
	    return;
	  }else if(tmpres == 0)
	  {
	    fprintf(stderr, "%s is not a valid IP address\n", ip);
	    return;
	  }
	  remote->sin_port = htons(PORT_Socket);

	  if(connect(sock, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0){
	    perror("Could not connect");
	    return;
	  }
	  get = build_get_query(HOST, page);
	//  fprintf(stderr, "Query is:\n<<START>>\n%s<<END>>\n", get);

	  //Send the query to the server
	  int sent = 0;
	  while(sent < strlen(get))
	  {
	    tmpres = send(sock, get+sent, strlen(get)-sent, 0);
	    if(tmpres == -1){
	      perror("Can't send query");
	      return;
	    }
	    sent += tmpres;
	  }


	  //now it is time to receive the page
	/*  memset(buf, 0, sizeof(buf));
	  int htmlstart = 0;
	  char * htmlcontent;
	  while((tmpres = recv(sock, buf, BUFSIZ, 0)) > 0){
	    if(htmlstart == 0)
	    {
	      htmlcontent = strstr(buf, "\r\n\r\n");
	      if(htmlcontent != NULL){
	        htmlstart = 1;
	        htmlcontent += 4;
	      }
	    }else{
	      htmlcontent = buf;
	    }
	    if(htmlstart){
	      fprintf(stdout, htmlcontent);
	    }

	    memset(buf, 0, tmpres);
	  }
	  if(tmpres < 0)
	  {
	    perror("Error receiving data");
	  }*/



	  free(page);
	  free(get);
	  free(remote);
	  free(ip);
	  close(sock);
	  return ;
}








