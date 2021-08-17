//sample.c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

extern int xvar;
extern void sayHello(void){}

static int __init sample_init(void)
{
  printk("Hello World.. xvar=%d\n",xvar);
  sayHello();
  return 0;
}

static void __exit sample_exit(void)
{
  printk("Bye, leaving the world\n");
}

module_init(sample_init);
module_exit(sample_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your name");
MODULE_DESCRIPTION("A hello World Module");

