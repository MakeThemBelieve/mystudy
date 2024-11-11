
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




void *client_thread(void *arg){
    int clientfd = *(int *)arg;
    
    while (1)
    {
        char buffer[128] = {0};
        int count = recv(clientfd, buffer, 128, 0);
        if(count == 0) {
            break;
        }
        send(clientfd,buffer,count,0);
        printf("clientfd:%d,count:%d,buffer:;%s\n",clientfd,count,buffer);
    }
    
    close(clientfd);
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

#if(0)
        
#elif0 //一个线程一个客户端

        while (1)
    {

        struct sockaddr_in clientaddr;
        socklen_t len = sizeof(clientaddr);
        int clientfd = accept(sockfd,(struct sockaddr*)&clientaddr,&len);

        pthread_t thid;
        pthread_create(&thid,NULL,client_thread,&clientfd);

    }
#elif  0 //select

    fd_set rfds, rset; 
    //select() 机制中使用的数据结构，它实际上是一个 long 类型1024长度的数组。每个数组元素都能与一个打开的文件句柄建立联系。这种联系是由程序员通过特定的宏来建立的
    FD_ZERO(&rfds);
    //用于清零 fd_set集合，即清除集合中的所有文件描述符
    FD_SET(sockfd, &rfds);
    // 用于将文件描述符 fd 添加到 fd_set 集合中。
    int maxfd = sockfd;
    // 更新最大文件描述符

    while (1)
    {
        
        rset = rfds;
        // 将需要监听的 rfds 拷贝到 rset 中
        int nready = select(maxfd+1,&rset,NULL,NULL,NULL);
        // select(maxfd,rset,wset,eset,timeout)   最大fd，可读集合，可写集合，错误集合，超时时间
        // maxfd主要用于内部最大循环
        if(FD_ISSET(sockfd,&rset)){
            // 用于测试文件描述符 fd 是否在 fd_set 集合中，如果在则返回非零值，否则返回零。
            struct sockaddr_in clientaddr;
            socklen_t len = sizeof(clientaddr);
            int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
            // accept函数接收客户端连接，
            // 第一项是用于标识要监听套接字。
            // 第二项是一个指向struct sockaddr类型变量的指针，用于接收客户端地址信息。
            // 第三项是一个指向socklen_t类型变量的指针，用于指定结构体的长度。
            // 如果accept函数调用成功，它将返回一个新的socket描述符，这个新的描述符用于与客户端进行通信。原先的sockfd描述符仍然保持为监听状态，可以继续接收其他客户端的连接请求。
            // 如果accept函数调用失败，它将返回 -1，并且可以通过errno来获取错误信息。
                printf("sockfd:%d\n", clientfd);

            FD_SET(clientfd,&rfds);
            maxfd = clientfd;
        }

        int i = 0;
        for ( i = sockfd+1; i <= maxfd; i++)
        {
            if( FD_ISSET(i,&rset)){
                char buffer[128] = {0};
                int count = recv(i, buffer, 128, 0);
                // recv的工作原理是从指定的套接字接收数据，并将其存储在缓冲区中。如果函数执行成功，它会返回实际接收到的字节数。如果返回值为0，则表示对端已经关闭连接。如果返回值为-1，则表示发生了错误，可以通过errno变量来获取具体的错误信息。
                // 第一项代表套接字（socket）的描述符。
                // 第二项是一个字符数组或者缓冲区，用于存储从网络接收到的数据。
                // 第三项表示缓冲区的大小，即buffer能够容纳的最大数据量。
                // 最后一个参数，代表标志（flags）。在这个函数调用中，0 表示没有设置任何特殊标志。标志可以用于改变 recv 函数的行为
                if (count == 0)
                {
                    printf("disconnect\n");
                    FD_CLR(i,&rfds); 
                    close(i);
                    break;
                }
                send(i, buffer, count, 0);
                // 这个函数用于将数据从一个已连接的套接字发送出去
                // 第一项代表要发送数据的套接字描述
                // 第二项是字符缓冲区
                // 第三项表示要发送的数据的字节数
                // 第四项 flags参数。flags被设置为0，意味着使用默认的发送选项。
                printf("clientfd:%d,count:%d,buffer:;%s\n", i, count, buffer);
            }
        }
    }
#elif 0  //poll
    struct pollfd fds[1024] = {0};
    fds[sockfd].fd = sockfd; // 指定要监控的文件描述符
    fds[sockfd].events = POLLIN;
    // 表示要监控的事件类型，如 POLLIN（可读）、POLLOUT（可写）、POLLERR（错误）等。

    int maxfd =sockfd;
    while (1)
    {
        int nready = poll(fds, maxfd+1, -1);
        // 第一项表示要监控的文件描述符，第二项表示监视的文件描述符的数量，第三小表示超时时间
        if (fds[sockfd].revents & POLLIN) {
            // 意思是文件描述符可以进行读取操作，revent表示是当前监控的事件的类型

            struct sockaddr_in clientaddr;
            socklen_t len = sizeof(clientaddr);
            int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);

            printf("sockfd:%d\n", clientfd);
            fds[sockfd].fd = clientfd;
            fds[sockfd].events = POLLIN;

            maxfd =clientfd;
        }

        int i = 0;
        for (i = sockfd+1;i <= maxfd; i++) {
            if(fds[i].revents & POLLIN){
                char buffer[128] = {0};
                int count = recv(i, buffer, 128, 0);
                if (count == 0)
                {
                    printf("disconnect\n");
                    fds[i].fd = -1;
                    fds[i].events = 0;
                    close(i);
                    break;
                }
                send(i, buffer, count, 0);
 
                printf("clientfd:%d,count:%d,buffer:;%s\n", i, count, buffer);
            }
        }
    }
#else  //epoll
    int epfd = epoll_create(1); 
    // size 参数被忽略，但必须大于零, 返回一个非负整数作为 epoll 实例的文件描述符

    struct epoll_event ev; // 主要用于传递文件描述符及其相关的事件类型
    ev.events = EPOLLIN | EPOLLET;   //ET只接收一次
    ev.data.fd = sockfd; // data 是个联合体，可以用于存储用户自定义的数据，比如文件描述符或者指针等

    epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&ev);
    // 用于向 epoll 实例中添加、修改或删除需要监听的文件描述符
    // 第一项是指定epoll实例，第三项是要进行操作的文件描述符
    // 第二项指定要执行的操作，可以是以下值之一：
    // EPOLL_CTL_ADD：向 epfd 注册新的文件描述符。
    // EPOLL_CTL_MOD：修改已经注册的文件描述符的事件。
    // EPOLL_CTL_DEL：从 epfd 删除一个文件描述符。
    // 第四项指向 struct epoll_event 结构体的指针，其中包含要注册或修改的事件信息

    struct epoll_event events[1024] = {0};
    while (1)
    {
        int nready = epoll_wait(epfd, events, 1024, -1);
        // 用于等待文件描述符上的 I / O 事件发生
        // 第一项是监听的epoll实例，第二项是就绪事件指针，第三项是最大事件数量，第四项最大时间

        int i = 0;
        for ( i = 0; i < nready; i++)
        {
            int connfd =events[i].data.fd;
            if (sockfd == connfd) {
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);
                int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);

                ev.events = EPOLLIN;
                ev.data.fd = clientfd;

                epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);

                printf("clientfd:%d\n",clientfd);
            }else if (events[i].events & EPOLLIN){

                char buffer[128] = {0};
                int count = recv(connfd, buffer, 128, 0);
                if (count == 0)
                {
                    printf("disconnect\n");
                    
                    epoll_ctl(epfd,EPOLL_CTL_DEL,connfd,NULL);
                    close(i);
                    break;
                }
                send(connfd, buffer, count, 0);

                printf("clientfd:%d,count:%d,buffer:;%s\n", i, count, buffer);
            }
        }
        
    }
    
    

#endif


    getchar();

    return 0;
}
