/*
 * MainBaseroutines.c
 *
 *  Created on: Aug 31, 2013
 *      Author: kablin
 */
#define _REENTRANT
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <termio.h>
#include <string.h>
#include <sqlite3.h>
#include <pthread.h>
#include <netinet/in.h>
#include <signal.h>
#include <syslog.h>
#include <stddef.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <comroutines.h>
#include <LocalDB.h>

//static const char *const request ="\r\nrequest=PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiIHN0YW5kYWxvbmU9Im5vIj8%2BPHJl%0D%0AcXVlc3Q%2BIDxmdW5jdGlvbiB2YWx1ZT0iZ2V0X2xpc3RfY2hlY2tfc3VtIi8%2BIDxhdXRoPiA8aW5w%0D%0AdXRfcGFyYW1ldGVyIHZhbHVlPSIxMTU3OTgxIi8%2BIDxkZXZpY2VfaWQgdmFsdWU9IjIwMDEiLz4g%0D%0APC9hdXRoPiA8bGlzdD48aWRfbGlzdCB2YWx1ZT0iMTgxNzI1NCIvPiA8Y3VycmVudF9jcmMgdmFs%0D%0AdWU9Ii0xOTM1MjIxMjM3Ii8%2BIDwvbGlzdD48L3JlcXVlc3Q%2B\r\n";

char base64_table[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ""abcdefghijklmnopqrstuvwxyz""0123456789+/"};
char * c_host = "192.168.1.35";
//static const char *const c_port = "8080";
/*static const char *const c_request =
	"POST /modules/skd_app/app/skd_new.php HTTP/1.1\r\n"
	"Authorization: Basic c2tkX3VzZXI6UXdlMzIxQXNk\r\n"
	"Host: 192.168.31.55\r\n"
	"Content-type: application/x-www-form-urlencoded ;charset=UTF-8\r\n"
	"Content-Length: 396\r\n"
	"\r\nrequest=PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiIHN0YW5kYWxvbmU9Im5vIj8%2BPHJl%0D%0AcXVlc3Q%2BIDxmdW5jdGlvbiB2YWx1ZT0iZ2V0X2xpc3RfY2hlY2tfc3VtIi8%2BIDxhdXRoPiA8aW5w%0D%0AdXRfcGFyYW1ldGVyIHZhbHVlPSIxMTU3OTgxIi8%2BIDxkZXZpY2VfaWQgdmFsdWU9IjIwMDEiLz4g%0D%0APC9hdXRoPiA8bGlzdD48aWRfbGlzdCB2YWx1ZT0iMTgxNzI1NCIvPiA8Y3VycmVudF9jcmMgdmFs%0D%0AdWU9Ii0xOTM1MjIxMjM3Ii8%2BIDwvbGlzdD48L3JlcXVlc3Q%2B\r\n"
	"\r\n";*/



//static const char *const tmpbase = "tmp.sqlt";
char * crc32="857941633";
//char * crc32="0";
char * idList="1817254";
char Download_success=0;





//char * Base64(const unsigned char *data, int input_length,int *output_length)
char * Base64(char *dst, const char *src, unsigned int len)
{

	  int x, y = 0, i, m=0;
	  int n = 3;
	  char triple[3], quad[4];
	  static char encode[1024]={0};

	    for(x = 0; x < len; x = x + 3)
	    {
	        if((len - x) / 3 == 0) n = (len - x) % 3;

	        for(i=0; i < 3; i++) triple[i] = '0';
	        for(i=0; i < n; i++) triple[i] = src[x + i];
	        quad[0] = base64_table[(triple[0] & 0xFC) >> 2]; // FC = 11111100
	        quad[1] = base64_table[((triple[0] & 0x03) << 4) | ((triple[1] & 0xF0) >> 4)]; // 03 = 11
	        quad[2] = base64_table[((triple[1] & 0x0F) << 2) | ((triple[2] & 0xC0) >> 6)]; // 0F = 1111, C0=11110
	        quad[3] = base64_table[triple[2] & 0x3F]; // 3F = 111111
	        if(n < 3) quad[3] = '=';
	        if(n < 2) quad[2] = '=';
	        for(i=0; i < 4; i++)
	        {

	        	if (quad[i]=='+')
	          	{
	          		dst[y + i] = '%';
	          		dst[y + i+1] = '2';
	          		dst[y + i+2] = 'B';
	          		y=y+2;
	          	}
	          	else if (quad[i]=='/')
	          	{
	          	 	 dst[y + i] = '%';
	          	  	 dst[y + i+1] = '2';
	          	  	 dst[y + i+2] = 'F';
	          	  	 y=y+2;
	          	}
	          	else   	dst[y + i] = quad[i];
	        }
	        y = y + 4;
	        m = m + 4;
	    }
	  dst[y] = '\0';
	  sprintf(encode,dst);
	  return encode;
}


/*

char * BuildBaseRequest()
{
   char * req =
		"POST /modules/skd_app/app/skd_new.php HTTP/1.1\r\n"
		"Authorization: Basic c2tkX3VzZXI6UXdlMzIxQXNk\r\n"
		"Host: ";

    char * tmp="\r\n"
   		"Content-type: application/x-www-form-urlencoded ;charset=UTF-8\r\n"
		"Connection: close\r\n"
   		"Content-Length: ";

   char * rez=malloc((strlen(req)+strlen(c_host)+1)*sizeof(char));
   strcpy(rez,req);
   strcat(rez,c_host);
   rez=realloc(rez,(strlen(rez)+strlen(tmp)+1)*sizeof(char));
   strcat(rez,tmp);

   char * tmp1="<?xml version='1.0' encoding='UTF-8' ?><request> <function value = 'get_list_check_sum'/> <auth> <input_parameter value='1157981'/> <device_id value='2001'/> </auth> <list><id_list value='";
   char * xmlreq=malloc((strlen(tmp1)+1)*sizeof(char));
   strcpy(xmlreq,tmp1);
   xmlreq=realloc(xmlreq,(strlen(xmlreq)+strlen(idList)+1)*sizeof(char));
   strcat(xmlreq,idList);
   char * tmp2 ="'/> <current_crc value='";
   xmlreq=realloc(xmlreq,(strlen(xmlreq)+strlen(tmp2)+1)*sizeof(char));
   strcat(xmlreq,tmp2);
   xmlreq=realloc(xmlreq,(strlen(xmlreq)+strlen(crc32)+1)*sizeof(char));
   strcat(xmlreq,crc32);
   char * tmp3="'/> </list></request>";
   xmlreq=realloc(xmlreq,(strlen(xmlreq)+strlen(tmp3)+1)*sizeof(char));
   strcat(xmlreq,tmp3);

  // puts(xmlreq);
   char * dest=malloc(1024*sizeof(char));;
   char * xml64=Base64(dest,xmlreq,strlen(xmlreq));


   char * tmp4="request=";
   xmlreq=malloc((strlen(dest)+strlen(tmp4)+1)*sizeof(char));
   strcpy(xmlreq,tmp4);
   strcat(xmlreq,dest);


   char * s=malloc(100*sizeof(char));
   int size =strlen(xmlreq);
   sprintf(s,"%d", size);
   rez=realloc(rez,(strlen(rez)+strlen(s)+1)*sizeof(char));
   strcat(rez,s);
   char * tmp5="\r\n\r\n";
   rez=realloc(rez,(strlen(rez)+strlen(tmp5)+1)*sizeof(char));
   strcat(rez,tmp5);
   rez=realloc(rez,(strlen(rez)+strlen(xmlreq)+1)*sizeof(char));
   strcat(rez,xmlreq);
   rez=realloc(rez,(strlen(rez)+strlen(tmp5)+1)*sizeof(char));
   strcat(rez,tmp5);


   free(s);
   free(xmlreq);
   //free(xml64);
   free(dest);
  return rez;
}
*/

/*
static int callback1(void *NotUsed, int argc, char **argv, char **azColName){
	NotUsed=0;
	int i;
	for (i=0;i<argc;i++)
	{
		if (strcmp("id1",azColName[i])==0)
		{
			if (atoi(argv[i])!=0)Download_success=1;
		}
	}
	return 0;
 }

static int callback2(void *NotUsed, int argc, char **argv, char **azColName){
	NotUsed=0;
	int i;
	//puts(crc32);
	for ( i=0;i<argc;i++)
	{
		if (strcmp("crc32_value",azColName[i])==0)
		{
			crc32=( char *)malloc((strlen(argv[i])+1)*sizeof(char));
			strcpy(crc32,argv[i]);
		    Download_success=1;
		}
	}
	return 0;
 }*/

/*

int CheckNewDb()
{
	char *zErrMsg = 0;
	char * idSel= "Select Count(id) as id1 FROM list_result";
	char* crcSel= "Select crc32_value  from crc32_result";


	int rc;
	sqlite3 *db;
	sqlite3_close(db);
	rc = sqlite3_open(tmpbase, &db);
	if( rc )
	{
	    Log( "Can't open temp database:");
	    Log(  sqlite3_errmsg(db));
	    SaveLog("Can't open temp database:");
	    sqlite3_close(db);
	    return 1;
    }

	Download_success=0;
	rc = sqlite3_exec(db,idSel, callback1, 0, &zErrMsg);
	if( rc!=SQLITE_OK )
	{
        Log("SQL error:");
        SaveLog("SQL error: CheckNewDb_1");
        sqlite3_close(db);
        return 1;
	}
	if ( Download_success==1)
    {
    	Download_success=0;
    	rc = sqlite3_exec(db,crcSel, callback2, 0, &zErrMsg);
        if ( Download_success==1)
        {
    	    sqlite3_close(db);
        	rename(tmpbase,crc32);
    	    ChangeDB(crc32);
        } else sqlite3_close(db);
    } else sqlite3_close(db);
    return 0;

}*/

/*

int GetDataBase()
{
    remove(tmpbase);
	struct addrinfo hint, *addr;
	int             sock, file;
	ssize_t         readed;
	char            buf[256];
	int             i, rn = 0;
	int             content = 0;
	int             dbsize=0;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	getaddrinfo(c_host, c_port, &hint, &addr);

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(sock, addr->ai_addr, sizeof(struct sockaddr_in))!=0)
	{
		 SaveLog("GetDataBase No Connection");
		 return 1;
	}


	freeaddrinfo(addr);


	char * request=BuildBaseRequest();

	//puts (request);
	write(sock, request,strlen(request));

	//write(sock, c_request,strlen(c_request));
	file = open(tmpbase, O_WRONLY | O_CREAT | O_TRUNC, 00644);
	while (1)
	{
		readed = read(sock, buf, sizeof(buf));
		if (-1 == readed)
			return 1;
		if (!readed)
			break;

		if (content)
		{
			//puts("Conte");
			dbsize=dbsize+write(file, buf, readed);
		}
		else
		{
			for (i = 0; i < readed; ++i)
			{
				if ('\n' == buf[i])
					++rn;
				else if ('\r' == buf[i])
				{
				}
				else
					rn = 0;

				if (2 == rn)
				{
					content = 1;

					dbsize=dbsize+write(file, buf + i + 1,
						readed - i - 1);

					break;
				}
			}
		}
	}
	close(file);
	close(sock);

    if (dbsize>0)
    {
    iSaveLog(dbsize,": size. new db file loaded");
	CheckNewDb();
    }
	return 0;
}



void BaseUpdateThread()
{

	sigset_t sigs;
		sigemptyset(&sigs);
		sigaddset(&sigs, SIGINT);
		sigaddset(&sigs, SIGTERM);
		//sigaddset(&sigs, SIGALRM);
		sigaddset(&sigs, SIGUSR1);
		sigaddset(&sigs, SIGUSR2);

		if(pthread_sigmask(SIG_BLOCK, &sigs, NULL) != 0) {
			SaveLog("BaseUpdateTread Error Mask");
			}

	while(MaincikleStop==0)
	{
      int tm=sleep(45);
      while ((tm!=0)&&(MaincikleStop==0)) tm=sleep(tm);
      GetDataBase();
	}
return;
}

*/










