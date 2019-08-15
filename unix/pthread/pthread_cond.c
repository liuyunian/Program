/**
 * 条件变量的用法示例
 * 示例中没有对函数调用进行异常处理
 */
#include <stdio.h>
#include <string.h> // memset
#include <unistd.h> // usleep
#include <stdlib.h> // malloc
#include <pthread.h>

struct msg{
    char * msgInfo;
    struct msg * next;
};

struct msg * msgQue = NULL; // 消息队列

pthread_mutex_t queLock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t queCond = PTHREAD_COND_INITIALIZER;

// 处理消息的线程入口函数
void * process_msg(void * arg){
    struct msg * p_msg = NULL;

    for(;;){
        pthread_mutex_lock(&queLock); 
        while(msgQue == NULL){
            pthread_cond_wait(&queCond, &queLock);
        }

        p_msg = msgQue;
        msgQue = p_msg->next;
        pthread_mutex_unlock(&queLock);

        printf("处理消息：%s\n", p_msg->msgInfo);
        
        free(p_msg->msgInfo);
        free(p_msg);
        p_msg = NULL;
    }


    return (void *)0;
}

void enqueue_msg(char * msgInfo){
    struct msg * p_msg = (struct msg *)malloc(sizeof(struct msg));
    p_msg->msgInfo = msgInfo;

    pthread_mutex_lock(&queLock);
    p_msg->next = msgQue;
    msgQue = p_msg;
    pthread_mutex_unlock(&queLock);

    pthread_cond_signal(&queCond);
}

int main(){
    pthread_t tid;
    pthread_create(&tid, NULL, process_msg, NULL);

    for(int i = 0; i < 100; ++ i){
        char * msgBuf = (char *)malloc(20);
        memset(msgBuf, 0, 20);
        sprintf(msgBuf, "message %d", i);

        printf("收到消息：%s\n", msgBuf);
        enqueue_msg(msgBuf);
        // usleep(1000); // 1ms
    }

    pthread_join(tid, NULL);

    return 0;
}
