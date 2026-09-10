#ifndef RESPONSES_H
#define RESPONSES_H

#include <stdio.h>
#include <stdlib.h>

typedef enum
{
    CONTENT_TYPE_TEXT,
    CONTENT_TYPE_CSS,
    CONTENT_TYPE_JS,

    CONTENT_TYPE_IMAGE,
    CONTENT_TYPE_IMAGE_PNG,
    CONTENT_TYPE_IMAGE_GIF,
    CONTENT_TYPE_IMAGE_ICO,

    CONTENT_TYPE_JSON,
    CONTENT_TYPE_PDF,

    CONTENT_TYPE_AMOUNT,
} contentType;

static char *content_type_strings[] =
    {
        [CONTENT_TYPE_TEXT] = "text/html",
        [CONTENT_TYPE_CSS] = "text/css",
        [CONTENT_TYPE_JS] = "text/javascript",
        [CONTENT_TYPE_IMAGE] = "image/jpeg",
        [CONTENT_TYPE_IMAGE_ICO] = "image/x-icon",
        [CONTENT_TYPE_IMAGE_PNG] = "image/png",
        [CONTENT_TYPE_IMAGE_GIF] = "image/gif",
        [CONTENT_TYPE_JSON] = "application/json",
        [CONTENT_TYPE_PDF] = "application/pdf",
};

static char *arr_content_type_from_string[] =
    {
        [CONTENT_TYPE_TEXT] = ".html",
        [CONTENT_TYPE_CSS] = ".css",
        [CONTENT_TYPE_JS] = ".js",
        [CONTENT_TYPE_IMAGE] = ".jpg",
        [CONTENT_TYPE_IMAGE_ICO] = ".ico",
        [CONTENT_TYPE_IMAGE_PNG] = ".png",
        [CONTENT_TYPE_IMAGE_GIF] = ".gif",
        [CONTENT_TYPE_JSON] = ".json",
        [CONTENT_TYPE_PDF] = ".pdf",
};

typedef struct response
{
    char version[16];
    int status_code;
    char status_message[128];
    unsigned char *body;
    size_t body_length;
    contentType content_type;
} response;

#define FILE_SERVER_ERROR "html/500.html"

response not_found(response *resp);

response internal_server_error(response *resp);

#endif