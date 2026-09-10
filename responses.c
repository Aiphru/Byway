#include "responses.h"
#include "parser.h"

response not_found(response *resp)
{
}

response internal_server_error(response *resp)
{
    FILE *file = fopen(FILE_SERVER_ERROR, "rb");
    resp->body = read_file(file, &resp->body_length);
    fclose(file);
    resp->status_code = 500;
    strcpy(resp->status_message, "Internal Server Error");
    return *resp;
}
