#ifndef LOCALDB_H_
#define LOCALDB_H_
#define _REENTRANT
#include <sys/time.h>
#include <comroutines.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "libpq-fe.h"
void CloseDb();
char* FindCard( char * Card);
int OpenDb(char * Path);
int ChangeDB(const char * Path);
void WriteEntranceLogDb(char * card,char* direction,char* result);
void WriteLogDb(char * data);
#endif /* LOCALDB_H_ */
