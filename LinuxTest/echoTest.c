#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PORT 8888
#define BUFFER_SIZE 1024
#define MAXCLIENT 5
int main()
{
    int serverFd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];
    fd_set readfds;
    int maxFd, clientCount = 0;
    int clientFds[MAXCLIENT]; 

    for (int i = 0; i < MAXCLIENT; i++)
    {
        clientFds[i] = -1;
    }

    //1、 创建socket
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == serverFd)
    {
        perror("socket");
        exit(1);
    }

    // 允许端口复用 (避免 TIME_WAIT导致bind 失败)
    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 绑定地址
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("bind");
        exit(1);
    }

    // 监听(允许5个排队连接)
    if (listen(serverFd, 5) == -1)
    {
        perror("listen");
        exit(1);
    }

    printf("TCP Echo Server 启动, 监听端口 %d...\n", PORT);

    while(1)
    {
        FD_ZERO(&readfds);
        //将监听fd添加到select
        FD_SET(serverFd, &readfds);
        maxFd = serverFd;
        //将客户端fd添加到select
        for(int i = 0; i < MAXCLIENT; i++)
        {
            if (clientFds[i] != -1)
            {
                FD_SET(clientFds[i], &readfds);
            }

            if (clientFds[i] > maxFd)
            {
                maxFd = clientFds[i];
            }
        }

        int events = select(maxFd + 1, &readfds, NULL, NULL, NULL);
        if (events < 0)
        {
            perror("select");
            exit(1);
        }

        if (FD_ISSET(serverFd, &readfds))
        {
            int newFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientLen);
            if (newFd < 0)
            {
                perror("accept");
                continue;
            }

            for (int i = 0; i < MAXCLIENT; i++)
            {
                if (-1 == clientFds[i])
                {
                    clientFds[i] = newFd;
                    clientCount++;
                    printf("新客户端连接 (fd = %d), 当前在线 ：%d\n", newFd, clientCount);
                    break;
                }
            }

            if (MAXCLIENT == clientCount)
            {
                printf("客户端已满， 拒绝连接\n");
                close(newFd);
            }

        }
        
        for (int i = 0; i < MAXCLIENT; i++)
        {

            printf("i = %d, fd is %d\n",i, clientFds[i]);
            if (-1 == clientFds[i])
            {
                continue;
            }

            if (FD_ISSET(clientFds[i], &readfds))
            {
                memset(buffer, 0, BUFFER_SIZE);
                int n = recv(clientFds[i], buffer, BUFFER_SIZE - 1, 0);
                printf("recv n = %d\n",n);
                if (n <= 0)
                {
                    printf("客户端 fd=%d 断开，当前在线: %d\n", clientFds[i], --clientCount);
                    close(clientFds[i]);
                    clientFds[i] = -1;
                }
                else
                {
                    buffer[n] = '\0';
                    printf("收到消息：（fd = %d): %s\n", clientFds[i], buffer);
                    // 立刻回显给发送者（触发 ACK，让客户端知道已收到）
                    char ack[] = "已收到\n";
                    int n = send(clientFds[i], ack, strlen(ack), 0);
                    {
                        printf("send number %d\n", n);
                    }

                    for (int j = 0; j < MAXCLIENT; j++)
                    {
                        if (clientFds[j] != -1 && clientFds[j] != clientFds[i])
                        {
                            int n = send(clientFds[j], buffer, strlen(buffer), 0);
                            printf("send to fd %d number %d\n",clientFds[j],n);
                        }
                    }

                }
            }
        }
    }

    close(serverFd);
    return 0;
}