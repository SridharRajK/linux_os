#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kfifo.h>



#define MAX_SIZE 1024
#define IOC_MAGIC 'p'
#define MY_IOCTL_LEN 		_IO(IOC_MAGIC,1)
#define MY_IOCTL_AVAIL		_IO(IOC_MAGIC,2)
#define MY_IOCTL_RESET		_IO(IOC_MAGIC,3)
#define MY_IOCTL_PSTAT		_IOR(IOC_MAGIC, 4, struct pseudo_stat)


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

int pseudo_open(struct inode* inode, struct file* file)
{
	printk("Pseudo device file opened\n");
	return 0;
}
int pseudo_close(struct inode* inode, struct file* file)
{
	printk("Pseudo device file closed\n");
	return 0;
}

ssize_t pseudo_read(struct file * file, char __user * ubuf , size_t usize, loff_t * off)
{
   int ret, wlen;
   char *tbuf;
   if(kfifo_is_empty(&kfifo))
   {
	   printk("Buffer is Empty\n");
	   return -ENOSPC;
   }
   wlen = kfifo_len(&kfifo);
   tbuf = kmalloc(wlen,GFP_KERNEL);
   wlen = kfifo_out(&kfifo, pbuffer,wlen);
   ret=copy_to_user(ubuf, pbuffer, wlen);
   if(ret) {
     printk("Erorr inn copy_to_user,failed");
     return -EINVAL;
   }
   
   rd_offset += wlen;
   buflen -= wlen;
      
   printk("Pseudo--read method, transferred %d bytes\n",wlen);
   return wlen;
}

ssize_t pseudo_write(struct file * file, const char __user * ubuf , size_t usize, loff_t * off)
{
   char *tbuf;
   int ret, wlen;
   if(kfifo_is_full(&kfifo))
   {
	   printk("Buffer is full\n");
	  return -ENOSPC;
   }
   wlen = usize;
   if(wlen > kfifo_avail(&kfifo))
	   wlen = kfifo_avail(&kfifo);

   tbuf = kmalloc(wlen,GFP_KERNEL);

   ret  = copy_from_user(tbuf,ubuf,wlen);

   kfifo_in(&kfifo,tbuf,wlen);
   kfree(tbuf);

   printk("Pseudo--write method, transferred %d bytes\n",wlen);
   return wlen;
}

struct pseudo_stat{
	int len;
	int avail;
};

struct pseudo_stat stat;

static long pseudo_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	printk("Pseudo--IOCTL method\n");
	switch(cmd){
		case MY_IOCTL_PSTAT:
			printk("IOCTL 8b--kfifo statistics\n");
			stat.len = kfifo_len(&kfifo);
			stat.avail = kfifo_avail(&kfifo);
			//ret = copy_to_user((char __user*)arg, &stat, sizeof(pseudo_stat));
			if(copy_to_user((char __user*)arg, &stat, sizeof(stat))){
				printk("error in copy_to_user\n");
				return -EFAULT;
			}
			break;
		}
	return 0;
}

struct file_operations fops = {
	.open 		= 		pseudo_open,
	.release 	= 		pseudo_close,
	.write 		= 		pseudo_write,
	.read 		= 		pseudo_read,
	.unlocked_ioctl = 	pseudo_ioctl
};



static int __init pseudo_init(void)
{
	int ret, i = 0;
	
	pbuffer = kmalloc(MAX_SIZE, GFP_KERNEL);
	
	/* creating struct class */
	pclass = class_create(THIS_MODULE, "pseudo_class");
	
	/* Allocating Major number */
	ret = alloc_chrdev_region(&pdevid, 0, ndevices, "pseudo_sample");
	if(ret){
		printk("Failed to register driver\n");
		return -EINVAL;
	}
	printk("Device psample0 registered successfully, major=%d, minor=%d\n", MAJOR(pdevid), MINOR(pdevid));
	printk("Step8b pseudo driver sample.. welcome\n");
	
	/* creating cdev structure */
	cdev_init(&cdev, &fops);
	
	kobject_set_name(&cdev.kobj, "pdevice%d", 0);
	
	/* Adding character device to the system */
	ret = cdev_add(&cdev, pdevid, 1);

	/* creating the devide */
	pdev = device_create(pclass, NULL, pdevid, NULL, "psample%d", i);
	
	return 0;
}

static void __exit pseudo_exit(void){
	cdev_del(&cdev);
	device_destroy(pclass, pdevid);
	unregister_chrdev_region(pdevid, ndevices);
	printk("Pseudo driver removed successfully.. step8a Bye\n");
	class_destroy(pclass);
	kfree(pbuffer);
}


module_init(pseudo_init);
module_exit(pseudo_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sridhar");
MODULE_DESCRIPTION("Step8b character device driver");

