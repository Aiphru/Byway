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
        strcpy(req.method, strtok_r(line, " ", &word_save));
        strcpy(req.path, strtok_r(NULL, " ", &word_save));
        strcpy(req.version, strtok_r(NULL, " ", &word_save));
    }
    for (int i = 0; i < MAX_HEADERS; i++)
    {
        line = strtok_r(NULL, "\r\n", &line_save);
        if (line == NULL || strlen(line) == 0)
            break;
        strcpy(req.headers[i], line);
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
            printf("Dot detected \n");
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

void handle_get_request(response *resp, request *req)
{
    char buffer[4096];
    // int content_length = 0;
    char *path = generate_path(req);
    printf("Serving resource : %s\n", path);
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        FILE *notFound = fopen("html/404.html", "r");
        perror("Resource not found");
        while (fgets(buffer, sizeof(resp->body), notFound))
        {
            strcat(resp->body, buffer);
            // content_length += strlen(buffer);
        }
        resp->status_code = 404;
        strcpy(resp->status_message, "Not found");
        snprintf(resp->body_length, sizeof(resp->body_length), "%zu", strlen(resp->body));
        fclose(notFound);
        free(path);
        return;
    }
    while (fgets(buffer, sizeof(resp->body), file))
    {
        strcat(resp->body, buffer);
        // content_length += strlen(buffer);
    }
    snprintf(resp->body_length, sizeof(resp->body_length), "%zu", strlen(resp->body));
    fclose(file);
    free(path);
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
                snprintf(resp->body_length, sizeof(resp->body_length), "%zu", strlen(resp->body));
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
