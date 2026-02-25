#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <error.h>
#define MAXLINE 256
int main(int argc, char* argv[])
{
    int fd1 = open("pipe1", O_WRONLY);
    int fd2 = open("pipe2", O_RDONLY);
    if(-1 == fd2)
    {
        error(1,errno, "open pipe2");
    }

    char recvline[MAXLINE];
    char sendline[MAXLINE];

    while (fgets(sendline, MAXLINE, stdin) != NULL)
    {
        write(fd1, sendline, strlen(sendline));
        read(fd2, recvline, MAXLINE);
        printf("from p2 : %s\n",recvline);
    }

    close(fd1);
    close(fd2);

    return 0;
}

