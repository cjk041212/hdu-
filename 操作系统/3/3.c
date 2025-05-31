#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#define END             0xffff  //fat表项占用
#define FREE            0x0000  //fat表项未占用
#define MAX_OPENFILE    10  //最多打开文件表项数
#define PATH_MAX_LENGTH 100  //路径最大长度
#define NAME_MAX_LENGTH 15  //文件名最大长度
#define PATH            "./sys"  //文件系统写入文件.sys中
#define MODIFIED '1'     //打开文件表项被修改
#define UNMODEFIED '0'  //打开文件表项未修改
#define ASSIGNED '1'   //打开文件表项被占用
#define UNASSIGNED '0' //打开文件表项被释放
#define DICTORY '0'  //fcb为文件夹
#define DATA '1'   //fcb为文件
#define EMPTY '0'  //文件、文件夹为空
#define UNEMPTY '1'//文件、文件夹不为空
typedef struct FCB {
    char filename[8];//文件名
    char exname[4];//扩展名
    unsigned char attribute;//标识位 DATA('1')为数据文件 DICTORY('0')为文件夹
    struct tm time_info;//创建时间
    unsigned short first;//开始块
    unsigned long length;//占用大小(以BLOCK_SIZE递增)
    char free;//标识位 EMPTY('0')表示文件为空 UNEMPTY('1')表示文件不为空
    int len;//文件实际长度
} fcb;

typedef struct FAT {
    unsigned short id;
} fat;

typedef struct USEROPEN {
    fcb open_fcb;//打开的文件项目的信息
    char dir[80];//绝对路径
    int count;//读写指针
    char fcb_state;//标识是否被修改 MODIFIED('1')表示被修改 UNMODIFIED('0')表示未被修改
    char topenfile;//标识是否被占用 ASSIGNED('1')表示被占用 UNASSIGNED('0')表示未被占用
} useropen;

typedef struct BLOCK0 {
    int BLOCK_SIZE;//单个磁盘块大小
    int BLOCK_NUM;//磁盘块个数
    int SIZE;//文件系统总大小
    int FAT_NUM;//储存fat表需要的磁盘块数
    unsigned short root_start;//根目录开始块数
    unsigned char *start_block;//数据区开始指针
} block0;

unsigned char *myvhard;//文件系统开始位置指针
useropen openfile_list[MAX_OPENFILE];//打开文件项目表
int curdir;//当前打开文件项目号
char current_dir[80];//当前路径
unsigned char *startp;//数据区开始指针
int BLOCK_SIZE;//单个磁盘块大小
int BLOCK_NUM;//磁盘块个数
int SIZE;//文件系统总大小
int FAT_NUM;//储存一张fat表需要的磁盘块数
int ROOT_START;//根目录开始块数

void openfile_set(int setdir,fcb *open_fcb, char dir[],int count, char fcbstate, char topenfile);
void fcb_set(fcb *f, const char *filename, const char *exname, unsigned char attribute, unsigned short first,
             unsigned long length, char ffree, int len);
void fcb_copy(fcb *dest, fcb *src);
int do_read(int des_fd,int len,char *text);
void format();
void fat_format();
int get_free();
int fat_allocate(unsigned short first, unsigned short length);
//取得实际地址
void *get_fact_path(char *fact_path, const char *path);
fcb *find_fcb_r(char *token, int start,int *cnt);
fcb *find_fcb(const char *path, int *cnt);
void init_folder(int first, int second);
void reclaim_space(int first);
void reclaim_space_fat(int cnt);
void do_write(int des_fd,char *str, int len,int mode);
void startsys() {
    FILE *fp;
    int i;
    if ((fp = fopen(PATH, "r")) != NULL) {
        //首先读取引导块，确定磁盘块大小
        block0 *init_block = (block0 *) malloc(sizeof(block0));
        memset(init_block, 0, sizeof(block0));
        fread(init_block, sizeof(block0), 1, fp);
        BLOCK_SIZE = init_block->BLOCK_SIZE;
        SIZE = init_block->SIZE;
        BLOCK_NUM = init_block->BLOCK_NUM;
        FAT_NUM = init_block->FAT_NUM;
        ROOT_START = init_block->root_start;
        fclose(fp);
        fp = fopen(PATH, "r");
        myvhard = (unsigned char *) malloc(SIZE);
        memset(myvhard, 0, SIZE);
        fread(myvhard, SIZE, 1, fp);
        fclose(fp);
    } else {
        printf("文件系统未安装将调用format函数安装文件系统\n");
        format();
    }
    //初始化用户打开表
    //第一项初始化为根目录
    openfile_set(0,((fcb *) (myvhard + (1+FAT_NUM*2) * BLOCK_SIZE)),"\\",0,UNMODEFIED,ASSIGNED);
    curdir = 0;//当前目录
    //其他项置为空
    fcb *empty = (fcb *) malloc(sizeof(fcb));//生成一个空的FCB
    fcb_set(empty, "\0", "\0", DICTORY, 0, 0, EMPTY,0);
    for (i = 1; i < MAX_OPENFILE; i++) {
        openfile_set(i,empty,"\0",0,UNMODEFIED,UNASSIGNED);
    }
    //设置当前目录
    strcpy(current_dir, openfile_list[curdir].dir);
    startp = ((block0 *) myvhard)->start_block;
    free(empty);
}
void format() {
    int i;
    int first;
    FILE *fp;
    block0 *init_block = (block0 *) malloc(sizeof(block0));
    unsigned char *ptr;
    printf("请输入每块磁盘块大小\n");
    scanf("%d",&init_block->BLOCK_SIZE);
    printf("请输入磁盘块个数\n");
    scanf("%d",&init_block->BLOCK_NUM);
    getchar();
    init_block->SIZE = init_block->BLOCK_NUM * init_block->BLOCK_SIZE;
    SIZE = init_block->SIZE;
    BLOCK_SIZE = init_block->BLOCK_SIZE;
    BLOCK_NUM = init_block->BLOCK_NUM;
    init_block->FAT_NUM = (int) (((init_block->BLOCK_NUM)* sizeof(fat))/init_block->BLOCK_SIZE)+1;
    FAT_NUM = init_block->FAT_NUM;
    init_block->root_start = 1+2*FAT_NUM;
    ROOT_START = init_block->root_start;
    init_block = realloc(init_block,init_block->SIZE);
    ptr = (unsigned char *) init_block;
    myvhard = (unsigned char *) init_block;
    init_block->start_block = (unsigned char *) (init_block + BLOCK_SIZE * (FAT_NUM * 2 + 1 +2));
    ptr = ptr + BLOCK_SIZE;
    //初始化FAT
    fat_format();
    //分配空间给引导块和FAT1 FAT2块
    //第一次get_free未检查
    fat_allocate(get_free(),1);
    fat_allocate(get_free(), FAT_NUM);
    fat_allocate(get_free(), FAT_NUM);
    ptr += BLOCK_SIZE * 2*FAT_NUM;
    //分配空间给根目录
    fcb *root = (fcb *) ptr;
    first = get_free();
    if (first == -1){
        fprintf(stderr, "系统已无空闲磁盘块\nformat失败\n");
        exit(-1);
    }
    if(fat_allocate(first, 2) == -1){
        fprintf(stderr, "系统已无空闲磁盘块\nformat失败\n");
        exit(-1);
    }
    fcb_set(root, ".", "dic", DICTORY, first, BLOCK_SIZE , UNEMPTY,0);
    root++;
    fcb_set(root, "..", "dic", DICTORY, first, BLOCK_SIZE, UNEMPTY,0);
    root++;
    // 初始化根目录区剩余的表项
    for (i = 2; i < BLOCK_SIZE / sizeof(fcb); i++, root++) {
        root->free = EMPTY;
    }
    root = (fcb *)(myvhard + BLOCK_SIZE *(2+2*FAT_NUM));
    for (i = 0; i < BLOCK_SIZE / sizeof(fcb); i++, root++) {
        root->free = EMPTY;
    }
    //写入文件
    fp = fopen(PATH, "w");
    fwrite(myvhard, SIZE, 1, fp);
    fclose(fp);
}

//fcb设置
void fcb_set(fcb *f, const char *filename, const char *exname, unsigned char attribute, unsigned short first,
             unsigned long length, char ffree, int len) {
    time_t *now = (time_t *) malloc(sizeof(time_t));
    struct tm *time_info;
    time(now);
    time_info = localtime(now);
    memset(f->filename, 0, 8);
    memset(f->exname, 0, 4);
    strncpy(f->filename, filename, 7);
    strncpy(f->exname, exname, 3);
    f->attribute = attribute;
    memcpy(&f->time_info,time_info, sizeof(struct tm));
    f->first = first;
    f->length = length;
    f->free = ffree;
    f->len = len;
    free(now);
}
//打开项设置
void openfile_set(int setdir,fcb *open_fcb, char dir[],int count, char fcbstate, char topenfile){
    fcb_copy(&openfile_list[setdir].open_fcb, open_fcb);
    strcpy(openfile_list[setdir].dir, dir);
    openfile_list[setdir].count = count;//读写指针为0
    openfile_list[setdir].fcb_state = fcbstate;
    openfile_list[setdir].topenfile = topenfile;
}
//fcb复制
void fcb_copy(fcb *dest, fcb *src) {
    memset(dest->filename, '\0', 8);
    memset(dest->exname, '\0', 3);
    strcpy(dest->filename, src->filename);
    strcpy(dest->exname, src->exname);
    memcpy(&dest->time_info, &src->time_info, sizeof(struct tm));
    dest->attribute = src->attribute;
    dest->first = src->first;
    dest->length = src->length;
    dest->free = src->free;
    dest->len = src->len;
}
//FAT初始化
void fat_format(){
    fat *fat0 = (fat *) (myvhard + BLOCK_SIZE);
    fat *fat1 = (fat *) (myvhard + BLOCK_SIZE * (FAT_NUM + 1));
    int i;
    for (i = 0; i < BLOCK_NUM; i++, fat0++, fat1++) {
        fat0->id = FREE;
        fat1->id = FREE;
    }
}
//获取空闲块 否则返回-1
int get_free() {
    unsigned char *ptr = myvhard;
    fat *fat0 = (fat *) (ptr + BLOCK_SIZE);
    fat *fat1 = (fat *) (ptr + BLOCK_SIZE * (1+FAT_NUM));
    int i ;
    for (i = 0; i < BLOCK_NUM; i++, fat0++,fat1++) {
        if (fat0->id == FREE){
            fat0->id = END;
            return i;
        }
    }
    return -1;
}
//fat表分配空间
int fat_allocate(unsigned short first, unsigned short length){
    fat *fat0 = (fat *) (myvhard + BLOCK_SIZE);
    fat *fat1 = (fat *) (myvhard + BLOCK_SIZE * FAT_NUM + BLOCK_SIZE);
    int i,j;
    int allocated;
    //到达起始位置
    fat0 = fat0 + first;
    for (i = 0; i < length-1; i++) {
        allocated = get_free();
        if(allocated != -1){
            fat0->id = allocated;
        } else{
            fprintf(stderr, "系统已无空闲磁盘块\n");
            //回退操作
            fat0 = (fat *) (myvhard + BLOCK_SIZE);
            fat1 = (fat *) (myvhard + BLOCK_SIZE * (FAT_NUM + 1));
            for (j = 0; j < BLOCK_NUM; j++, fat0++, fat1++) {
                fat0->id = fat1->id;
            }
            return -1;
        }
        fat0 = (fat *) (myvhard + BLOCK_SIZE);
        fat0 = fat0 + allocated;
    }
    fat0->id = END;
    fat0 = (fat *) (myvhard + BLOCK_SIZE);
    fat1 = (fat *) (myvhard + BLOCK_SIZE * (FAT_NUM + 1));
    for (j = 0; j < BLOCK_NUM; j++, fat0++, fat1++) {
        fat1->id = fat0->id;
    }
    return 0;
}
void get_fullname(char *fullname, fcb *fcb) {
    memset(fullname, '\0', NAME_MAX_LENGTH);
    strcat(fullname, fcb->filename);
    if (fcb->attribute == DATA) {
        strncat(fullname, ".", 2);
        strncat(fullname, fcb->exname, 4);
    }
}
int get_useropenfd(){
    int i;
    for (i = 0; i < MAX_OPENFILE; i++) {
        if (openfile_list[i].topenfile == UNASSIGNED) {
            return i;
        }
    }
    return -1;
}
//取得实际地址
void *get_fact_path(char *fact_path, const char *path) {
    //如果开头为\即是从根目录开始,就是其实际地址
    if (path[0] == '\\') {
        strcpy(fact_path, path);
        return 0;
    }
    char str[PATH_MAX_LENGTH];
    char *token,*final;
    //到当前目录
    memset(fact_path, '\0', PATH_MAX_LENGTH);
    strcpy(fact_path, current_dir);
    //划分
    strcpy(str, path);
    token = strtok(str, "/");
    do {
        //当前目录
        if (!strcmp(token, ".")) {
            continue;
        }
        if (!strcmp(token, "~")){
            continue;
        }
        //上层目录
        if (!strcmp(token, "..")) {
            //根目录的上层目录仍为根目录
            if (!strcmp(fact_path, "\\")) {
                continue;
            } else {
                //找到最后一个分割符出现位置
                final = strrchr(fact_path,'/');
                //最后的位置置成\0,到上层目录
                memset(final, '\0', 1);
                continue;
            }
        }
        strcat(fact_path, "/");
        strcat(fact_path, token);
    } while ((token = strtok(NULL, "/")) != NULL);
    return fact_path;
}
//寻找文件或目录在块中位置  从根目录开始翻
fcb *find_fcb(const char *path, int *cnt) {
    char fact_path[PATH_MAX_LENGTH];
    get_fact_path(fact_path, path);
    if (strcmp(fact_path,"\\")==0) {
        *cnt = ROOT_START;
        return (fcb *) (myvhard + BLOCK_SIZE * ROOT_START);
    }
    char *token = strtok(fact_path, "/");
    token = strtok(NULL, "/");
    return find_fcb_r(token, ROOT_START,cnt);
}
//寻找文件或目录在块中位置  开始递归
fcb *find_fcb_r(char *token, int start,int *cnt) {
    int i, length = BLOCK_SIZE;
    char fullname[NAME_MAX_LENGTH] = "\0";
    fcb *root = (fcb *) (BLOCK_SIZE * start + myvhard);
    fcb *dir;
    block0 *init_block = (block0 *) myvhard;
    for (i = 0, dir = root; i < length / sizeof(fcb); i++, dir++) {
        if (dir->free == EMPTY) {
            continue;
        }
        //取得完整文件名
        get_fullname(fullname, dir);
        if (!strcmp(token, fullname)) {
            token = strtok(NULL, "/");
            if (token == NULL) {
                *cnt = start;
                return dir;
            }
            return find_fcb_r(token, dir->first,cnt);
        }
    }
    fat *fat0 = (fat *) (myvhard + BLOCK_SIZE);
    fat0 = fat0 + start;
    if(fat0->id == END){
        *cnt = 0;
        return NULL;
    } else{
        return find_fcb_r(token, fat0->id,cnt);
    }
}
void reclaim_space_fat(int cnt){
    fcb * dir = (fcb *)(myvhard + BLOCK_SIZE * cnt);
    int i;
    int length = BLOCK_SIZE;
    int flag = 0;
    fat *fat0 = (fat *) (myvhard + BLOCK_SIZE);
    fat *fat1 = (fat *) (myvhard + BLOCK_SIZE * FAT_NUM + BLOCK_SIZE);
    int flag2 = 0;
    for (i = 0 ; i < length / sizeof(fcb); i++, dir++) {
        if (dir->free == UNEMPTY) {
            flag = 1;
            break;
        }
    }
    if (flag == 0){
        return;
    }
    for (int j = 0; j < BLOCK_NUM; ++j,fat0++,fat1++) {
        if(fat0->id == cnt){
            flag2 =1;
            break;
        }
    }
    if(flag2 == 0){
        return;
    }
    fat* fat0_copy = (fat *) (myvhard + BLOCK_SIZE);
    fat* fat1_copy = (fat *) (myvhard + BLOCK_SIZE * FAT_NUM + BLOCK_SIZE);
    fat0_copy = fat0_copy + cnt;
    fat1_copy = fat1_copy + cnt;
    fat0->id = fat0_copy->id;
    fat1->id = fat1_copy->id;
    fat0_copy->id = fat1_copy->id = FREE;

}
void cd_open(char *args,int mode) {
    int fd;
    char fact_path[PATH_MAX_LENGTH];
    fcb *dir;
    int i;
    int cnt;
    memset(fact_path, '\0', PATH_MAX_LENGTH);
    get_fact_path(fact_path, args);
    dir = find_fcb(fact_path,&cnt);
    if ( dir == NULL) {
        fprintf(stderr, "找不到该项目\n");
        return;
    }
    if (mode == DICTORY && dir->attribute == DATA){
        fprintf(stderr, "请不要输入文件名\n");
        return;
    }
    if (mode == DATA && dir->attribute == DICTORY){
        fprintf(stderr, "请不要输入文件夹名\n");
        return;
    }
    //检查cd是否已经在openfile_list内
    for (i = 0; i < MAX_OPENFILE; i++) {
        if (openfile_list[i].topenfile == UNASSIGNED) {
            continue;
        }
        if (!strcmp(dir->filename, openfile_list[i].open_fcb.filename) &&
            dir->first == openfile_list[i].open_fcb.first) {
            //设置打开文件位置
            curdir = i;
            memset(current_dir, '\0', sizeof(current_dir));
            strcpy(current_dir, openfile_list[i].dir);
            printf("项目名为：%s扩展名为:%s路径为:%s修改时间为：%d年%d月%d日%d时%d分%d秒\n",  openfile_list[curdir].open_fcb.filename,openfile_list[curdir].open_fcb.exname,openfile_list[curdir].dir
                    ,openfile_list[curdir].open_fcb.time_info.tm_year+1900,openfile_list[curdir].open_fcb.time_info.tm_mon+1,openfile_list[curdir].open_fcb.time_info.tm_mday,openfile_list[curdir].open_fcb.time_info.tm_hour,openfile_list[curdir].open_fcb.time_info.tm_min,openfile_list[curdir].open_fcb.time_info.tm_sec);
            return;
        }
    }
    //文件未打开
    fd = get_useropenfd();
    if (fd == -1) {
        fprintf(stderr, "没有多余的打开窗口,请先关闭某个窗口\n");
        return;
    }
    openfile_set(fd,dir,fact_path,0,UNMODEFIED,ASSIGNED);
    curdir = fd;
    memset(current_dir, '\0', sizeof(current_dir));
    strcpy(current_dir, openfile_list[fd].dir);
    printf("项目名为：%8s扩展名为:%-6s路径为:%s修改时间为：%d年%d月%d日%d时%d分%d秒\n",  openfile_list[curdir].open_fcb.filename,openfile_list[curdir].open_fcb.exname,openfile_list[curdir].dir
            ,openfile_list[curdir].open_fcb.time_info.tm_year+1900,openfile_list[curdir].open_fcb.time_info.tm_mon+1,openfile_list[curdir].open_fcb.time_info.tm_mday,openfile_list[curdir].open_fcb.time_info.tm_hour,openfile_list[curdir].open_fcb.time_info.tm_min,openfile_list[curdir].open_fcb.time_info.tm_sec);
}
fcb *find_free_space(int first){
    int i;
    fcb *dir = (fcb *) (myvhard + BLOCK_SIZE * first);
    for (i = 0; i < BLOCK_SIZE / sizeof(fcb); i++, dir++) {
        if (dir->free == EMPTY) {
            return dir;
        }
    }
    fat *fat0 = (fat *) (myvhard + BLOCK_SIZE);
    fat0 = fat0 + first;
    if(fat0->id == END){
        return NULL;
    } else{
        return find_free_space(fat0->id);
    }
}
void init_folder(int first, int second) {
    int i;
    fcb *par = (fcb *) (myvhard + BLOCK_SIZE * first);
    fcb *cur = (fcb *) (myvhard + BLOCK_SIZE * second);

    fcb_set(cur, ".", "dic", DICTORY, second, BLOCK_SIZE, UNEMPTY,0);
    cur++;
    fcb_set(cur, "..", "dic", DICTORY, first, par->length, UNEMPTY,0);
    cur++;
    for (i = 2; i < BLOCK_SIZE / sizeof(fcb); i++, cur++) {
        cur->free = EMPTY;
    }
}
void mkdir_create(char *args,int mode) {
    int first,second,thrid;
    char path[PATH_MAX_LENGTH];
    char parpath[PATH_MAX_LENGTH], dirname[PATH_MAX_LENGTH];
    char *end;
    fcb *dir = NULL;
    fat *fat0 = (fat *) (myvhard + BLOCK_SIZE);
    int i;
    int cnt;
    get_fact_path(path, args);
    end = strrchr(path, '/');
    memset(parpath,'\0', PATH_MAX_LENGTH);
    if (end == NULL) {
        fprintf(stderr, "不要输入特殊字符/\n");
        return;
    } else {
        strncpy(parpath, path, end - path);
        strcpy(dirname, end + 1);
    }
    if (find_fcb(parpath,&cnt) == NULL) {
        fprintf(stderr, "无法找到路径 %s\n", parpath);
        return;
    }
    if (find_fcb(path,&cnt) != NULL) {
        fprintf(stderr, "'%s已存在\n", args);
        return;
    }
    first = find_fcb(parpath,&cnt)->first;
    fat0 = fat0 + first;
    second = fat0->id;
    while (second != END){
        fat0 = (fat *) (myvhard + BLOCK_SIZE);
        fat0 = fat0 + second;
        second = fat0->id;
    }
    dir = find_free_space(first);
    if(dir == NULL){
        second = get_free();
        if(second !=-1){
            fat_allocate(second,1);
            fat0->id = second;
            fcb *cur = (fcb *) (myvhard + BLOCK_SIZE * second);
            for (i = 0; i < BLOCK_SIZE / sizeof(fcb); i++, cur++) {
                cur->free = 0;
            }
            find_fcb(parpath,&cnt)->length += BLOCK_SIZE;
        } else{
            fprintf(stderr, "系统全部写满\n");
            return;
        }
        dir = (fcb *) (myvhard + second);
    }
    thrid= get_free();
    if(thrid !=-1){
        fat_allocate(thrid,1);
    } else{
        fprintf(stderr, "系统全部写满\n");
        return;
    }
    if(mode == DICTORY){
        fcb_set(dir, dirname, "dic", DICTORY, thrid, BLOCK_SIZE, UNEMPTY,0);
        init_folder(first, thrid);
    } else{
        char *token = strtok(dirname, ".");
        char exname[8];
        char filename[8];
        strncpy(filename, token, 8);
        token = strtok(NULL, ".");
        if (token != NULL) {
            strncpy(exname, token, 4);
        } else {
            strncpy(exname, "dat", 3);
        }
        fcb_set(dir, filename, exname, DATA, thrid, BLOCK_SIZE, UNEMPTY,0);
    }
}
void rmdir_rm(char *args, int mode){
    int i, j;
    fcb *dir;
    fcb *dir_start;
    int first;
    int cnt;
    char path[PATH_MAX_LENGTH];
    get_fact_path(path, args);
    i = (int )strlen(path);
    //检查..,.不能被删除
    if(path[i-1] == '.'){
        fprintf(stderr, "没有权限删除.和..\n");
        return ;
    }
    if (!strcmp(path, "\\")) {
        fprintf(stderr, "没有权限删除根目录\n");
        return ;
    }
    dir = find_fcb(path,&cnt);
    if(mode == DICTORY){
        if (dir == NULL) {
            fprintf(stderr, "找不到文件夹%s\n", path);
            return ;
        }
        if (dir->attribute == DATA) {
            fprintf(stderr, "请使用rm指令删除文件%s\n", path);
            return ;
        }
    } else{
        if (dir == NULL) {
            fprintf(stderr, "找不到文件%s\n", path);
            return ;
        }
        if (dir->attribute == DICTORY) {
            fprintf(stderr, "请使用rmdir指令删除文件夹%s\n", path);
            return ;
        }
    }
    //查看该项目是否被打开
    for (j = 0; j < MAX_OPENFILE; j++) {
        if (openfile_list[j].topenfile == UNASSIGNED) {
            continue;
        }
        if (!strcmp(dir->filename, openfile_list[j].open_fcb.filename) &&
            dir->first == openfile_list[j].open_fcb.first) {
            fprintf(stderr, "请先关闭%s,再删除\n", path);
            return;
        }
    }
    first = dir->first;
    if(mode == DICTORY){
        dir_start = (fcb *) (myvhard + BLOCK_SIZE * first);
        dir_start++;
        dir_start++;
        if(dir_start->free == UNEMPTY){
            fprintf(stderr, "文件夹%s中还存在文件\n", path);
            return;
        }
        dir->free = EMPTY;
        reclaim_space_fat(cnt);
        dir_start = (fcb *) (myvhard + BLOCK_SIZE * first);
        dir_start->free = EMPTY;
        dir_start++;
        dir_start->free = EMPTY;
        reclaim_space(first);
    } else{
        dir->free = EMPTY;
        reclaim_space(first);
    }
}
void reclaim_space(int first){
    fat *fat0 = (fat *) (myvhard + BLOCK_SIZE);
    fat *fat1 = (fat *) (myvhard + BLOCK_SIZE * FAT_NUM + BLOCK_SIZE);
    fat0 = fat0 + first;
    fat1 = fat1 + first;
    int offset;
    while (fat0->id != END) {
        offset = fat0->id - first;
        first = fat0->id;
        fat0->id = FREE;
        fat1->id = FREE;
        fat0 += offset;
        fat1 += offset;
    }
    fat0->id = FREE;
    fat1->id = FREE;

}
void ls(){
    int first = openfile_list[curdir].open_fcb.first;
    fat *fat0 = (fat *) (myvhard + BLOCK_SIZE);
    fcb *dir = (fcb *) (myvhard + BLOCK_SIZE * first);
    int i;
    fat0 = fat0 + first;
    block0 *init_block = (block0 *) myvhard;
    if (openfile_list[curdir].open_fcb.attribute == DATA){
        fprintf(stderr, "'%s'不是一个目录\n", openfile_list[curdir].open_fcb.filename);
        return ;
    }
    while (fat0->id != END){
        dir = (fcb *) (myvhard + BLOCK_SIZE * first);
        for (i = 0; i < BLOCK_SIZE / sizeof(fcb); i++, dir++)  {
            if (dir->free == EMPTY) {
                continue;
            }
            printf("项目名为：%s,扩展名为:%s,属性为%c,长度为%ld,修改时间为：%d年%d月%d日%d时%d分%d秒\n",  dir->filename,dir->exname,dir->attribute,dir->length,
                   dir->time_info.tm_year+1900,dir->time_info.tm_mon+1,dir->time_info.tm_mday,dir->time_info.tm_hour,dir->time_info.tm_min,dir->time_info.tm_sec);
        }
        first = fat0->id;
        fat0 = (fat *) (myvhard + BLOCK_SIZE);
        fat0 = fat0 + first;
    }
    dir = (fcb *) (myvhard + BLOCK_SIZE * first);
    for (i = 0; i < BLOCK_SIZE / sizeof(fcb); i++, dir++)  {
        if (dir->free == EMPTY) {
            continue;
        }
        printf("项目名为：%s扩展名为:%s属性为%c长度为%ld个磁盘块修改时间为：%d年%d月%d日%d时%d分%d秒\n",  dir->filename,dir->exname,dir->attribute,dir->length,
                dir->time_info.tm_year+1900,dir->time_info.tm_mon+1,dir->time_info.tm_mday,dir->time_info.tm_hour,dir->time_info.tm_min,dir->time_info.tm_sec);
    }

}
void close(int fd){
    int cur;
    if (openfile_list[fd].topenfile == UNASSIGNED){
        fprintf(stderr, "这个文件已处于关闭状态\n");
        return;
    }
    if (!strcmp(openfile_list[fd].dir,"\\")){
        fprintf(stderr, "根目录无法关闭\n");
        return;
    }
    if (fd == curdir){
        curdir = 0;
        strcpy(current_dir, openfile_list[curdir].dir);
    }
    if (openfile_list[fd].fcb_state == 1) {
        fcb_copy(find_fcb(openfile_list[fd].dir,&cur), &openfile_list[fd].open_fcb);
    }
    openfile_list[fd].topenfile = UNASSIGNED;
}
void write(int des_fd,int mode){
    char aa;
    int len = 0;
    int str_len = 0;
    int len_sum;
    int str_start;
    printf("请输入要写入字节数\n");
    scanf("%d",&str_len);
    char* str = NULL;
    int cur;
    if(mode == 2){
        printf("请输入读写指针的位置\n");
        scanf("%d",&cur);
    } else{
        cur = openfile_list[des_fd].count;
    }
    if(mode == 1){
        str = malloc(str_len);
        len_sum = str_len;
    } else if(mode == 2){
        if((openfile_list[des_fd].open_fcb.len - cur) > str_len){
            len_sum = openfile_list[des_fd].open_fcb.len;
            str_start = cur + str_len;
        } else{
            len_sum = str_len + cur;
            str_start = len_sum;
        }
        str = malloc(len_sum*2);
    } else{
        len_sum = openfile_list[des_fd].open_fcb.len + str_len;
        str = malloc(len_sum*2);
    }
    if (openfile_list[des_fd].open_fcb.attribute == DICTORY){
        fprintf(stderr, "无法写入文件夹\n");
        return ;
    }
    if (openfile_list[des_fd].topenfile == UNASSIGNED){
        fprintf(stderr, "文件未打开\n");
        return ;
    }
    if(mode == 1){
        openfile_list[des_fd].open_fcb.len = len_sum;
        memset(str, '\0',str_len);
        getchar();
        printf("要随机填充吗？\n");
        if((aa=getchar())=='y')for (; len < str_len; ++len)str[len] = (len%10)+'0';
        else{
            getchar();
            while (scanf("%c",&aa)!=EOF){
                str[len++] = aa;
                if(len>=str_len)break;
            }
        }
        str[len] = '\0';
        getchar();
        
    } else if (mode == 2){
        openfile_list[des_fd].open_fcb.len = len_sum;
        openfile_list[des_fd].count = 0;
        memset(str, '\0',len_sum*2);
        do_read(des_fd,cur,str);
        char *input = malloc(str_len*2);
        memset(input, '\0',str_len*2);
        getchar();
        printf("要随机填充吗？\n");
        if((aa=getchar())=='y')for (; len < str_len; ++len)input[len] = (len%10)+'0';
        else{
            getchar();
            while (scanf("%c",&aa)!=EOF){
                input[len++] = aa;
                if(len>=str_len)break;
            }
        }
        str[len] = '\0';
        getchar();     
        char *input_end = malloc(2*(len_sum-str_start));
        openfile_list[des_fd].count = str_start;
        do_read(des_fd,len_sum-str_start,input_end);
        strcat(str,input);
        free(input);
        strcat(str,input_end);
        free(input_end);
    } else{
        openfile_list[des_fd].count = 0;
        memset(str, '\0',len_sum*2);
        do_read(des_fd,openfile_list[des_fd].open_fcb.len,str);
        openfile_list[des_fd].open_fcb.len = len_sum;
        char *input = malloc(str_len*2);
        memset(input, '\0',str_len*2);
        getchar();
        printf("要随机填充吗？\n");
        if((aa=getchar())=='y')for ( ; len < str_len; ++len)input[len] = (len%10)+'0';
        else{
            getchar();
            while (scanf("%c",&aa)!=EOF){
                input[len++] = aa;
                if(len>=str_len)break;
            }
        }
        input[len] = '\0';
        getchar();
        strcat(str,input);
        free(input);
    }
    do_write(des_fd,str,len_sum,mode);
}
void do_write(int des_fd,char *str, int len,int mode){
    int first = openfile_list[des_fd].open_fcb.first;
    fat *fat0 = (fat *) (myvhard + BLOCK_SIZE);
    fat *fat1 = (fat *) (myvhard + BLOCK_SIZE * FAT_NUM + BLOCK_SIZE);
    reclaim_space(first);
    int cur;
    int block_num = (int)((len+BLOCK_SIZE-1)/BLOCK_SIZE );
    int write = get_free();
    first = write;
    if (write == -1){
        fprintf(stderr, "系统已无空闲磁盘块\n");
        fat0 = (fat *) (myvhard + BLOCK_SIZE);
        fat1 = (fat *) (myvhard + BLOCK_SIZE * FAT_NUM + BLOCK_SIZE);
        memcpy(fat0, fat1, FAT_NUM*BLOCK_SIZE);
    }
    memcpy(myvhard+write*(BLOCK_SIZE),str,BLOCK_SIZE);
    fcb *dir = find_fcb(openfile_list[des_fd].dir,&cur);
    dir->first = write;
    int old = write;
    int i = 1;
    fat0 = fat0 + old;
    fat0->id = END;
    block_num--;
    while (i <= block_num){
        write = get_free();
        if (write == -1){
            fprintf(stderr, "系统已无空闲磁盘块\n");
            fat0 = (fat *) (myvhard + BLOCK_SIZE);
            fat1 = (fat *) (myvhard + BLOCK_SIZE * FAT_NUM + BLOCK_SIZE);
            memcpy(fat0, fat1, FAT_NUM*BLOCK_SIZE);
        }
        fat0->id = write;
        fat0 = (fat *) (myvhard + BLOCK_SIZE);
        fat0 = fat0 + write;
        fat0->id = END;
        memcpy(myvhard+write*(BLOCK_SIZE),str+BLOCK_SIZE*i,BLOCK_SIZE);
        i++;
    }
    fat0 = (fat *) (myvhard + BLOCK_SIZE);
    fat1 = (fat *) (myvhard + BLOCK_SIZE * FAT_NUM + BLOCK_SIZE);
    memcpy(fat1, fat0, FAT_NUM*BLOCK_SIZE);
    dir->length =  (int)((len+BLOCK_SIZE-1)/BLOCK_SIZE )*BLOCK_SIZE;
    dir->len = len;
    openfile_set(des_fd,dir,openfile_list[des_fd].dir,0,UNMODEFIED,ASSIGNED);
}
int do_read(int des_fd,int len,char *text){
    if((openfile_list[des_fd].count + len) > openfile_list[des_fd].open_fcb.len){
        fprintf(stderr, "读取超过文件总长\n");
        return -1;
    }
    // 计算逻辑块号和块内偏移量
    int block_num = openfile_list[des_fd].count / BLOCK_SIZE;
    int block_offset = openfile_list[des_fd].count % BLOCK_SIZE;
    char buf[BLOCK_SIZE];
    // 得到物理块号
    fat* fat1 = (fat*)(myvhard + BLOCK_SIZE);
    int cur_fat = openfile_list[des_fd].open_fcb.first;
    fat* fat_ptr = fat1 + openfile_list[des_fd].open_fcb.first;
    for(int i = 0; i < block_num; i++){
        cur_fat = fat_ptr->id;
        fat_ptr = fat1 + fat_ptr->id;
    }
    // 开始读取
    int cnt = 0;
    while(cnt < len){
        unsigned char* pos = (unsigned char*)(myvhard + BLOCK_SIZE*cur_fat);
        for(int i = 0; i < BLOCK_SIZE; ++i){
            buf[i] = pos[i];
        }
        for(; block_offset < BLOCK_SIZE; ++block_offset){
            text[cnt] = buf[block_offset];
            ++cnt;
            openfile_list[des_fd].count++;
            if(cnt == len){
                break;
            }
        }
        if(cnt < len){
            cur_fat = fat_ptr->id;
            fat_ptr = fat1 + fat_ptr->id;
            block_offset = 0;
        }
    }
    text[cnt] = '\0';
    return cnt;
}
void read(int des_fd){
    if(des_fd < 0 || des_fd >= MAX_OPENFILE){
        fprintf(stderr, "文件描述符错误\n");
        return;
    }
    int start;
    printf("请输入文件读取开始位置\n");
    scanf("%d",&start);
    int len;
    printf("请输入读取长度\n");
    scanf("%d",&len);
    char *text = malloc(len*2);
    if (openfile_list[des_fd].open_fcb.attribute == DICTORY){
        fprintf(stderr, "无法读入文件夹\n");
        return ;
    }
    if (openfile_list[des_fd].topenfile == UNASSIGNED){
        fprintf(stderr, "文件未打开\n");
        return ;
    }
    if (len > openfile_list[des_fd].open_fcb.len){
        fprintf(stderr, "读入长度大于文件总长\n");
        return ;
    }
    openfile_list[des_fd].count = start;
    int cnt = do_read(des_fd,  len, text);
    if(cnt == -1){
        fprintf(stderr, "读取文件错误\n");
        return ;
    }else{
        printf("%s\n", text);
        printf("共读取 %d B\n", cnt);
        getchar();
    }
}
void my_exitsys(){
    FILE *fp = fopen(PATH, "w");
    fwrite(myvhard, SIZE, 1, fp);
    fclose(fp);
    free(myvhard);
}
void pwd(){
    printf("当前打开文件/文件夹表为\n");
    int i;
    for (i = 0; i < MAX_OPENFILE; i++) {
        if (openfile_list[i].topenfile == UNASSIGNED) {
            continue;
        } else{
            printf("第%d项,项目名为：%s扩展名为:%s路径为:%s修改时间为：%d年%d月%d日%d时%d分%d秒\n",  i,openfile_list[i].open_fcb.filename,openfile_list[i].open_fcb.exname,openfile_list[i].dir
                    ,openfile_list[i].open_fcb.time_info.tm_year+1900,openfile_list[i].open_fcb.time_info.tm_mon+1,openfile_list[i].open_fcb.time_info.tm_mday,openfile_list[i].open_fcb.time_info.tm_hour,openfile_list[i].open_fcb.time_info.tm_min,openfile_list[i].open_fcb.time_info.tm_sec);
        }
    }
}
void printfat(){
    fat* fat1 = (fat*)(myvhard + BLOCK_SIZE);
    int cnt = 0;
    for(int i = 0; i < SIZE / BLOCK_SIZE; ++i){
        printf("%04x ",fat1->id);
        ++fat1;
        ++cnt;
        if(cnt % 16 == 0){
            printf("\n");
            cnt = 0;
        }
    }
    printf("\n");
}
int main(){
    startsys();
    printf("文件系统初始化成功\n");
    while(1){
        printf("%s> ", current_dir);
        char yes;
        char command[100] = "";
        fgets(command, sizeof(command), stdin);
        command[strlen(command) - 1] = '\0';
        if(strncmp(command, "format", 6) == 0){
           printf("是否更改磁盘大小,将重新初始化，丢失文件数据。\n");
            if((yes=getchar())=='y'){
                format();
                startsys();
            }
        }
        else if(strncmp(command, "cd", 2) == 0){
            char *dirname = command + 3;
            cd_open(dirname,DICTORY);
        }else if(strncmp(command, "mkdir", 5) == 0){
            char *dirname = command + 6;
            mkdir_create(dirname,DICTORY);
        }
        else if(strncmp(command, "rmdir", 5) == 0){
            char *dirname = command + 6;
            rmdir_rm(dirname,DICTORY);
        }
        else if(strncmp(command, "ls", 2) == 0){
            ls();
        }
        else if(strncmp(command, "create", 6) == 0){
            char *dirname = command + 7;
            mkdir_create(dirname,DATA);
        }
        else if(strncmp(command, "rm", 2) == 0){
            char *dirname = command + 3;
            rmdir_rm(dirname,DATA);
        }
        else if(strncmp(command, "open", 4) == 0){
            char *dirname = command + 5;
            cd_open(dirname,DATA);
        }
        else if(strncmp(command, "close", 5) == 0){
            pwd();
            printf("输入想要关闭的项号\n");
            int dir;
            scanf("%d",&dir);
            getchar();
            close(dir);
        }
        else if(strncmp(command, "write", 5) == 0){
            int mode;
            printf("请选择写入模式(1.截断2.覆盖3.追加)\n");
            scanf("%d",&mode);
            pwd();
            printf("输入想要写入的项号\n");
            int dir;
            scanf("%d",&dir);
            write(dir,mode);
        }
        else if(strncmp(command, "read", 4) == 0){
            pwd();
            printf("输入想要读入的项号\n");
            int dir;
            scanf("%d",&dir);
            read(dir);
        }
        else if(strncmp(command, "printfat", 8) == 0){
            printfat();
        }
        else if(strncmp(command, "exit", 4) == 0){
            my_exitsys();
            return 0;
        }
        else{
            printf("命令错误！\n");
        }
    }
}
