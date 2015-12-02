#include "LocalDB.h"
PGconn    *conn;
PGresult  *res;
void CloseDb()
{
	PQfinish(conn);
	puts("close conn");
}
void WriteLogDb(char * data)
{
	PGresult        *res;
	time_t x=0;
	time(&x);
	struct tm * timeinfo=localtime(&x);
	char * req1="insert into log values ('";
	char * req2="');";
	char * rez=malloc((strlen(req1)+strlen(req2)+strlen(data)+strlen(asctime(timeinfo))+4)*sizeof(char));
	strcpy(rez,req1);
	strcat(rez,data);
	strcat(rez,"','");
	strcat(rez,asctime(timeinfo));
	strcat(rez,req2);
	res = PQexec(conn, rez);
	free(rez);
	PQclear(res);
	return;
}
void WriteEntranceLogDb(char * card,char* direction,char* result)
{
	PGresult *res;
	char* req1="Insert into accreditation_data.tbl_skd_log (card_number,access_granted,id_reader) VALUES ('";
	char* req2="',";
	char* req3=",";
	char* req4=");";
 	// printf("%d %s",direction,result);
 	// return;
    char* rez=malloc((strlen(req1)+strlen(req2)+strlen(req3)+strlen(req4)+strlen(card)+strlen(result)+strlen(direction))*sizeof(char));
    //printf("%s\n",rez);
	strcpy(rez,req1);
	strcat(rez,card);
	strcat(rez,req2);
	strcat(rez,result);
	strcat(rez,req3);
	strcat(rez,direction);
	strcat(rez,req4);
	//puts(rez);
	res = PQexec(conn, rez);
	free(rez);
	PQclear(res);
	return;
}
char * FindCard(char * Card)
{
	if(strlen(Card)!=9)
	{
		Log("Wrong input");
		Log("Card");
		WriteLogDb("Wrong query result");
		return "-1";
	}
	struct timeval begin;
	gettimeofday(&begin,NULL);
	PGresult  *res;
	char * rez=( char *)malloc(70*sizeof(char));
	strcpy(rez,"SELECT * FROM accreditation_data.get_access('");
	strcat(rez,Card);
	strcat(rez,"');");
	//puts(rez);
	char* Access="-1";
	res = PQexec(conn, rez);

	if (PQresultStatus(res) != PGRES_TUPLES_OK ) {
        Log("Query error");
	}
	int rec_count = PQntuples(res);
	if ( rec_count==0)
	{
	  Log("No card");
	}
	else if (rec_count>1)
	{
		Log("Wrong query result");
		WriteLogDb("Wrong query result");
		WriteLogDb(rez);
	}
	else
	{
		int Fiednum = PQfnumber(res,"get_access");
		if (Fiednum<0)
		{
			 Log("No access field in query");
			 WriteLogDb("No access field in query");
			 WriteLogDb(rez);
		}
		else
		{

			Access=PQgetvalue(res,0,Fiednum);
		}
		
	}
	free(rez);
	PQclear(res);
	return Access;
}
int OpenDb(char * Path)
{
	conn = PQconnectdb("dbname=mnmx host=localhost user=test_user password=qwerty");
	if (PQstatus(conn) == CONNECTION_BAD) {
		puts("Не удается подключиться к базе данных");
		return 0;
	}
	else{
		return 1;
	}
}
