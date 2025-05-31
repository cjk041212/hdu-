#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_MSG_LEN 100

// 消息缓冲区结构体
struct MsgBuf {
    long mtype;
    char mtext[MAX_MSG_LEN];
};

// 新增信号量用于控制执行顺序
sem_t sender1_start;
sem_t sender2_start;
int x1=1,x2=1;
// 发送者1线程函数
void *sender1(void *arg) {
    int res;
    struct MsgBuf msg;
    char buf[MAX_MSG_LEN];
    // 等待开始信号
    sem_wait(&sender1_start);
    // 打开有名信号量
    sem_t *mutex = sem_open("mutex", O_CREAT | O_RDWR, 0666, 0);
    sem_t *s1_done = sem_open("sender1_over", O_CREAT | O_RDWR, 0666, 0);
    sem_t *r1_done = sem_open("receive1_over", O_CREAT | O_RDWR, 0666, 0);
    if (mutex == SEM_FAILED || s1_done == SEM_FAILED || r1_done == SEM_FAILED) {
        perror("Semaphore open failed");
        exit(EXIT_FAILURE);
    }
    // 获取消息队列ID
    int msqid = msgget((key_t)8088, 0666 | IPC_CREAT);
    if (msqid == -1) {
        perror("Message queue creation failed");
        exit(EXIT_FAILURE);
    }
    while (1) {
        sem_wait(mutex);
        printf("sender1线程等待用户输入字符:");
        if (scanf("%s", buf) != 1) {
            perror("Input reading failed");
            sem_post(mutex);
            continue;
        }
        msg.mtype = 1;
        if (strcmp(buf, "exit") == 0) {
            x1=0;
            strcpy(msg.mtext, "end1");
            res = msgsnd(msqid, (void *)&msg, MAX_MSG_LEN, 0);
            if (res == -1) {
                perror("Message send failed");
                sem_post(mutex);
                continue;
            }
            sem_wait(r1_done);
            res = msgrcv(msqid, (void *)&msg, MAX_MSG_LEN, 2, 0);
            if (res == -1) {
                perror("Message receive failed");
                sem_post(mutex);
                continue;
            }
            printf("%s\n", msg.mtext);
            sem_post(s1_done);
            sem_post(mutex);
            // 允许 sender2 开始
            sem_post(&sender2_start);
            sleep(1);
            return NULL;
        } else if (strcmp(buf, "goto_sender2") == 0) {
            if(!x2){
                perror("Message send failed");
                continue;
            }
            // 允许 sender2 开始
            sem_post(&sender2_start);
            sem_post(mutex);
            // 等待 sender2 让出控制权
            sem_wait(&sender1_start);
            continue;
        } else {
            strcpy(msg.mtext, buf);
            res = msgsnd(msqid, (void *)&msg, MAX_MSG_LEN, 0);
            if (res == -1) {
                perror("Message send failed");
            }
            sem_post(mutex);
            sleep(1);
        }
    }
}

// 发送者2线程函数
void *sender2(void *arg) {
    int res;
    struct MsgBuf msg;
    char buf[MAX_MSG_LEN];
    // 等待开始信号
    sem_wait(&sender2_start);
    sem_t *mutex = sem_open("mutex", O_CREAT | O_RDWR, 0666, 0);
    sem_t *s2_done = sem_open("sender2_over", O_CREAT | O_RDWR, 0666, 0);
    sem_t *r2_done = sem_open("receive2_over", O_CREAT | O_RDWR, 0666, 0);
    if (mutex == SEM_FAILED || s2_done == SEM_FAILED || r2_done == SEM_FAILED) {
        perror("Semaphore open failed");
        exit(EXIT_FAILURE);
    }
    int msqid = msgget((key_t)8088, 0666 | IPC_CREAT);
    if (msqid == -1) {
        perror("Message queue creation failed");
        exit(EXIT_FAILURE);
    }
    while (1) {
        sem_wait(mutex);
        printf("sender2线程等待用户输入字符:");
        if (scanf("%s", buf) != 1) {
            perror("Input reading failed");
            sem_post(mutex);
            continue;
        }
        msg.mtype = 1;
        if (strcmp(buf, "exit") == 0) {
            x2=0;
            strcpy(msg.mtext, "end2");
            res = msgsnd(msqid, (void *)&msg, MAX_MSG_LEN, 0);
            if (res == -1) {
                perror("Message send failed");
                sem_post(mutex);
                continue;
            }
            sem_wait(r2_done);
            res = msgrcv(msqid, (void *)&msg, MAX_MSG_LEN, 3, 0);
            if (res == -1) {
                perror("Message receive failed");
                sem_post(mutex);
                continue;
            }
            printf("%s\n", msg.mtext);
            sem_post(s2_done);
            sem_post(mutex);
            // 允许 sender1 继续
            sem_post(&sender1_start);
            sleep(1);
            return NULL;
        } else if (strcmp(buf, "goto_sender1") == 0) {
            if(!x1){
                perror("Message send failed");
                continue;
            }
            // 允许 sender1 开始
            sem_post(&sender1_start);
            sem_post(mutex);
            // 等待 sender1 让出控制权
            sem_wait(&sender2_start);
            continue;
        } else {
            strcpy(msg.mtext, buf);
            res = msgsnd(msqid, (void *)&msg, MAX_MSG_LEN, 0);
            if (res == -1) {
                perror("Message send failed");
            }
            sem_post(mutex);
            sleep(1);
        }
    }
}

// 接收者线程函数
void *receiver(void *arg) {
    int res;
    int s1_fin = 0;
    int s2_fin = 0;
    struct MsgBuf msg;
    sem_t *s1_done = sem_open("sender1_over", O_CREAT | O_RDWR, 0666, 0);
    sem_t *r1_done = sem_open("receive1_over", O_CREAT | O_RDWR, 0666, 0);
    sem_t *s2_done = sem_open("sender2_over", O_CREAT | O_RDWR, 0666, 0);
    sem_t *r2_done = sem_open("receive2_over", O_CREAT | O_RDWR, 0666, 0);
    if (s1_done == SEM_FAILED || r1_done == SEM_FAILED || s2_done == SEM_FAILED || r2_done == SEM_FAILED) {
        perror("Semaphore open failed");
        exit(EXIT_FAILURE);
    }
    int msqid = msgget((key_t)8088, 0666 | IPC_CREAT);
    if (msqid == -1) {
        perror("Message queue creation failed");
        exit(EXIT_FAILURE);
    }
    while (1) {
        res = msgrcv(msqid, (void *)&msg, MAX_MSG_LEN, 0, 0);
        if (res == -1) {
            perror("Message receive failed");
            continue;
        }
        printf("\nReceiver线程收到内容“%s”\n", msg.mtext);
        if (strcmp(msg.mtext, "end1") == 0) {
            msg.mtype = 2;
            strcpy(msg.mtext, "over1");
            res = msgsnd(msqid, (void *)&msg, MAX_MSG_LEN, 0);
            if (res == -1) {
                perror("Message send failed");
                continue;
            }
            sem_post(r1_done);
            sem_wait(s1_done);
            s1_fin = 1;
        } else if (strcmp(msg.mtext, "end2") == 0) {
            msg.mtype = 3;
            strcpy(msg.mtext, "over2");
            res = msgsnd(msqid, (void *)&msg, MAX_MSG_LEN, 0);
            if (res == -1) {
                perror("Message send failed");
                continue;
            }
            sem_post(r2_done);
            sem_wait(s2_done);
            s2_fin = 1;
        }
        if (s1_fin && s2_fin) {
            if (msgctl(msqid, IPC_RMID, 0) == -1) {
                perror("Message queue removal failed");
            }
            exit(EXIT_SUCCESS);
        }
    }
}

int main() {
    int msqid = msgget((key_t)8088, 0666 | IPC_CREAT);
    if (msqid != -1) {
        if (msgctl(msqid, IPC_RMID, 0) == -1) {
            perror("Message queue removal failed");
        }
    }
    sem_unlink("mutex");
    sem_unlink("sender1_over");
    sem_unlink("sender2_over");
    sem_unlink("receive1_over");
    sem_unlink("receive2_over");

    sem_t *mutex = sem_open("mutex", O_CREAT | O_RDWR, 0666, 0);
    sem_t *s1_done = sem_open("sender1_over", O_CREAT | O_RDWR, 0666, 0);
    sem_t *r1_done = sem_open("receive1_over", O_CREAT | O_RDWR, 0666, 0);
    sem_t *s2_done = sem_open("sender2_over", O_CREAT | O_RDWR, 0666, 0);
    sem_t *r2_done = sem_open("receive2_over", O_CREAT | O_RDWR, 0666, 0);
    if (mutex == SEM_FAILED || s1_done == SEM_FAILED || r1_done == SEM_FAILED || s2_done == SEM_FAILED || r2_done == SEM_FAILED) {
        perror("Semaphore open failed");
        exit(EXIT_FAILURE);
    }

    // 初始化新增信号量
    sem_init(&sender1_start, 0, 1);
    sem_init(&sender2_start, 0, 0);

    pthread_t s1, s2, r;
    if (pthread_create(&s1, NULL, sender1, NULL) != 0) {
        perror("Sender1 thread creation failed");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&s2, NULL, sender2, NULL) != 0) {
        perror("Sender2 thread creation failed");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&r, NULL, receiver, NULL) != 0) {
        perror("Receiver thread creation failed");
        exit(EXIT_FAILURE);
    }
    sem_post(mutex);
    if (pthread_join(s1, NULL) != 0) {
        perror("Sender1 thread join failed");
    }
    if (pthread_join(s2, NULL) != 0) {
        perror("Sender2 thread join failed");
    }
    if (pthread_join(r, NULL) != 0) {
        perror("Receiver thread join failed");
    }
    // 销毁新增信号量
    sem_destroy(&sender1_start);
    sem_destroy(&sender2_start);
    return 0;
}    
