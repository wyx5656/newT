#include <func.h>
#include <signal.h>

typedef void(*sighandler_t)(int);

sighandler_t signal(int signum, sighandler_t handler);

void handler(int signo) {
    switch (signo) 
    {
        case SIGINT:
            printf("Caught SIGINT\n");
            break;
        case SIGTSTP:
            printf("Caught SIGTSTP\n");
            break;
        default:
            printf("Unknown %d\n", signo);
    }
}

int main(int argc, char* argv[])
{
    // 注册信号处理函数 (捕获信号)
    sighandler_t oldhandler = signal(SIGINT, handler);
    if (oldhandler == SIG_ERR) {
        error(1, errno, "signal %d", SIGINT);
    }
    oldhandler = signal(SIGTSTP, handler);
    if (oldhandler == SIG_ERR) {
        error(1, errno, "signal %d", SIGTSTP);
    }
    for(;;) {
    }
    return 0;
}


