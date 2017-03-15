
#ifndef __COMMON__H
#define __COMMON__H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>


//color codes
#define red  "\x1B[31m"
#define green "\x1B[32m"
#define yellow  "\x1B[33m"
#define blue  "\x1B[34m"
#define magenta  "\x1B[35m"
#define cyan  "\x1B[36m"
#define white  "\x1B[37m"
#define RESET "\033[0m"

#define MAX_USERNAMELEN 21

typedef enum
{
  CONNECT,
  DISCONNECT,
  GET_USERS,
  SET_USERNAME,
  SET_PASSWORD,
  PUBLIC_MESSAGE,
  PRIVATE_MESSAGE,
  TOO_FULL,
  USERNAME_ERROR,
  SUCCESS,
  ERROR

}message_type;

typedef struct
{
  message_type type;
  char username[21];
  char data[256];

}message_t;

typedef struct connection
{
  int socket;
  struct sockaddr_in address;
  char username[20];
} connection_t;

void trim_newline(char *text);
void clear_stdin_buffer();

#endif