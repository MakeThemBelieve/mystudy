#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>



#include "spinlock.h"
#include "thrd_pool.h"

/*
编译动态库：gcc thrd_pool.c -o libthrdpool.so -I./ -lpthread -fPIC -shared
可以通过提供头文件thrd_pool.h和动态库libthrdpool.so，让别人来使用该线程池
*/

typedef struct spinlock spinlock_t;



typedef struct taskex_s {
    
    void *pnext;
    handler_pt func;
    void *arg;
} taskex_t;



//任务封装
typedef struct task_s {

    void *next;          //为了链接下一项（任务是队列）
    handler_pt func;
    void *arg;
} task_t;



//任务队列
typedef struct task_queue_s {

    void *head;
    void **tail;
    int block;            //阻塞标识
    spinlock_t lock;
    pthread_mutex_t mutex;
    pthread_cond_t cond;   //根据条件变量阻塞线程
    // 因为是条件判断来进行加锁，判断的是队列的状态（非任务状态，而是具体的读取条件）,所以在队列中进行判断加锁
} task_queue_t;



//线程池构成
struct thrdpool_s {

    task_queue_t *task_queue;     //任务队列   ->存储任务状态
    atomic_int quit;              // 原子变量   ->标记退出
    int thrd_count;
    pthread_t *threads;           // 存储多个线程的标识符PID
};



//创建队列
static task_queue_t *     //因为为内部（仅.c文件中不暴露给用户）使用，所以用静态声明
__taskqueue_create() {

    int ret;
    task_queue_t *queue = (task_queue_t*)malloc(sizeof(task_queue_t));
    if (queue) {
        ret = pthread_mutex_init(&queue->mutex, NULL);  //初始化互斥锁为可用
        if (ret == 0)
        {
            ret = pthread_cond_init(&queue->cond, NULL);
            if (ret == 0 ) {
                spinlock_init(&queue->lock);
                queue->head = NULL;
                queue->tail = &queue->head;
                queue->block = 1;
                return queue;
            }
            pthread_cond_destroy(&queue->cond);
        }
        
        free(queue);
    }

    // 若初始化失败
    pthread_mutex_destroy(&queue->mutex);
    return NULL;
}



static void
__noblock(task_queue_t *queue) {

    pthread_mutex_lock(&queue->mutex);
    queue->block = 0;
    pthread_mutex_unlock(&queue->mutex);
    pthread_cond_broadcast(&queue->cond);
}

// 队列插入新的任务task
static inline void
__add_task(task_queue_t *queue, task_t *task){

    //不限定任务类型，只要任务的结构起始内存是一个用于链接下一个节点的指针
    void **link = (void**)task;
    *link = NULL;

    spinlock_lock(&queue->lock);
    *queue->tail = link; //等价queue->tail->next
    //因为指针固定都是8个字节，二维指针指向一个结构体时候，所以只会指向第一项的内容（一项8个字节就是指向前8个字节）
    //一维指针会指向整个结构体
    queue->tail = link;
    spinlock_unlock(&queue->lock);
    pthread_cond_signal(&queue->cond);
}


//队列中删除数据
static inline void *
__pop_task(task_queue_t *queue){

    spinlock_lock(&queue->lock);
    if(queue->head == NULL) {
        spinlock_unlock(&queue->lock);
        return NULL;
    }
    task_t *task;
    task = queue->head;


    void **link = (void**)task;
    queue->head = *link;    // 等效queue->head = task->next;

    if (queue->head == NULL) {
        queue->tail = &queue->head;
    }
    spinlock_unlock(&queue->lock);
    return task;
}


// 消费者线程取出任务
static inline void *
__get_task(task_queue_t *queue){
    task_t *task;
    while ((task = __pop_task(queue)) == NULL) { // 虚假唤醒-多线程并发有可能出现这种问题
        pthread_mutex_lock(&queue->mutex);
        if (queue->block == 0) {
            pthread_mutex_unlock(&queue->mutex);
            return NULL;
        }

        pthread_cond_wait(&queue->cond, &queue->mutex); 
        // 1.先unlock ,2.休眠，3.__add_task时候在cond唤醒，4.加上lock
        pthread_mutex_unlock(&queue->mutex);
    }
}


//对称-销毁队列
static void
__taskqueue_destory(task_queue_t *queue) {

    task_t *task;
    while ((task = __pop_task(queue))) {
        free(task);
    }
    spinlock_destroy(&queue->lock);
    pthread_cond_destroy(&queue->cond);
    pthread_mutex_destroy(&queue->mutex);
    free(queue);
}


// 线程入口函数-取出任务，执行任务
static void *
__thrdpool_worker(void *arg) {
    
    thrdpool_t *pool = (thrdpool_t*)arg;
    task_t *task;
    void *ctx;

    while (atomic_load(&pool->quit) == 0) {
        task = (task_t*)__get_task(pool->task_queue);
        if (!task) break;
        handler_pt func = task->func;
        ctx = task->arg;
        free(task);
        func(ctx);
    }

    return NULL;
}

// 停止
static void __threads_terminate(thrdpool_t *pool)
{
    atomic_store(&pool->quit, 1);
    /*默认情况下，线程池中的线程在执行任务时可能会阻塞等待任务的到来。
    如果在线程池退出时，仍有线程处于等待任务的阻塞状态，那么这些线程将无法在有新任务时重新启动并执行。
    调用 __nonblock 函数可以将任务队列设置为非阻塞状态，确保所有线程都能正确地退出，
    而不会被阻塞在等待任务的状态中。*/
    __noblock(pool->task_queue);
    int i;
    for (i = 0; i < pool->thrd_count; i++)
    {
        pthread_join(pool->threads[i], NULL); // 阻塞调用它的线程，直到指定的线程结束执行。
    }
}

// 创建线程
static int __threads_create(thrdpool_t *pool, size_t thrd_count) {

    pthread_attr_t attr;
    int ret;

    ret = pthread_attr_init(&attr);

    if (ret == 0) {
        pool->threads = (pthread_t*)malloc(sizeof(pthread_t) * thrd_count);
        if (pool->threads) {
            int i = 0;
            for (;i < thrd_count; i++) {
                if (pthread_create(&pool->threads[i], &attr, __thrdpool_worker, pool) != 0)
                    break;
                }
        
            pool->thrd_count = i;
            pthread_attr_destroy(&attr);
            if (i == thrd_count)
                return 0;
            __threads_terminate(pool);
            free(pool->threads);
        }
            ret = -1;
    }
    return ret;
}

// 停止线程-------用户调用的接口
void thrdpool_terminate(thrdpool_t *pool)
{
    atomic_store(&pool->quit, 1);
    __noblock(pool->task_queue);
}



// 创建线程-------用户调用的接口
thrdpool_t *
thrdpool_create(int thrd_count)
{
    thrdpool_t *pool;
    pool = (thrdpool_t *)malloc(sizeof(thrdpool_t));
    if (!pool)
        return NULL;

    // 创建任务队列
    task_queue_t *queue = __taskqueue_create();
    if (queue)
    {
        pool->task_queue = queue;
        atomic_init(&pool->quit, 0);
        if (__threads_create(pool, thrd_count) == 0)
        {
            return pool;
        }
        __taskqueue_destory(pool->task_queue);
    }
    free(pool);
    return NULL;
}

// 生产者抛出任务到线程池(加到任务队列)
int thrdpool_post(thrdpool_t *pool, handler_pt func, void *arg)
{
    if (atomic_load(&pool->quit) == 1)
    {
        return -1;
    }
    task_t *task = (task_t *)malloc(sizeof(task_t));
    if (!task)
        return -1;
    task->func = func;
    task->arg = arg;

    __add_task(pool->task_queue, task);
    return 0;
}

// 等待线程池中的所有线程完成任务，并清理线程池的资源
void thrdpool_waitdone(thrdpool_t *pool)
{
    int i;
    for (i = 0; i < pool->thrd_count; i++)
    {
        pthread_join(pool->threads[i], NULL);
    }
    __taskqueue_destory(pool->task_queue);
    free(pool->threads);
    free(pool);
}
