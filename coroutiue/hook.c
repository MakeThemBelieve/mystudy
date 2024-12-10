
#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>

// hook
//提供了两个接口read,write，每次read和write能不使用系统本身的系统接口，而采用自己提供的接口

#if 1
typedef ssize_t (*read_t)(int fd, void *buf, size_t count);
read_t read_f = NULL;


typedef ssize_t (*write_t)(int fd, const void *buf, size_t count);
write_t write_f = NULL;



ssize_t read(int fd, void *buf, size_t count) {


    struct pollfd fds[1]= {0};
    fds[0].fd = fd;
    fds[0].events = POLLIN;

    int res = poll(fds, 1, 0);
    if(res <= 0) {  //不可读
        
        swapcontext();       //切换，等待   ctx-->fd一对一
    }
    
    ssize_t ret = read_f(fd, buf, count);
    printf("writf:%s\n", (char *)buf);
    return ret;
}



ssize_t write(int fd, const void *buf, size_t count){

    printf("write:%s\n", (const char *)buf);
    return write_f(fd, buf, count);
}



void init_hook(void)
{

    if (!read_f) {

        read_f = dlsym(RTLD_NEXT, "read");
        // dlsym：是一个动态链接库函数，用来查找在共享库中定义的符号（函数、变量等）,第一项是指向共享库句柄，第二项是要查找的符号
        // RTLD_NEXT 表示查找下一个已加载的共享库中的符号。
    }

    if (!write_f) {

        write_f = dlsym(RTLD_NEXT, "write");
    }
}

#endif


int main(int argc, char const *argv[])
{
    init_hook();

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(struct sockaddr_in));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(2048);

    if (-1 == bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr))) {
        perror("bind");
        return -1;
    }

    listen(sockfd, 10);

    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);
    int clientfd  = accept(sockfd, (struct sockaddr*)&clientaddr, &len);
    printf("accept\n");

    while (1) {
        char buffer[128] = {0};
        int count = read(clientfd, buffer, 128);
        if (count == 0) break;
        write(clientfd, buffer, count);
        printf("sockfd: %d, clientfd: %d, count: %d,buffer:%s\n",sockfd, clientfd, count,buffer);
    }

    int fd = open("a.txt", O_CREAT | O_RDWR);
    if (fd < 0)
    {
        return -1;
    }

    char *str = "1234567890";
    int ret = write(fd, str, strlen(str));
    printf("ret:%d\n", ret);

    close(fd);

    fd = open("a.txt", O_CREAT | O_RDWR);
    if (fd < 0) return -1;

    


    return 0;
}

