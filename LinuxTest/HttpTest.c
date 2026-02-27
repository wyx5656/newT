#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 4096

// 简单的 HTML 页面
const char *html_template =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=utf-8\r\n"
"Content-Length: %zu\r\n"
"\r\n"
"<!DOCTYPE html>"
"<html><head><title>我的 HTTP 服务器</title></head>"
"<body><h1>Hello from C HTTP Server!</h1>"
"<p>这是阶段 6 综合项目</p>"
"<p>你好，%s！</p></body></html>";

// 404 响应
const char *not_found =
"HTTP/1.1 404 Not Found\r\n"
"Content-Type: text/plain\r\n"
"Content-Length: 13\r\n"
"\r\n"
"404 Not Found";

int main() 
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) { perror("socket"); exit(1); }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, 5) == -1) {
        perror("listen");
        exit(1);
    }

    printf("极简 HTTP 服务器启动，访问 http://127.0.0.1:%d\n", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        memset(buffer, 0, BUFFER_SIZE);
        recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

        // 简单解析请求行（第一行）
        char method[16], path[256], version[16];
        sscanf(buffer, "%s %s %s", method, path, version);

        printf("收到请求: %s %s %s\n", method, path, version);

        if (strcmp(method, "GET") == 0) {
            char name[64] = "访客";
            char *query = strstr(path, "?name=");
            if (query) {
                sscanf(query, "?name=%63s", name);  // 简单提取 name 参数
            }

            // 构造响应
            char response[BUFFER_SIZE];
            int body_len = snprintf(response, sizeof(response), html_template, strlen(name) + 100, name);

            send(client_fd, response, body_len, 0);
        } else {
            send(client_fd, not_found, strlen(not_found), 0);
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}