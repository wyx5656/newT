// test_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>  // 添加此行以声明 inet_pton
#include <pthread.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
#define NUM_CLIENTS 1000
#define NUM_REQUESTS 100

void* client_thread(void* arg) {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[1024];
    
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
    
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("连接失败");
        return NULL;
    }
    
    for (int i = 0; i < NUM_REQUESTS; i++) {
        sprintf(buffer, "Message %d from thread %ld\n", i, (long)pthread_self());
        send(client_fd, buffer, strlen(buffer), 0);
        
        // 接收回显
        recv(client_fd, buffer, sizeof(buffer)-1, 0);
    }
    
    close(client_fd);
    return NULL;
}

int main() {
    pthread_t threads[NUM_CLIENTS];
    
    // 编译时需要链接 pthread 库
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_create(&threads[i], NULL, client_thread, NULL);
    }
    
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    return 0;
}