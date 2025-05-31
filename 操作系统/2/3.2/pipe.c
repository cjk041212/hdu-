#include <sys/sem.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/wait.h>

#define READ  0   
#define WRITE 1   
#define MAX_SIZE 16384  // 16KB管道测试尺寸

int main(int argc, char** argv) {
    int fd[2];                     // 管道文件描述符数组
    ssize_t status;                // 系统调用状态
    int flag = 0;                  // 循环控制标志
    char buf[MAX_SIZE], str[MAX_SIZE];  // 数据缓冲区
    pid_t pid1, pid2, pid3;        // 子进程PID
    
    // 信号量初始化
    sem_t *write_mutex = sem_open("pipe_test_wm", O_CREAT | O_RDWR, 0666, 0);
    sem_t *read_mutex1 = sem_open("pipe_test_rm_1", O_CREAT | O_RDWR, 0666, 0);
    sem_t *read_mutex2 = sem_open("pipe_test_rm_2", O_CREAT | O_RDWR, 0666, 0);

    // 创建管道
    status = pipe(fd);
    if (status < 0) {
        fprintf(stderr, "create Pipe Error %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    status = 0;
    
    /*======================== 子进程1：测试管道容量 ========================*/
    if ((pid1 = fork()) == -1) {
        printf("child process 1 fork() failed.\n");
        exit(1);
    }
    if (pid1 == 0) {
        printf("\n>> child process 1 [pid=%d, ppid=%d]:\n", getpid(), getppid());
        close(fd[READ]);  // 关闭读端
        
        // 设置非阻塞写模式
        int flags = fcntl(fd[WRITE], F_GETFL);
        fcntl(fd[WRITE], F_SETFL, flags | O_NONBLOCK);

        int count = 0;
        while (!flag) {
            status = write(fd[WRITE], buf, MAX_SIZE);
            if (status == -1) {
                flag = 1;
            } else {
                count++; //记录写入
                printf("child process 1 write %ldB.\n", status);
            }
        }
        printf("pipe size = %dKB.\n\n", (count * MAX_SIZE) / 1024);
        exit(0);
    }

    /*======================== 子进程2：发送消息 ========================*/
    if ((pid2 = fork()) == -1) {
        printf("child process 2 fork() failed.\n");
        exit(1);
    }
    if (pid2 == 0) {
        sem_wait(write_mutex);  // 获取写锁
        close(fd[READ]);
        
        printf(">> child process 2 [pid=%d, ppid=%d]:\n", getpid(), getppid());
        status = write(fd[WRITE], "This is child process 2.\n", strlen("This is child process 2.\n"));
        printf("child process 2 write %ldB.\n\n", status);
        
        sem_post(write_mutex);  // 释放写锁
        sem_post(read_mutex1);  // 通知读就绪
        exit(0);
    }

    /*======================== 子进程3：发送消息 ========================*/
    if ((pid3 = fork()) == -1) {
        printf("child process 3 fork() failed.\n");
        exit(1);
    }
    if (pid3 == 0) {
        sem_wait(write_mutex);  // 获取写锁
        close(fd[READ]);
        
        printf(">> child process 3 [pid=%d, ppid=%d]:\n", getpid(), getppid());
        status = write(fd[WRITE], "This is child process 3.\n", strlen("This is child process 3.\n"));
        printf("child process 3 write %ldB.\n\n", status);
        
        sem_post(write_mutex);  // 释放写锁
        sem_post(read_mutex2);  // 通知读就绪
        exit(0);
    }

    /*============================ 父进程处理 ============================*/
    wait(0);  // 等待所有子进程
    close(fd[WRITE]);  // 关闭写端
    
    // 设置非阻塞读模式
    printf(">> father process [pid=%d]:\n", getpid());
    int flags = fcntl(fd[READ], F_GETFL);
    fcntl(fd[READ], F_SETFL, flags | O_NONBLOCK);

    // 非阻塞读取测试
    while (!flag) {
        status = read(fd[READ], str, MAX_SIZE);
        if (status == -1) {
            flag = 1;
        } else {
            printf("father process read %ldB.\n", status);
        }
    }
    printf("\n");
    sem_post(write_mutex);
    // 等待最终数据
    sem_wait(read_mutex1);
    sem_wait(read_mutex2);
    
    // 最终读取
    printf(">> father process [pid=%d]:\n", getpid());
    status = read(fd[READ], str, MAX_SIZE);
    printf("father process read %ldB.\n", status);
    for (int i = 0; i < status; i++) {
        printf("%c", str[i]);
    }
    printf("\n");

    /*============================ 资源清理 ============================*/
    sem_close(write_mutex);
    sem_close(read_mutex1);
    sem_close(read_mutex2);
    sem_unlink("pipe_test_wm");
    sem_unlink("pipe_test_rm_1");
    sem_unlink("pipe_test_rm_2");

    return 0;
}
