#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init_task.h>

static int mod1_init(void)
{
    struct task_struct *p;

    p = &init_task;

    printk(KERN_ALERT "名称\t\t\t进程号\t\t状态\t\t优先级\t\t父进程号\n");

    for_each_process(p)
    {
        if (p->mm == NULL)
        {
            printk(KERN_ALERT "%-10s\t\t%d\t\t%u\t\t%d\t\t%d\n", p->comm, p->pid, p->__state, p->normal_prio, p->parent->pid);
        }
    }

    return 0;
}

// 清理函数
static void mod1_exit(void)
{
    printk(KERN_ALERT "Mod1 unloaded\n");
}

// 函数注册
module_init(mod1_init);
module_exit(mod1_exit);

// 模块许可申明
MODULE_LICENSE("GPL");
