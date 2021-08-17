#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/sched/signal.h>

static struct task_struct *task1;
static struct task_struct *task2;

static int val=100;
const int max=20;


static int thread_one(void *pargs)
{
    int i=0;
    for(i=1;i<=max;i++)
    {
    	val++;
    	if(kthread_should_stop())
    		break;
    }
    printk("Stopping Thread A\n");
    do_exit(0);
	return 0;
}

static int thread_two(void *pargs)
{
    int i=0;
    for(i=1;i<=max;i++)
    {
    	val--;
    	if(kthread_should_stop())
    		break;
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
  printk("Thread Demo,Leaving the world,val=%d\n",val);
}

module_init(tdemo_init);
module_exit(tdemo_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sumanth");
MODULE_DESCRIPTION("Thread Example Module");

