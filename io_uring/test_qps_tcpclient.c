

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <arpa/inet.h>



#define TEST_MSSAGE     "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz\r\n"
#define RBUFFER_LENGTH  128

static void *test_qps_entry(void *arg) {
    
}



//建立TCP连接
int connect_tcpserver(const char *ip, unsigned short port) {

    int connfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in tcpserver_addr;
    memset(&tcpserver_addr, 0, sizeof(struct sockaddr_in));

    tcpserver_addr.sin_family = AF_INET;
    tcpserver_addr.sin_addr.s_addr = inet_addr(ip);
    // 将 IPv4 地址的点分十进制表示形式转换为网络字节顺序的 32 位整数
    tcpserver_addr.sin_port = htons(port);
    // htonl：用于将一个 32 位整数（如 IP 地址）转换为网络字节顺序。
    // htons：用于将一个 16 位整数（如端口号）转换为网络字节顺序。

    int ret = connect(connfd, (struct sockaddr*)&tcpserver_addr, sizeof(struct sockaddr_in));
    // 在客户端，连接到服务器的指定地址和端口
    if (ret){
        perror("connect\n");
        return -1;
    }

    return connfd;
}


int send_recv_tcppkt(int fd) {

    //发送测试文本到fd
    int res = send(fd, TEST_MSSAGE, strlen(TEST_MSSAGE), 0);
    if (res < 0) {
        exit(1);
        //结束会话
    }
    char rbuffer[RBUFFER_LENGTH] = {0};
    res = recv(fd, rbuffer, RBUFFER_LENGTH, 0);
    if (res < 0) {
        exit(1);
    }

    if (strcmp(rbuffer, TEST_MSSAGE) != 0) {   //strcmp比较<0左边字典序大，>0右边字典序大，=0完全相等
        printf("faild: '%s' != '%s'", rbuffer, TEST_MSSAGE);
        return -1;
    }
    return 0;
}

// ./test_qps_tcpclient -s 127.0.0.1 -p 2048 -t 50 -c 100 -n 10000
int main(int argc, char *argv[]){
    
    char serverip[16] = {0};
    int port;
    int threadnum = 0;
    int connection = 0;
    int requestion = 0;

    int opt;

    while ((opt = getopt(argc,argv,"s:p:t:c:n:?")) != -1) {
        // 解析命令行参数（不是线程安全的函数）
        switch (opt)
        {
        case 's':
            printf("-s: %s\n", optarg);
            strcpy(serverip,optarg);
            break;

        case 'p':
            printf("-p: %s\n", optarg);
            port = atoi(optarg); // 用于将字符串转换为整数
            break;

        case 't':
            printf("-t: %s\n", optarg);
            threadnum = atoi(optarg);
            break;

        case 'c':
            printf("-c: %s\n", optarg);
            connection = atoi(optarg);
            break;

        case 'n':
            printf("-n: %s\n", optarg);
            requestion = atoi(optarg);
            break;

        default:
            return -1;
        }
    }

    int connfd = connect_tcpserver(serverip, port);
    if (connfd < 0) {
        printf("connnect_tcpserver failed\n");
        return -1;
    }

    int ret = send_recv_tcppkt(connfd);
    if (ret != 0) {
        printf("send_recv_tcppkt failed\n");
        return -1;
    }

    printf("success\n");
    return 0;
}
