#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static const char *MUTEX_NAME = "mutex_shm";  // 互斥信号量名称
static const char *FULL_NAME  = "full_shm";   // 满信号量名称
#define RESULT_SIZE 1024                      // 结果缓冲区大小
#define KEY_NUM     8848                      // 共享内存键值

/*------------------------ 辅助函数 ------------------------*/
// 信号量P操作(申请资源)
void P(sem_t *semPtr) {
    sem_wait(semPtr);
}

// 信号量V操作(释放资源)
void V(sem_t *semPtr) {
    sem_post(semPtr);
}

int main(int argc, char** argv) {
    key_t key = KEY_NUM;          // 共享内存标识键
    char result[RESULT_SIZE];     // 接收缓冲区
    int shmid;                    // 共享内存标识符
    char* shmptr;                 // 共享内存指针

    /*============== 共享内存初始化 ==============*/
    // 创建/获取共享内存段
    shmid = shmget(key, RESULT_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("Receiver: Shmget Error");
        exit(EXIT_FAILURE);
    }

    // 附加共享内存到当前进程
    shmptr = shmat(shmid, NULL, 0);

    /*============== 信号量初始化 ==============*/
    sem_t* mutex = sem_open(MUTEX_NAME, O_CREAT, 0666, 1);  // 互斥信号量
    sem_t* full = sem_open(FULL_NAME, O_CREAT, 0666, 0);    // 满信号量

    /*============== 消息接收处理 ==============*/
    P(full);     // 等待数据可用
    P(mutex);    // 获取共享内存访问权
    strcpy(result, shmptr);
    V(mutex);    // 释放共享内存访问权
    
    printf("接收到来自sender的消息是: %s\n", result);
    printf("over\n");

    /*============== 资源清理 ==============*/
    // 分离共享内存
    if (shmdt(shmptr) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

    // 删除共享内存段
    shmctl(shmid, IPC_RMID, NULL);

    // 清理信号量资源
    sem_t *mutexPtr = sem_open(MUTEX_NAME, O_CREAT);
    sem_t *fullPtr = sem_open(FULL_NAME, O_CREAT);
    sem_close(mutexPtr);
    sem_unlink(MUTEX_NAME);
    sem_close(fullPtr);
    sem_unlink(FULL_NAME);

    return 0;
}
