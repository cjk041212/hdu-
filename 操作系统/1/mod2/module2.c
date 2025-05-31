#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/sched/signal.h>

/* 在使用宏之前，先声明无符号变量 */
static unsigned int pid;

/* module_param(name,type,perm)
name既是用户看到的参数名，也是模块内接收参数的变量
type表示数据的类型，例如：int,double,bool,unit...
perm对于全局可读的变量为0444，对于根目录可写为0644。
*/
module_param(pid, uint, 0644);

static int mod2_init(void)
{
    /* 
    parent   父进程
    children 子进程
    sibling  表示当前进程的链表
    */
    struct task_struct *p;
    struct task_struct *parent;
    struct task_struct *children;
    struct task_struct *sibling;
    struct list_head *list;

    p = NULL;

    /* 根据进程号pid得到进程描述符 */
    p = pid_task(find_get_pid(pid), PIDTYPE_PID);

    if (p == NULL)
        printk(KERN_ALERT "Pid does not exist\n");
    else {
        parent = p->parent;
        printk(KERN_ALERT "父进程:\n");
        printk(KERN_ALERT "程序名\t\tPID号\t进程状态\t进程优先级\n");
        printk(KERN_ALERT "%-10s\t%5d\t%u\t\t%d\n", parent->comm, parent->pid, parent->__state, parent->prio);

        printk(KERN_ALERT "\n兄弟进程:\n");
        printk(KERN_ALERT "程序名\t\tPID号\t进程状态\t进程优先级\n");
        /* 遍历兄弟进程 */
        list_for_each(list, &parent->children) {
            sibling = list_entry(list, struct task_struct, sibling);
            printk(KERN_ALERT "%-10s\t%5d\t%u\t\t%d\n", sibling->comm, sibling->pid, sibling->__state, sibling->prio);
        }

        printk(KERN_ALERT "\n子进程:\n");
        printk(KERN_ALERT "程序名\t\tPID号\t进程状态\t进程优先级\n");
        /* 遍历子进程 */
        list_for_each(list, &p->children) {
            children = list_entry(list, struct task_struct, sibling);
            printk(KERN_ALERT "%-10s\t%5d\t%u\t\t%d\n", children->comm, children->pid, children->__state, children->prio);
        }
        printk(KERN_ALERT "\n");
    }

    return 0;
}

static void mod2_exit(void)
{
    printk(KERN_ALERT "Mod2 unloaded\n");
}

module_init(mod2_init);
module_exit(mod2_exit);

MODULE_LICENSE("GPL");
