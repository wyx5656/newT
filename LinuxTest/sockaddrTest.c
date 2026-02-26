#include <stdio.h>
#include <stdint.h>      // 添加此行以支持 uint16_t, uint32_t 等类型
#include <arpa/inet.h>   

int main()
{
    uint16_t port = 8888;
    uint16_t net_port = htons(port);

    printf("端口 - 主机序: %u (0x%04X), 网络序: %u (0x%04X)\n", 
           port, port, net_port, net_port);
    
    uint32_t ip = inet_addr("192.168.1.1");
    uint32_t net_ip = htonl(ip);

    printf("IP - 主机序: %u (0x%08X), 网络序: %u (0x%08X)\n", 
           ip, ip, net_ip, net_ip);
           
    // 以点分十进制格式显示IP
    struct in_addr addr;
    addr.s_addr = net_ip;
    printf("IP地址(网络序): %s\n", inet_ntoa(addr));

    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("tcp socketfd %d\n", socketfd);
    
    int socketfd1 = socket(AF_INET, SOCK_DGRAM, 0);
    printf("ucp socketfd %d\n", socketfd1);
    return 0;
}
