#include <setjmp.h>
#include <stdio.h>


jmp_buf env;
// 用来保存程序的执行环境（包括寄存器的状态、程序计数器等信息），
// 以便在调用 longjmp 时，能够恢复到 setjmp 调用点的状态，从而实现跳转。

    void func(int arg) {

    printtf("func:%d\n", arg);
    longjmp(env, ++arg);
    // 用来跳转到之前调用 setjmp 的位置，并恢复程序的状态。
}

int main() {

    int ret = setjmp(env); // 用来保存当前的执行环境。
    if (ret == 0) {
        func(ret);
    } else if (ret == 1) {
        func(ret);
    } else if (ret == 2) {
        func(ret);
    } else if (ret == 3) {
        func(ret);
    }
}



