#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAXSIZE 10   //队列个数

    typedef struct{
    int rear,head;
    int* queue; 
}Queue;


//初始化
// Queue* initsQueue(void) {
    
//     Queue* q = (Queue*)malloc(sizeof(Queue));
//     q->head = q->rear = 0;
//     q->queue = (int*)malloc(sizeof(int)*MAXSIZE);
// }


// 入队操作
void pushQueue(Queue *q, int n) {

    if(q->rear == q->head || q->rear == (q->head - 1 + MAXSIZE)%MAXSIZE) return;
    q->queue[q->rear] = n;
    q->rear ++; 
}


//出队操作
int popQueue(Queue *q, int n) {

    if(q->rear == q->head) return;
    
}



//清空操作
int front_Queue(){

}


//判空操作
bool empty_Queue(){

}


//输出操作



int main(int argc, char const *argv[]) {

    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->head = q->rear = 0;
    q->queue = (int *)malloc(sizeof(int) * MAXSIZE);

    int num1 = 100;
    pushQueue(q, num1);



    return 0;
}
