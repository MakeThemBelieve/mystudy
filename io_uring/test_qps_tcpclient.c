

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/time.h>

#define TEST_MSSAGE     "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz\r\n"
#define RBUFFER_LENGTH  128
#define TIME_SUB_MS(tv1, tv2) ((tv1.tv_sec - tv2.tv_sec) * 1000 + (tv1.tv_usec - tv2.tv_usec) / 1000)


typedef struct test_context_s {
    char serverip[16];
    int port;
    int threadnum;
    int connection;
    int requestion;

#if 1
    int failed;
#endif
} test_context_t;


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


static void *test_qps_entry(void *arg) {

    test_context_t *pctx =(test_context_t*)arg;

    int connfd = connect_tcpserver(pctx->serverip, pctx->port);
    if (connfd < 0) {
        printf("connnect_tcpserver failed\n");
        return NULL;
    }

    int count = pctx->requestion / pctx->threadnum;
    int i = 0;

    int res; 
    
    while (i++ < count) {
        res = send_recv_tcppkt(connfd);
        if (res != 0) {

            printf("send_recv_tcppkt failed\n");
            pctx->failed++;
            continue;
        }
    }
    
    return NULL;
}

// ./test_qps_tcpclient -s 127.0.0.1 -p 2048 -t 50 -c 100 -n 10000
int main(int argc, char *argv[]){
    
    int ret = 0;
    test_context_t ctx = {0};

    int opt;

    while ((opt = getopt(argc,argv,"s:p:t:c:n:?")) != -1) {
        // 解析命令行参数（不是线程安全的函数）
        switch (opt)
        {
        case 's':
            printf("-s: %s\n", optarg);
            strcpy(ctx.serverip,optarg);
            break;

        case 'p':
            printf("-p: %s\n", optarg);
            ctx.port = atoi(optarg); // 用于将字符串转换为整数
            break;

        case 't':
            printf("-t: %s\n", optarg);
            ctx.threadnum = atoi(optarg);
            break;

        case 'c':
            printf("-c: %s\n", optarg);
            ctx.connection = atoi(optarg);
            break;

        case 'n':
            printf("-n: %s\n", optarg);
            ctx.requestion = atoi(optarg);
            break;

        default:
            return -1;
        }
    }

    //栈上分配
    // pthread_t ptid[10] = {0};
    //堆上分配
    pthread_t *ptid = malloc(ctx.threadnum * sizeof(pthread_t));
    int i = 0;

    struct timeval tv_begin;
    gettimeofday(&tv_begin, NULL);
    for (i = 0;i < ctx.threadnum; i++) {
        pthread_create(&ptid[i], NULL, test_qps_entry, &ctx);
    }

    for (i = 0;i < ctx.threadnum; i++) {
        pthread_join(ptid[i], NULL);
    }

    struct timeval tv_end;
    gettimeofday(&tv_end, NULL);

    int time_used = TIME_SUB_MS(tv_end, tv_begin);

    printf("success: %d,failed: %d, time_used: %d\n", ctx.requestion-ctx.failed, ctx.failed, time_used);

clean:
    // clean 是一个标签。
    free(ptid);
    //释放防止内存泄漏
    return 0;
}
