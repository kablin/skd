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












