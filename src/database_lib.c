//this lib is used 
#include "database_lib.h"
#include "sockutils.c"

Database* database_connect(const char *host, int port)
{
  Database* db =malloc(sizeof(Database));

  //port to string
  char port_str[6]; 
  sprintf(port_str, "%d", port);

  if(db == NULL)
  {
    perror("malloc: error allocating memory for database");
    exit(1);

  }
  //db->socket_fd = make_client(port, 5); //replace this with actual making client

  //make client 


  struct addrinfo *info = make_addrinfo(host, port_str);
  if (info==NULL){
    perror("make_addrinfo: error creating address info");
    exit(1);
  }

  //connect to host
  db->socket_fd = host_connect(info);
  if (db->socket_fd < 0){
    perror("host_connect: error connecting to host");
    exit(1);
  }

  freeaddrinfo(info);

  return db;
}

int database_execute(Database* database, char *command)
{
  write(database->socket_fd, command, strlen(command));
  char response[1024];
  read(database->socket_fd, response, sizeof(response));
  printf("server response: %s\n", response);
  return 0;
}

void database_close(Database* database)
{
  close(database->socket_fd);
  free(database);
}

/**
 * 
 * int main()
{
  Database* db = database_connect(8080);
  char command[1024];
  while (1)
  {
    printf("Enter a command: ");

    if (strcmp(command, "exit\n") == 0 || strcmp(command, "q\n") == 0 || strcmp(command, "quit\n") == 0)
    {
      break;
    }
    fgets(command, sizeof(command), stdin);
    command[strcspn(command, "\n")] = 0;
    database_execute(db, command);
  }

  database_close(db);
  printf("Exiting...\n");
  return 0;
}
*/
