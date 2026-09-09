#include "serv.h"

int send_response(int client_fd, response *resp)
{
    char response_buffer[1024];

    int len = snprintf(
        response_buffer,
        sizeof(response_buffer),
        "%s %d %s\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s",
        resp->version,
        resp->status_code,
        resp->status_message,
        resp->body_length,
        resp->body);
    return send(client_fd, response_buffer, len, 0);
}

int httpServer()
{
    int current_size = 512;
    char *buffer = malloc(sizeof(char) * current_size);
    int server_fd, client_fd;
    ssize_t msglen;
    int option = 1;
    request req;
    size_t received = 0;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listening on port %d...\n", PORT);
    if (listen(server_fd, 5) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        if ((client_fd = accept(server_fd, (struct sockaddr *)&addr, &addrlen)) < 0)
        {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
        received = 0;
        buffer[0] = '\0';
        do
        {
            if (received >= current_size - 1)
            {
                current_size *= 2;
                char *tmp = realloc(buffer, current_size);
                if (tmp == NULL)
                {
                    free(buffer);
                    return -1;
                }
                buffer = tmp;
            }
            msglen = recv(client_fd, buffer + received, current_size - received - 1, 0);
            if (msglen <= 0)
            {
                break;
            }
            received += msglen;
            buffer[received] = '\0';
        } while (strstr(buffer, "\r\n\r\n") == NULL);
        printf("----- RAW REQUEST -----\n");
        printf("%.*s", (int)received, buffer);
        printf("\n-----------------------\n");
        req = parse_http_request(buffer, received);
        response resp = generate_http_response(&req);
        if (send_response(client_fd, &resp))
        {
            free(resp.body);
            printf("Response sent \n");
        }
        close(client_fd);
    }
    return 0;
}
