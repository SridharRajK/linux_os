#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/uaccess.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kfifo.h>
#include <linux/wait.h>

#define MAX_SIZE 1024

dev_t pdevid;
int ndevices=1;
unsigned char *pbuffer;
int rd_offset = 0;
int wr_offset = 0;
int buflen = 0;
struct cdev cdev;
struct device *pdev;
struct class *pclass;
struct kfifo kfifo;

typedef struct priv_obj{
	struct device *pdev;
	struct cdev cdev;
	struct kfifo kfifo;
	struct list_head lentry;
	wait_queue_head_t rd_queue;
}PRIV_OBJ;

LIST_HEAD(mydevlist);

int pseudo_open(struct inode* inode,struct file* file)
{
	PRIV_OBJ *pobj = container_of(inode->i_cdev, PRIV_OBJ, cdev);
	file->private_data=pobj;
	printk("Pseudo-Open method \n");
	return 0;
};

int pseudo_close(struct inode* inode,struct file* file)
{
	printk("Pseudo-Close method \n");
	return 0;
};

ssize_t pseudo_read(struct file * file, char __user * ubuf , size_t usize, loff_t * off)
{
   int ret, rlen;
   rlen=usize;
   PRIV_OBJ *pobj = file->private_data; 
   file->private_data = pobj;
   
   if(buflen==0) {    //(wr_offset - rd_offset)==0
     printk("pseudo read -- buffer is empty\n");
     return 0;       //buffer is empty
   }    
   if(rlen > buflen)
       rlen = buflen;  // min of uspace request, buffer length
    
   ret=copy_to_user(ubuf, pbuffer, rlen);
   if(ret) {
     printk("Erorr inn copy_to_user,failed");
     return -EINVAL;
   }
   
   if(kfifo_is_empty(&pobj->kfifo))
   	wait_event_interruptible(pobj->rd_queue,(kfifo_len(&pobj->kfifo)>0));
   rd_offset += rlen;
   buflen -= rlen;
      
   printk("Pseudo--read method, transferred %d bytes\n",rlen);
   return rlen;
}

ssize_t pseudo_write(struct file * file, const char __user * ubuf , size_t usize, loff_t * off)
{
   int ret, wlen;
   char *tbuf;
   
   PRIV_OBJ *pobj = file->private_data;
   
   if(kfifo_is_full(&(pobj->kfifo)))
   {
	   printk("Buffer is full\n");
	  return -ENOSPC;
   }
   wlen = usize;
   if(wlen > kfifo_avail(&(pobj->kfifo)))
	   wlen = kfifo_avail(&(pobj->kfifo));

   tbuf = kmalloc(wlen,GFP_KERNEL);

   ret  = copy_from_user(tbuf,ubuf,wlen);

   kfifo_in(&(pobj->kfifo),tbuf,wlen);
   kfree(tbuf);

	wake_up_interruptible(&pobj->rd_queue);
   printk("Pseudo--write method, transferred %d bytes\n",wlen);
   return wlen;
}

struct file_operations fops = {

.open		=	pseudo_open,
.release	=	pseudo_close,
.write		=	pseudo_write,
.read		=	pseudo_read

};

static int __init pseudo_init(void)
{
	int ret,i=0;
	PRIV_OBJ *pobj;
	pobj = kmalloc(sizeof(PRIV_OBJ),GFP_KERNEL);
	list_add_tail(&pobj->lentry, &mydevlist);
	
	init_waitqueue_head(&pobj->rd_queue);

	pclass=class_create(THIS_MODULE,"pseudo_class");
	pbuffer= kmalloc(MAX_SIZE, GFP_KERNEL);
	kfifo_init(&(pobj->kfifo),pbuffer,MAX_SIZE);

	//kfifo_alloc(&kfifo,MAX_SIZE,GFP_KERNEL);
	ret=alloc_chrdev_region(&pdevid,0,ndevices,"pseudo_step10");
	if (ret){

		printk("Psuedo: Failed to register driver\n");
		return -EINVAL;
		}
	cdev_init(&(pobj->cdev),&fops);
	kobject_set_name(&((pobj->cdev).kobj),"pdevice%d",i);
	ret=cdev_add(&(pobj->cdev),pdevid,1);
	if (ret){

		printk("Psuedo: Failed to register device\n");
		return -EINVAL;
		}
	pobj->pdev=device_create(pclass,NULL,pdevid,NULL,"psample%d",i);

printk("Successfully registered,major = %d ,minor = %d",MAJOR(pdevid),MINOR(pdevid));
printk("Step10 Pseudo Driver Sample ..Welcome\n");
return 0;
}
static void __exit pseudo_exit(void)
{
	PRIV_OBJ *ptr, *qtr;
	list_for_each_entry_safe(ptr, qtr, &mydevlist, lentry){
		kfree(ptr);
	}
	unregister_chrdev_region(pdevid,ndevices);
	//cdev_del(&(pobj->cdev));
	device_destroy(pclass,pdevid);
	class_destroy(pclass);
	printk("Step10 Pseudo driver sample BYE \n");
	//kfifo_free(&(pobj->kfifo));
}

module_init(pseudo_init);
module_exit(pseudo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sumanth");
MODULE_DESCRIPTION("Step 10 Pseudo Drivers");
