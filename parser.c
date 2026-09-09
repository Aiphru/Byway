#include "parser.h"

void printRequest(request *req)
{
    printf("\n========== HTTP REQUEST ==========\n");
    printf("Method:  %s\n", req->method);
    printf("Path:    %s\n", req->path);
    printf("Version: %s\n", req->version);
    printf("\nHeaders:\n");
    for (int i = 0; i < MAX_HEADERS; i++)
    {
        if (strlen(req->headers[i]) == 0)
            break;
        printf("  %s\n", req->headers[i]);
    }
    printf("\nBody:\n");
    if (strlen(req->body) > 0)
        printf("  %s\n", req->body);
    else
        printf("  <empty>\n");
    printf("==================================\n\n");
}

request parse_http_request(char *buffer, ssize_t msglen)
{
    int headers_amount = 0;
    request req;
    memset(&req, 0, sizeof(req));
    char *line_save;
    char *word_save;
    char *body = strstr(buffer, "\r\n\r\n");
    if (body != NULL)
    {
        body += 4;
        snprintf(req.body, sizeof(req.body), "%s", body);
    }
    char *line = strtok_r(buffer, "\r\n", &line_save);
    if (line == NULL)
        return req;
    if (strstr(line, "HTTP/") != NULL)
    {
        snprintf(req.method, sizeof(req.method), "%s", strtok_r(line, " ", &word_save));
        snprintf(req.path, sizeof(req.path), "%s", strtok_r(NULL, " ", &word_save));
        snprintf(req.version, sizeof(req.version), "%s", strtok_r(NULL, " ", &word_save));
    }
    for (int i = 0; i < MAX_HEADERS; i++)
    {
        line = strtok_r(NULL, "\r\n", &line_save);
        if (line == NULL || strlen(line) == 0)
            break;
        snprintf(req.headers[i], sizeof(req.headers[i]), "%s", line);
        headers_amount++;
    }
    req.headers_amount = headers_amount;
    printRequest(&req);
    return req;
}

char *sanitize_path(char *unsanitizedPath, int len)
{
    for (int i = 0; i < len; i++)
    {
        if ((unsanitizedPath[i] == '.' && unsanitizedPath[i + 1] == '/') || (unsanitizedPath[i] == '.' && unsanitizedPath[i + 1] == '.'))
        {
            unsanitizedPath[i] = '/';
        }
    }
    return unsanitizedPath;
}

char *generate_path(request *req)
{
    if (strcmp(req->path, "/") == 0)
    {
        return strdup("html/index.html");
    }
    int length = strlen(req->path);
    char *path = sanitize_path(req->path, length);
    int n = strlen(req->path) + 5;
    char *file_name = (char *)malloc(sizeof(char) * n);
    file_name[n - 1] = '\0';
    if (file_name == NULL)
    {
        return 0;
    }
    snprintf(file_name, n, "%s%s", "html", req->path);
    return file_name;
}

char *read_file(FILE *file, size_t *len)
{
    size_t capacity = 1024;
    size_t length = 0;
    char *buffer = malloc(capacity);
    if (buffer == NULL)
        return NULL;
    while (1)
    {
        size_t n = fread(buffer + length, sizeof(char), capacity - length, file);
        length += n;
        if (n == 0)
            break;
        if (length == capacity)
        {
            capacity *= 2;
            char *tmp = realloc(buffer, capacity);
            if (tmp == NULL)
            {
                free(buffer);
                return NULL;
            }
            buffer = tmp;
        }
    }
    buffer[length] = '\0';
    *len = length;
    return buffer;
}

void handle_get_request(response *resp, request *req)
{
    char *path = generate_path(req);
    printf("Serving resource : %s\n", path);
    FILE *file = fopen(path, "rb");
    free(path);
    if (file == NULL)
    {
        file = fopen(FILE_NOT_FOUND, "rb");
        if (file == NULL)
            internal_server_error(resp);
        perror("Resource not found");
        resp->body = read_file(file, &resp->body_length);
        resp->status_code = 404;
        strcpy(resp->status_message, "Not found");
        fclose(file);
        return;
    }
    resp->body = read_file(file, &resp->body_length);
    if (resp->body == NULL)
        internal_server_error(resp);
    fclose(file);
    return;
}

int parse_query(request *req)
{
    if (strcmp(req->path, "/add") == 0)
    {
        char *token = strtok(req->body, "&");
        char *firstArg = token;
        char *secondArg = strtok(NULL, "&");
        if (firstArg == NULL || secondArg == NULL)
            return -1;
        char *str_x = strchr(firstArg, '=');
        char *str_y = strchr(secondArg, '=');
        if (str_x == NULL || str_y == NULL)
            return -1;
        int x = atoi(str_x + 1);
        int y = atoi(str_y + 1);
        return x + y;
    }
}

void handle_post_request(response *resp, request *req)
{
    for (int i = 0; i < req->headers_amount; i++)
    {
        if (strstr(req->headers[i], "Content-Type"))
        {
            if (strstr(req->headers[i], "x-www-form-urlencoded"))
            {
                int result = parse_query(req);
                snprintf(resp->body, sizeof(resp->body), "%d", result);
                resp->body_length = strlen(resp->body);
                break;
            }
        }
    }
}

http_method parse_method(char *method)
{
    if (strcmp(method, "GET") == 0)
    {
        return METHOD_GET;
    }
    if (strcmp(method, "POST") == 0)
    {
        return METHOD_POST;
    }
}

response generate_http_response(request *req)
{
    response resp;
    memset(&resp, 0, sizeof(resp));
    strcpy(resp.version, "HTTP/1.1");
    strcpy(resp.status_message, "OK");
    resp.status_code = 200;
    switch (parse_method(req->method))
    {
    case METHOD_GET:
        handle_get_request(&resp, req);
        return resp;
    case METHOD_POST:
        handle_post_request(&resp, req);
        return resp;
        break;
    default:
        resp.status_code = 400;
        strcpy(resp.status_message, "Bad Request");
        return resp;
    }
}
