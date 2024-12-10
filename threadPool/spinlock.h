
//ai版



#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdatomic.h>

// 定义自旋锁类型
typedef struct spinlock
{
    atomic_flag flag; // 用于实现自旋锁的标志位
} spinlock_t;

// 初始化自旋锁
static inline void spinlock_init(spinlock_t *lock)
{
    atomic_flag_clear(&lock->flag); // 初始化自旋锁，设置为未锁定状态
}

// 加锁操作
static inline void spinlock_lock(spinlock_t *lock)
{
    // 自旋，直到获得锁
    while (atomic_flag_test_and_set(&lock->flag))
    {
        // 这里的循环就是自旋，等待锁释放
        // atomic_flag_test_and_set() 会先读取锁的状态，如果锁已经被设置为1，则返回1，并将其设置为1
        // 如果没有被设置为1，它将返回0，并设置为1，即获取锁
    }
}

// 解锁操作
static inline void spinlock_unlock(spinlock_t *lock)
{
    // 解锁操作，通过清除标志位来释放锁
    atomic_flag_clear(&lock->flag);
}

// 销毁自旋锁（清理资源，虽然在很多实现中这不是必需的，但可以提供这个函数以便一致性）
static inline void spinlock_destroy(spinlock_t *lock)
{
    // 原子操作，标志位的清理通常不需要做什么操作
    // 这里只是为了接口的一致性
}

#endif // SPINLOCK_H
