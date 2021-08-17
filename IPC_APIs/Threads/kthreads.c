#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/sched/signal.h>

static struct task_struct *task1;
static struct task_struct *task2;

static int thread_one(void *pargs)
{
    int i=0;
    allow_signal(SIGKILL);
    while (!kthread_should_stop())
    {
        printk("Thread A--%d\n",i++);
        msleep(5);
    }
    printk("Stopping Thread A\n");
    do_exit(0);
	return 0;
}

static int thread_two(void *pargs)
{
    int i=0;
    allow_signal(SIGKILL);
    while (!kthread_should_stop())
    {
        printk("Thread B--%d\n",i++);
        msleep(5);
    }
    printk("Stopping Thread B\n");
    do_exit(0);
	return 0;
}

static int __init tdemo_init(void) {        //init_module
  task1=kthread_run(thread_one, NULL, "thread_A");  //kthread_create + wake_up_process
  task2=kthread_run(thread_two, NULL, "thread_B");
  printk("Thread Demo..welcome\n");
  return 0;
}
static void __exit tdemo_exit(void) {       //cleanup_module
  if(task1)
      kthread_stop(task1);
  if(task2)
      kthread_stop(task2);
  printk("Thread demo is leaving the world, bye...\n");
}

module_init(tdemo_init);
module_exit(tdemo_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sumanth");
MODULE_DESCRIPTION("Kernel thread Example Module");

