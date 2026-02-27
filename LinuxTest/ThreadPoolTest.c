#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string.h>
#define NUMWORKS 4
#define MSGLEN 64

int main()
{
    pid_t pids[NUMWORKS];
    int pipes[NUMWORKS][2];
    for (int i = 0; i < NUMWORKS; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            exit(1);
        }

        pids[i] = fork();
        if (0 == pids[i])
        {
            close(pipes[i][1]);
            char Msg[MSGLEN];

            while(1)
            {
                int n = read(pipes[i][0], Msg, sizeof(Msg));
                if (n <= 0)
                {
                    break;
                }

                Msg[n] = '\0';
                printf("子进程 %d(PID %d) 收到任务 %s\n", i, getpid(), Msg);
                sleep(5);
                printf("子进程 %d pid %d的任务完成\n",i, getpid());
            }

            close(pipes[i][0]);
            exit(0);    
        }
    }

// 主进程关闭所有读端
    for (int i = 0; i < NUMWORKS; i++) {
        close(pipes[i][0]);
    }

    printf("主进程创建 %d 个子进程完成\n", NUMWORKS);
    // 模拟分发任务
    for (int i = 0; i < 100; i++) {
        int worker = i % NUMWORKS;  // 轮询分发
        char msg[MSGLEN];
        snprintf(msg, sizeof(msg), "任务 %d", i + 1);

        write(pipes[worker][1], msg, strlen(msg));
        printf("主进程分发任务 %d 给子进程 %d\n", i + 1, worker);

        sleep(1);
    }

    // 通知子进程退出（发送空消息或关闭管道）
    for (int i = 0; i < NUMWORKS; i++) {
        close(pipes[i][1]);
    }

    // 等待子进程结束
    for (int i = 0; i < NUMWORKS; i++) {
        waitpid(pids[i], NULL, 0);
    }

    printf("所有子进程退出，主进程结束\n");
    return 0;
}