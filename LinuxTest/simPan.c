#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#define PORT 8888
#define NUM_WORKERS 3
#define BUFFER_SIZE 1024

int worker_pipes[NUM_WORKERS][2];  // 父写子读的管道
pid_t worker_pids[NUM_WORKERS];

void worker_process(int id) {
    close(worker_pipes[id][1]);  // 子关闭写端

    char buf[BUFFER_SIZE];
    while (1) {
        int n = read(worker_pipes[id][0], buf, BUFFER_SIZE);
        if (n <= 0) break;

        buf[n] = '\0';
        printf("[子进程 %d] 收到任务：%s\n", id, buf);

        // 模拟文件传输
        sleep(3);
        printf("[子进程 %d] 任务完成\n", id);
    }

    close(worker_pipes[id][0]);
    exit(0);
}

int main() {
    int listen_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // 创建监听 socket
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(listen_fd, 5);

    printf("网盘服务器启动，监听端口 %d...\n", PORT);

    // 创建进程池
    for (int i = 0; i < NUM_WORKERS; i++) {
        if (pipe(worker_pipes[i]) == -1) {
            perror("pipe");
            exit(1);
        }

        worker_pids[i] = fork();
        if (worker_pids[i] == 0) {
            worker_process(i);
        }

        close(worker_pipes[i][0]);  // 父关闭读端
    }

    printf("已创建 %d 个工作进程\n", NUM_WORKERS);

    while (1) {
        int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) continue;

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("客户端连接: %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        // 模拟任务（实际中从客户端读文件名或文件内容）
        char task[BUFFER_SIZE];
        snprintf(task, sizeof(task), "传输文件: photo.jpg 大小: 2.5MB");

        // 轮询找一个子进程（实际中可以用空闲队列）
        int worker_id = client_fd % NUM_WORKERS;  // 简单轮询
        write(worker_pipes[worker_id][1], task, strlen(task));

        close(client_fd);  // 模拟传输完成关闭
    }

    // 清理（实际中优雅关闭）
    for (int i = 0; i < NUM_WORKERS; i++) {
        close(worker_pipes[i][1]);
        kill(worker_pids[i], SIGTERM);
        waitpid(worker_pids[i], NULL, 0);
    }

    close(listen_fd);
    return 0;
}