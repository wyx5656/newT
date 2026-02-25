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
    int fd1 = open("pipe1", O_RDONLY);
    if (fd1 == -1) {
        error(1, errno, "open pipe1");
    }
    int fd2 = open("pipe2", O_WRONLY);
    if (fd2 == -1) {
        error(1, errno, "open pipe2");
    }
    printf("Established\n");
    char recvline[MAXLINE];
    char sendline[MAXLINE];
    while (fgets(sendline, MAXLINE, stdin) != NULL) {
        write(fd2, sendline, strlen(sendline));
        read(fd1, recvline, MAXLINE);
        printf("from p1: %s\n", recvline);
    }
close(fd1);
close(fd2);
return 0;
}