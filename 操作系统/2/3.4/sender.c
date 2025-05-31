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
#define INPUT_SIZE 1024                       // 输入缓冲区最大长度
#define KEY_NUM    8848                       // 共享内存键值

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
    char input[INPUT_SIZE];       // 输入缓冲区
    int shmid;                    // 共享内存标识符
    char* shmptr;                 // 共享内存指针

    /*============== 共享内存初始化 ==============*/
    // 创建共享内存段
    shmid = shmget(key, INPUT_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("Receiver: Shmget Error");
        exit(EXIT_FAILURE);
    }

    // 附加共享内存到当前进程
    shmptr = shmat(shmid, NULL, 0);
    
    sem_t* mutex = sem_open(MUTEX_NAME, O_CREAT, 0666, 1);  // 互斥信号量
    sem_t* full = sem_open(FULL_NAME, O_CREAT, 0666, 0);    // 满信号量

    printf("请输入一串字符:");
    if(fgets(input, INPUT_SIZE, stdin) == NULL) {
            perror("fgets failed");
    }
    input[strcspn(input, "\n")] = '\0';

    /*============== 共享内存写入 ==============*/
    P(mutex);                // 获取共享内存访问权
    strcpy(shmptr, input);    // 写入数据到共享内存
    V(mutex);                // 释放共享内存访问权
    V(full);                 // 通知接收方有数据可用
    printf("消息已发送给receiver!\n");

    // 分离共享内存
    if (shmdt(shmptr) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
