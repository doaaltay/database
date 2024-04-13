#ifndef DATABASE_LIB_H
#define DATABASE_LIB_H

#include <unistd.h>

typedef struct{
  int socket_fd;

}Database;

Database* database_connect(int port);
int database_execute(Database* database, char *command);
void database_close(Database* database);



#endif