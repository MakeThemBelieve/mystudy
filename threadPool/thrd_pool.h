#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H


typedef struct thrdpool_s thrdpool_t;
//任务执行规范 ctx 上下文
typedef void (*handler_pt)(void * /* ctx */);
//(*handler_pt)：表示这是一个函数指针，指向一个返回类型为 void，参数类型为 void* 的函数。

#ifdef __cplusplus
extern "C"
{
#endif

//对称处理
thrdpool_t *thrdpool_create(int thrd_count);

void thrdpool_terminate(thrdpool_t *pool);

int thrdpool_post(thrdpool_t *pool, handler_pt func, void *arg);

//测试用接口
void thrdpool_waitdone(thrdpool_t *pool);

#ifdef __cplusplus
}
#endif


#endif