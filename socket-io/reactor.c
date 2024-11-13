
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ENALE_HTTP_RESPONSE   1
#define BUFFER_LISTEN   1024
#define ROOT_DIR "/home/xzs/share/mystudy/socket-io/"

    typedef int (*RCALLBACK)(int fd);

// int accept_cb(int fd);
// int recv_cb(int fd);
// int recv_cb(int fd);


struct conn_item
{
    int fd;

    char rbuffer[BUFFER_LISTEN];
    int rlen;
    char wbuffer[BUFFER_LISTEN];
    int wlen;

    char resource[BUFFER_LISTEN];

    union {
        RCALLBACK accept_callback;
        RCALLBACK recv_callback;
    } recv_t;
    RCALLBACK send_callback;
};
//libevent


#if ENALE_HTTP_RESPONSE

typedef struct conn_item connection_t;

int http_request(connection_t *conn) {
    
    
    return 0;
}


int http_response(connection_t *conn)
{
    int filefd = open("test.html", O_RDONLY);
    
    struct stat stat_buf;  //获取文件状态
    fstat(filefd, &stat_buf);

#if 1
    conn->wlen = sprintf(conn->wbuffer,
    "HTTP/1.1 200 OK\r\n"
    "Accept-Ranges: bytes\r\n"
    "Content-Length: 44\r\n"
    "COntent-Type: text/html\r\n"
    "Date: Sat, 06 Aug 2024 13:16:46 GMT\r\n\r\n"
    "<html><head><title>test</title><head></html>\r\n\r\n");

#elif 0

    conn->wlen = sprintf(conn->wbuffer,
    "HTTP/1.1 200 OK\r\n"
    "Accept-Ranges: bytes\r\n"
    "Content-Length: %ld\r\n"
    "COntent-Type: text/html\r\n"
    "Date: Sat, 06 Aug 2024 13:16:46 GMT\r\n\r\n",stat_buf.st_size);

    int count = read(filefd, conn->wbuffer + conn->wlen, BUFFER_LISTEN - conn->wlen);
    conn->wlen += count;
#endif
    return conn->wlen;
}

#endif



int epfd = 0;
struct conn_item connlist[1024] = {0};


int set_event(int fd,int event,int flag){

    if (flag) { // 1 add 0 mod
        struct epoll_event ev;
        ev.events = event;
        ev.data.fd = fd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    }else{
        struct epoll_event ev;
        ev.events = event;
        ev.data.fd = fd;
        epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
    }
}


//clientfd
int recv_cb(int fd){

    char *buffer = connlist[fd].rbuffer;
    int idx = connlist[fd].rlen;

    int count = recv(fd, buffer+idx, BUFFER_LISTEN-idx, 0);
    if (count == 0)
    {
        //printf("disconnect\n");

        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);

        return -1;
    }
    connlist[fd].rlen+= count;

#if 0
    memcpy(connlist[fd].wbuffer,connlist[fd].rbuffer,connlist[fd].rlen);
    //内容拷贝
    connlist[fd].wlen = connlist[fd].rlen;
    //event
#elif 1

    http_request(&connlist[fd]);
    http_response(&connlist[fd]);

#endif

    set_event(fd, EPOLLOUT, 0);
    return count;
}



int send_cd(int fd){

    char *buffer = connlist[fd].wbuffer;
    int idx = connlist[fd].wlen;

    int count = send(fd, buffer, idx, 0);

    //event
    set_event(fd, EPOLLIN, 0);

    return count;
}

// listenfd
int accept_cb(int fd)
{

    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);

    int clientfd = accept(fd, (struct sockaddr *)&clientaddr, &len);
    if (clientfd < 0)
    {
        return -1;
    }

    set_event(clientfd, EPOLLIN, 1);

    connlist[clientfd].fd = clientfd;
    memset(connlist[clientfd].rbuffer, 0, BUFFER_LISTEN);
    connlist[clientfd].rlen = 0;
    memset(connlist[clientfd].wbuffer, 0, BUFFER_LISTEN);
    connlist[clientfd].wlen = 0;
    connlist[clientfd].recv_t.recv_callback = recv_cb;
    connlist[clientfd].send_callback = send_cd;

    return clientfd;
}

// tcp
int main(int argc, char const *argv[])
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    //创建一个套接字，相当于针对网卡开放一个接口，参数：ip协议类型(ipv4)、数据传输协议(TCP)
    // 为0，表示使用默认的协议。对于AF_INET和SOCK_STREAM，默认协议是TCP。

    struct sockaddr_in serveraddr; // 创建地址结构体
    memset(&serveraddr, 0, sizeof(struct sockaddr_in)); // 分配地址内存
    // 第一项表示要初始化的内存区域的起始地址。
    // 第二项表示要用来填充内存的值
    // 第三项表示要初始化的内存区域的大小，这里取了整个结构体大小，确保被完整初始化

    serveraddr.sin_family = AF_INET;                // 指定网络地址的类型
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); // 设置监听网段为(INADDR_ANY)所有
    serveraddr.sin_port = htons(2048); // 设置端口号为2048

    /// bind，成功返回0，出错返回-1 ，bind() 用于将socket与特定的IP地址和端口进行绑定。
    // 第一项是要绑定的socket的文件描述符
    // 第二项是指向包含IP地址和端口信息的sockaddr结构体的指针
    // 第三项是第二项的类型即结构体的长度。 
    if (-1 == bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr)))
    {
        perror("bind"); //输出错误信息
        return -1;
    }
    listen(sockfd, 10);
    // 作用是用来监听服务器创建的socket的。第二个参数代表的是相应的socket可以排队的最大连接数。


    connlist[sockfd].fd = sockfd;
    connlist[sockfd].recv_t.accept_callback = accept_cb;

    epfd = epoll_create(1); // size 参数被忽略，但必须大于零, 返回一个非负整数作为 epoll 实例的文件描述符
    set_event(sockfd,EPOLLIN,1);

    struct epoll_event events[1024] = {0};
    while (1)  //mainloop()
    {
        int nready = epoll_wait(epfd, events, 1024, -1);
        // 用于等待文件描述符上的 I / O 事件发生, 返回就绪的文件描述符数量
        // 第一项是监听的epoll实例，第二项是就绪事件指针，第三项是最大事件数量，第四项最大时间

        int i = 0;
        for ( i = 0; i < nready; i++)
        {
            int connfd = events[i].data.fd;
            if (events[i].events & EPOLLIN) {

                int count = connlist[connfd].recv_t.recv_callback(connfd);
                //printf("recv  count:%d <---buffer: %s\n",count, connlist[connfd].rbuffer);
            }else if (events[i].events & EPOLLOUT) {

                int count = connlist[connfd].send_callback(connfd);
                //printf("send-->buffer: %s\n", connlist[connfd].wbuffer);
            }
        }
    }

    getchar();

    return 0;
}
