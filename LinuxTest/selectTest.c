#include <func.h>
#include <sys/select.h>
#define MAXLINE 256

int main(int argc, char* argv[])
{
    int fd1 = open("pipe1", O_WRONLY);
    if (fd1 == -1) 
    {
        error(1, errno, "open pipe1");
        printf("1111\n");
    }
    int fd2 = open("pipe2", O_RDONLY);
    printf("1111\n");
    if (fd2 == -1) 
    {
        error(1, errno, "open pipe2");
    }
    printf("Established\n");
    char recvline[MAXLINE];
    char sendline[MAXLINE];

    fd_set mainfds;
    FD_ZERO(&mainfds);
    FD_SET(STDIN_FILENO, &mainfds);
    int maxfds = STDIN_FILENO;

    FD_SET(fd2, &mainfds);
    if(fd2 > maxfds)
    {
        maxfds = fd2;
    }

    for(;;)
    {
        fd_set reads = mainfds;
        int event = select(maxfds + 1, &reads, NULL, NULL, NULL);
        switch (event)
        {
            case -1:
                error(1, errno, "select");
            case 0:
                printf("TIMEOUT\n");
                continue;
            default:
                if (FD_ISSET(STDIN_FILENO, &reads)) 
                {
                    // 一定不会阻塞
                    fgets(sendline, MAXLINE, stdin);
                    // memset(sendline, 0, MAXLINE);
                    write(fd1, sendline, strlen(sendline) + 1); // +1: for '\0'
                }

                if (FD_ISSET(fd2, &reads)) 
                {
                    // 一定不会阻塞
                    int nbytes = read(fd2, recvline, MAXLINE);
                    if (nbytes == 0) {
                        // 管道的写端关闭了
                        goto end;
                    } else if (nbytes == -1) {
                        error(1, errno, "read pipe2");
                }
                printf("from p2: %s", recvline);
            }
        }
    }
    end:
    close(fd1);
    close(fd2);
    
    return 0;
}