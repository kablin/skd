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
	//char* req3="::bit,'";
	char* req3=",'";
	char* req4="');";
 	// printf("%d %s",direction,result);
 	// return;
    char* rez=malloc((strlen(req1)+strlen(req2)+strlen(req3)+strlen(req4)+strlen(card)+strlen(result)+strlen(self_ip)+strlen(direction))*sizeof(char));
	strcpy(rez,req1);
	strcat(rez,card);
	strcat(rez,req2);
	strcat(rez,result);
	strcat(rez,req3);
	if (use_id_in_log==0)
		strcat(rez,self_ip);
	else
	{
		char * s=malloc(10*sizeof(char));
		sprintf(s,"%d  ",use_id_in_log);
		strcat(rez,s);
		free(s);
	}
	//strcat(rez,direction);
	strcat(rez,req4);
	//puts(rez);
	//Log (rez);
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
	char * rez=( char *)malloc(660*sizeof(char));
	strcpy(rez,"SELECT CASE WHEN NOT EXISTS(SELECT C.ID FROM accreditation_data.tbl_cards C WHERE C.CARD_NUMBER = '");
	strcat(rez,Card);
	strcat(rez,"') THEN NULL ELSE (SELECT COALESCE(MIN(A.access_granted::integer)::bit, 0::BIT) as access FROM accreditation_data.tbl_cards C LEFT JOIN accreditation_data.tbl_link_access_group_to_skd_interval A ON A.ID_ACCESS_GROUP =C.ID_ACCESS_GROUP AND A.ID_INTERVAL IN (select B.ID FROM accreditation_data.tbl_skd_interval_list B WHERE ((b.date_from + b.time_from)::timestamp without time zone, (b.date_to + b.time_to)::timestamp without time zone) OVERLAPS (current_timestamp, INTERVAL '2 second') = true) WHERE C.CARD_NUMBER = '");
	strcat(rez,Card);
	strcat(rez,"') end  as access;");
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
		int Fiednum = PQfnumber(res,"access");
		if (Fiednum<0)
		{
			 Log("No access field in query");
			 WriteLogDb("No access field in query");
			 WriteLogDb(rez);
		}
		else
		{
			if(atoi(PQgetvalue(res,0,Fiednum))>0){
				Access="1";
			}
			else
			{
				Access="0";
			}
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
