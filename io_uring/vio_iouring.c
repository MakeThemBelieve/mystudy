
#include <sys/socket.h>
#include <netinet/in.h>

#include <string.h>
#include <liburing.h>
#include <stdio.h>
#include <unistd.h>


#define ENTERIES_LENGTH         4096

enum {
    READ,
    WRITE,
    ACCEPT,
};

struct conninfo {
    int confd;
    int type;
};

void set_write_event(struct io_uring *ring, int fd, const void *buf, size_t len, int flags)
{
    // 非系统调用，获取一个可用的 submit_queue_entry，用来提交IO
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);

    // 准备一个异步的发送请求
    io_uring_prep_send(sqe, fd, buf, len, flags);

    struct conninfo ci = {
        .confd = fd,
        .type = WRITE
    };

    memcpy(&sqe->user_data, &ci, sizeof(struct conninfo));
}


void set_read_event(struct io_uring *ring, int fd, void *buf, size_t len, int flags) {

    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    // 从提交队列中获取一个即将提交给内核进行异步I/O操作的请求。（获取的是队列的首地址）

    io_uring_prep_recv(sqe, fd, buf, len, flags);
    // 让内核准备接收数据的异步 I/O 操作

    struct conninfo ci = {
        .confd = fd,
        .type = READ
        };

    // 复制内存的内容
    memcpy(&sqe->user_data, &ci, sizeof(struct conninfo));
}

void set_accept_event(struct io_uring *ring,int fd,
 struct sockaddr *cliaddr, socklen_t *clilen, unsigned flags) {

    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    // 从提交队列中获取一个即将提交给内核进行异步I/O操作的请求。（获取的是队列的首地址）

    io_uring_prep_accept(sqe, fd, cliaddr, clilen, flags);
    // 用io_uring API 提供的一个函数，为一个 accept 系统调用准备一个提交队列条目（SQE）。这会告诉内核异步接受请求

    struct conninfo ci = {
        .confd = fd,
        .type = ACCEPT
    };

    // 复制内存的内容
    memcpy(&sqe->user_data, &ci, sizeof(struct conninfo));
}



// io_uring, tcp_server

int main() {

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    // 创建一个套接字，相当于针对网卡开放一个接口，参数：ip协议类型(ipv4)、数据传输协议(TCP)
    if (listenfd == -1) return -1;

    struct sockaddr_in serveraddr,clientaddr;      // 创建地址结构体
    serveraddr.sin_family = AF_INET;                // 指定网络地址的类型
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); // 设置监听网段为(INADDR_ANY)所有
    serveraddr.sin_port = htons(2048);              // 设置端口号为2048

    /// bind，成功返回0，出错返回-1 ，bind() 用于将socket与特定的IP地址和端口进行绑定。
    if (-1 == bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr))) {
        return -2;
    }
    
    listen(listenfd, 10);

    // 在内核中注册了两个队列，然后mmap映射出来

    //io_uring配置
    struct io_uring_params params;
    memset(&params, 0, sizeof(params));

    // 用于初始化一个 io_uring 实例(提交队列)并设置相关参数
    struct io_uring ring;
    io_uring_queue_init_params(ENTERIES_LENGTH, &ring, &params);

    socklen_t clilen = sizeof(clientaddr);
    set_accept_event(&ring, listenfd, (struct sockaddr*)&clientaddr, &clilen, 0);
   
    char buffer[1024] = {0};
    while (1) {

        struct io_uring_cqe  *cqe;

        io_uring_submit(&ring);
        // 会将 I/O 请求从 io_uring 的提交队列提交给内核，内核会根据请求执行相应的异步 I/O 操作
        //相当于accept

        int ret = io_uring_wait_cqe(&ring, &cqe);
        // 阻塞当前线程，直到有一个 I/O 请求完成并返回结果。(指针指向已完成的队列中的事件)

        struct io_uring_cqe *cqes[10];
        int cqecount = io_uring_peek_batch_cqe(&ring, cqes, 10);
        // 用于非阻塞地立即查看多个完成队列事件（指针赋值）

        int i = 0;
        unsigned count = 0;
        for (i; i < cqecount; i++) {

            cqe = cqes[i];
            count++;

            struct conninfo ci;
            memcpy(&ci, &cqe->user_data, sizeof(ci));
        
            if (ci.type == ACCEPT) {

                int connfd = cqe->res;

                set_read_event(&ring, connfd, buffer, 1024, 0);

            } else if (ci.type == READ) {

                int bytes_read = cqe->res;

                if (bytes_read == 0)
                { // 读到的数据为0，表示已经断开连接
                    close(ci.confd);
                }
                else if (bytes_read < 0){
                } else {

                    printf("buffer : %s\n", buffer);
                    // echo
                    set_write_event(&ring, ci.confd, buffer, bytes_read, 0);
                }
            } else if (ci.type == WRITE) {

                set_read_event(&ring, ci.confd, buffer, 1024, 0);
            }
            
        
        io_uring_cq_advance(&ring, count);
        // 在处理完成队列中的事件后更新完成队列的头部指针，指示已完成的事件数量(清空队列)
    }

//    getchar();
    }
}










