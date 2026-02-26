    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <sys/select.h>

    #define SERVERIP "127.0.0.1"
    #define PORT 8888
    #define BUFFERSIZE 1024

    int main()
    {
        int sockfd;
        struct sockaddr_in server_addr;
        char buffer[BUFFERSIZE];
        fd_set readFds;
        int maxfd = 0;
        int ret = 0;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == sockfd)
        {
            perror("inet_pton");
            exit(1);
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        if (inet_pton(AF_INET, SERVERIP, &server_addr.sin_addr) <= 0)
        {
            perror("inet_pton");
            exit(1);
        }

        if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        {
            perror("connect");
            exit(1);
        }

        printf("已连接到服务器 %s:%d\n", SERVERIP, PORT);

        while(1)
        {
            FD_ZERO(&readFds);
            FD_SET(sockfd, &readFds);
            FD_SET(STDIN_FILENO, &readFds);
            if (sockfd > STDIN_FILENO)
            {
                maxfd = sockfd;
            }
            else
            {
                maxfd = STDIN_FILENO;
            }

            select(maxfd + 1, &readFds, NULL, NULL, NULL);
            printf("sockfd %d, jianpan %d, maxfd %d\n", sockfd, STDIN_FILENO, maxfd);
            if (FD_ISSET(STDIN_FILENO, &readFds))
            {
                printf("bbbbbbbbbbbb\n");
                memset(buffer, 0, BUFFERSIZE);
                ret = read(STDIN_FILENO, buffer, sizeof(buffer));
                if(ret == 0)
                {
                    break;
                }

                send(sockfd, buffer, strlen(buffer), 0);
            }

            if (FD_ISSET(sockfd, &readFds))
            {
                printf("sockfd 就绪，开始 recv...\n");  // 加打印验证是否进入
                printf("aaaaa\n");
                while (1) 
                {  // 循环读完所有数据
                    memset(buffer, 0, BUFFERSIZE);
                    int n = recv(sockfd, buffer, BUFFERSIZE - 1, 0);
                    printf("recv 返回 n = %d\n", n);
                    if (n <= 0) {
                        if (n < 0) perror("recv");
                        printf("服务器断开\n");
                        break;
                    }

                    buffer[n] = '\0';
                    printf("收到: %s", buffer);  // 注意这里用 %s 不加 \n，因为服务器已带 \n
                    break;
                }

            }
        }

        close(sockfd);
        printf("已断开连接\n");
        return 0;
    }




