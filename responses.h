#ifndef RESPONSES_H
#define RESPONSES_H

#include <stdio.h>
#include <stdlib.h>

typedef struct response
{
    char version[16];
    int status_code;
    char status_message[64];
    char *body;
    size_t body_length;
} response;

#define FILE_SERVER_ERROR "html/500.html"

response not_found(response *resp);

response internal_server_error(response *resp);

#endif