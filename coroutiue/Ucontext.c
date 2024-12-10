
#include <ucontext.h>
#include <stdio.h>


ucontext_t ctx[2]; // 用于表示和保存线程或进程的执行上下文
ucontext_t main_ctx;

//struct ucontext ctx[2];

int count = 0;




// coroutine
void func1() {

    while (count++ < 20) {
        printf("1\n");
        swapcontext(&ctx[0], &ctx[1]); // 保存当前上下文到第一项指针中，然后切换到第二项指针所表示的上下文。
        printf("3\n");
    }
}

void func2() {

    while (count++ < 20) {
        printf("2\n");
        swapcontext(&ctx[1], &ctx[0]);
        printf("4\n");
    }
}

//调度器 
int main() {

    


#if 1
    char stack1[2048] = {0};
    char stack2[2048] = {0};

    getcontext(&ctx[0]); // 获取当前执行环境的上下文，并将其保存到指针中。
    ctx[0].uc_stack.ss_sp = stack1;
    ctx[0].uc_stack.ss_size = sizeof(stack1);
    ctx[0].uc_link = &main_ctx;
    makecontext(&ctx[0], func1, 0);
    // 用于设置 ucontext_t 结构体，使得当调用 setcontext 或 swapcontext 时
    // 程序会跳转到第二项函数的开始处，并传入指定的参数

    getcontext(&ctx[1]);
    ctx[1].uc_stack.ss_sp = stack2;
    ctx[1].uc_stack.ss_size = sizeof(stack2);
    ctx[1].uc_link = &main_ctx;
    makecontext(&ctx[1], func2, 0);

    printf("swapcontext\n");
    swapcontext(&main_ctx, &ctx[0]);

    printf("\n");

#endif



}