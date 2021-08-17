#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>


struct Box {
  int l;
  int b;
  int h;
  struct list_head entry;
};

LIST_HEAD(boxlist);

int __init listdemo_init(void) {        //init_module
  int i;
  struct Box *pb;
  for(i=1;i<=10;i++) {
    pb=kmalloc(sizeof(struct Box), GFP_KERNEL);
    pb->l=i*100;
    pb->b=i*100+10;
    pb->h=i*1000+i*100+i;
    list_add_tail(&pb->entry, &boxlist);
  }    
  //printdata();
  printk("Hello World..welcome\n");
  return 0;
}

void printdata(void) {
  struct list_head* pcur;
  struct Box *ptr;
  list_for_each(pcur,&boxlist) {
    ptr=list_entry(pcur, struct Box,entry); //container_of
    //print ptr->l,ptr->b, ptr->h
  }
}

void __exit listdemo_exit(void) {       //cleanup_module
  struct list_head *pcur,*pbak;
  struct Box *pb;
  list_for_each_safe(pcur,pbak, &boxlist) {
    pb=list_entry(pcur,struct Box, entry); //container_of
    kfree(pb);
  }
  printk("Bye,Leaving the world\n");
}

module_init(listdemo_init);
module_exit(listdemo_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sumanth");
MODULE_DESCRIPTION("A Hello, World Module");
