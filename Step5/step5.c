#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/uaccess.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kfifo.h>


#define MAX_SIZE 1024
dev_t pdevid;
struct cdev cdev;
int ndevices=1;
unsigned char *pbuffer;
int rd_offset=0;
int wr_offset=0;
int buflen=0;
struct device *pdev;
struct class *pclass;
struct kfifo kfifo;

int pseudo_open(struct inode* inode,struct file* file)
{
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
   
   rd_offset += rlen;
   buflen -= rlen;
      
   printk("Pseudo--read method, transferred %d bytes\n",rlen);
   return rlen;
}

ssize_t pseudo_write(struct file * file, const char __user * ubuf , size_t usize, loff_t * off)
{
   int ret, wlen;
   if(kfifo_is_full(&kfifo))
   {
	   printk("Buffer is full\n");
	  return -ENOSPC;
   }
   wlen = usize;
   if(wlen > kfifo_avail(&kfifo))
	   wlen = kfifo_avail(&kfifo);

   char *tbuf = kmalloc(wlen,GFP_KERNEL);

   ret  = copy_from_user(tbuf,ubuf,wlen);

   kfifo_in(&kfifo,tbuf,wlen);
   kfree(tbuf);

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
	pclass=class_create(THIS_MODULE,"pseudo_class");
	pbuffer= kmalloc(MAX_SIZE, GFP_KERNEL);
	kfifo_init(&kfifo,pbuffer,MAX_SIZE);
	//kfifo_alloc(&kfifo,MAX_SIZE,GFP_KERNEL);
	ret=alloc_chrdev_region(&pdevid,0,ndevices,"pseudo_step4");
	if (ret){

		printk("Psuedo: Failed to register driver\n");
		return -EINVAL;
		}
	cdev_init(&cdev,&fops);
	kobject_set_name(&cdev.kobj,"pdevice%d",i);
	ret=cdev_add(&cdev,pdevid,1);
	if (ret){

		printk("Psuedo: Failed to register device\n");
		return -EINVAL;
		}
	pdev=device_create(pclass,NULL,pdevid,NULL,"psample%d",i);

printk("Successfully registered,major = %d ,minor = %d",MAJOR(pdevid),MINOR(pdevid));
printk("Pseudo Driver Sample STEP 5 ..Welcome\n");
return 0;
}
static void __exit pseudo_exit(void)
{
	unregister_chrdev_region(pdevid,ndevices);
	cdev_del(&cdev);
	device_destroy(pclass,pdevid);
	class_destroy(pclass);
	printk("Pseudo driver sample STEP 5 BYE \n");
	kfifo_free(&kfifo);
}

module_init(pseudo_init);
module_exit(pseudo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sridhar");
MODULE_DESCRIPTION("Step 4 Pseudo Drivers");
