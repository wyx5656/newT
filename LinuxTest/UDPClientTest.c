#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8888
#define BUFFER_SIZE 1024

int main() 
{
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("connect");
        exit(1);
    }

    printf("已连接到聊天室！输入消息回车发送（输入 quit 退出）\n");

    while (1) 
    {
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;  // 去掉换行

        if (strcmp(buffer, "quit") == 0) break;

        send(sockfd, buffer, strlen(buffer), 0);

        int n = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (n > 0) 
        {
            buffer[n] = '\0';
            printf("%s", buffer);
        }
    }

    close(sockfd);
    printf("已退出聊天室\n");
    return 0;
}