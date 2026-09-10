#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include "responses.h"

#define MAX_HEADERS 64
#define FILE_NOT_FOUND "html/404.html"
#define EMPTY_BODY "<empty>"

typedef struct request
{
    char method[8];
    char path[128];
    char version[16];
    char headers[MAX_HEADERS][128];
    int headers_amount;
    char body[2048];
} request;

typedef enum
{
    METHOD_GET,
    METHOD_POST,
    METHOD_UNKNOWN,
} http_method;

char *content_type_to_str(contentType);

contentType content_type_from_str(char *path);

request parse_http_request(char *buffer, ssize_t msglen);

char *parse_query(response *resp, request *req);

void handle_post_request(response *resp, request *req);

char *read_file(FILE *file, size_t *len);

unsigned char *read_file_bytes(FILE *file, size_t *len);

void handle_get_request(response *resp, request *req);

response generate_http_response(request *req);

void printRequest(request *req);

#endif