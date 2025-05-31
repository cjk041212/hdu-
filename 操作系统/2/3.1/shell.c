#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CMD_QUANTITY 4   //command数量
#define MAX_CMD_LEN 128  //command长度限制

//宏
#define INVALID_COMMAND -1
#define EXIT 0
#define CMD_1 1
#define CMD_2 2
#define CMD_3 3

//command name
char *cmdStr[CMD_QUANTITY] = {"exit", "cmd1", "cmd2", "cmd3"};

//get command
int getCmdIndex(char *cmd) 
{
    for(int i = 0; i < CMD_QUANTITY; i++) {
        if (strcmp(cmd, cmdStr[i]) == 0) {
            return i;
        }
    }
    return INVALID_COMMAND;
}

//fork
void myFork(int cmdIndex) 
{
    pid_t pid = fork();
    if(pid < 0) 
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } 
    else if (pid == 0) 
    {
        printf("子进程正在运行.\n");
        char *cmd = cmdStr[cmdIndex];
        char *path = NULL;
        
        // 构造完整路径
        if(asprintf(&path, "./%s", cmd) == -1) {
            perror("asprintf failed");
            exit(EXIT_FAILURE);
        }

        execl(path, cmd, NULL);
        
        // 失败
        perror("execl failed.");
        free(path);
        exit(EXIT_FAILURE);
    } 
    else 
    {
        wait(NULL);  // wait fork
    }
}

void runCMD(int cmdIndex) {
    switch(cmdIndex) {
        case INVALID_COMMAND:
            printf("Command Not Found\n");
            break;
        case EXIT:
            exit(EXIT_SUCCESS);
        default:
            myFork(cmdIndex);
            break;
    }
}

int main() {
    char input[MAX_CMD_LEN];
    
    while(1) {
        printf("\n输入命令\n>>: ");
        
        if(fgets(input, MAX_CMD_LEN, stdin) == NULL) {
            perror("fgets failed");
            continue;
        }
        
        input[strcspn(input, "\n")] = '\0';
        
        int cmdIndex = getCmdIndex(input);
        runCMD(cmdIndex);
    }
    
    return 0;
}
