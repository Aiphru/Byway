#ifndef SERV_H
#define SERV_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "parser.h"

#define PORT 8800

int send_file_response(int client_fd, response *resp);
int send_response(int client_fd, response *resp);
int httpServer(void);

#endif