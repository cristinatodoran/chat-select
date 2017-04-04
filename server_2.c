
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "common.h"

#define MAX_CLIENTS 1
 

FILE *f;

void init(connection_t *server_connection, int port)
{
  if((server_connection->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("Failure! Creating socket didn't succeed!");
    exit(1);
  }

  server_connection->address.sin_family = AF_INET;
  server_connection->address.sin_addr.s_addr = INADDR_ANY;
  server_connection->address.sin_port = htons(port);

  if(bind(server_connection->socket, (struct sockaddr *)&server_connection->address, sizeof(server_connection->address)) < 0)
  {
    perror("Binding failed");
    exit(1);
  }
  // set SO_REUSEADDR on a socket to true (1):
  // Allows other sockets to bind() to this port, unless there is an active listening socket bound to the port already
  const int optVal = 1;
  const socklen_t optLen = sizeof(optVal);

  if(setsockopt(server_connection->socket, SOL_SOCKET, SO_REUSEADDR, (void*) &optVal, optLen) < 0)
  {
    perror("Setting socket failed");
    exit(1);
  }


  if(listen(server_connection->socket, 3) < 0) {
    perror("Listening failed");
    exit(1);
  }

   printf("Server Started \nWaiting for incoming clients to connect :) ...\n");
}

void stop_server(connection_t connection[])
{

  for (int i = 0; i < MAX_CLIENTS; i++)
  {
    close(connection[i].socket);
  }
  exit(0);
}
 
 
 


void handle_client_message(connection_t server)
{
  int read_size;
  message_t msg;

  if((read_size = recv(server.socket, &msg, sizeof(message_t), 0)) == 0)
  {
    printf("User offline: %s.\n", server.username);
    close(server.socket);
    server.socket = 0;
    

  } 
  else
  {
    printf("Message received: %s.\n", msg.data);


    
    if (f == NULL)
    {
      printf("Error opening file!\n");
      exit(1);
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    /* print some text */ 
    fprintf(f, "%d-%d-%d %d:%d:%d %s: %s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, msg.username , msg.data);

    }

  
  
}

int construct_fd_set(fd_set *set, connection_t *server_info,
                      connection_t clients[])
{
  //Initializes the file descriptor set fdset to have zero bits for all file descriptors.
  FD_ZERO(set);
  //Sets the bit for the file descriptor fd in the file descriptor set fdset.
  FD_SET(STDIN_FILENO, set);
  FD_SET(server_info->socket, set);

  int max_fd = server_info->socket;
  int i;
  for(i = 0; i < MAX_CLIENTS; i++)
  {
    if(clients[i].socket > 0)
    {
      FD_SET(clients[i].socket, set);
      if(clients[i].socket > max_fd)
      {
        max_fd = clients[i].socket;
      }
    }
  }
  return max_fd;
}



void handle_new_connection(connection_t *server_info, connection_t clients[])
{
  int new_socket;
  int address_len;
  new_socket = accept(server_info->socket, (struct sockaddr*)&server_info->address, (socklen_t*)&address_len);

  if (new_socket < 0)
  {
    perror("Accept Failed");
    exit(1);
  }

  int i;
  for(i = 0; i < MAX_CLIENTS; i++)
  {
    if(clients[i].socket == 0) {
      clients[i].socket = new_socket;
      break;

    } 
  }
}

 

int main(int argc, char *argv[])
{
  puts("Starting Logger Server.");

  connection_t logger_info;

  fd_set file_descriptors;

  connection_t server_info;
  connection_t clients[MAX_CLIENTS];
   
  for(int i = 0; i < MAX_CLIENTS; i++)
  {
    clients[i].socket = 0;
  }

  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    exit(1);
  }

  int portNo = atoi(argv[1]);
  init(&server_info,portNo);

  f = fopen("file.txt", "wa");

  while(1)
  {
    int max_fd = construct_fd_set(&file_descriptors, &server_info, clients);
  /*
  * When select returns, it has updated the sets to show which file descriptors have become ready for read/write/exception. 
  * All other flags have been cleared.
  */
    if(select(max_fd+1, &file_descriptors, NULL, NULL, NULL) < 0)
    {
      perror("Select Failed");
      stop_server(clients);
    }

    if(FD_ISSET(server_info.socket, &file_descriptors))
    {
      handle_new_connection(&server_info, clients);
    }

    for(int i = 0; i < MAX_CLIENTS; i++)
    {
      if(clients[i].socket > 0 && FD_ISSET(clients[i].socket, &file_descriptors))
      {
        handle_client_message(clients[i]);
      }
    }
  }
  fclose(f);
  return 0;
}