#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>


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

ssize_t pseudo_read(struct file* file, char __user* ubuf, size_t usize, loff_t * off)
{
	int ret, rlen;
    rlen = usize;
   
    if(buflen == 0) {    //(wr_offset - rd_offset)==0
      printk("pseudo read -- buffer is empty\n");
      return 0;       //buffer is empty
    }    
    if(rlen > buflen)
        rlen = buflen;  // min of uspace request and buffer length
    
    ret = copy_to_user(ubuf, pbuffer, rlen);
    if(ret) {
      printk("Error in copy_to_user,failed");
      return -EINVAL;
    }
   
    rd_offset += rlen;
    buflen -= rlen;
      
    printk("Data read successfully, transferred %d bytes\n",rlen);
    return rlen;
}

ssize_t pseudo_write(struct file* file, const char __user* ubuf, size_t usize, loff_t * off)
{
	int ret, wlen, kremain;
   
	wlen=usize;
	kremain = MAX_SIZE - wr_offset;
   
    if(kremain==0)    // wr_offset >= MAX_SIZE
       return -ENOSPC;     //buffer is full
         
    if(wlen > kremain)
       wlen = kremain;    //min of user request, remaining space
      
    ret=copy_from_user(pbuffer, ubuf, wlen);
    if(ret) {
      printk("Error in copy_from_user");
      return -EINVAL;
    }   
   
    buflen += wlen;
    wr_offset += wlen;
   
    printk("Data written successfully, transferred %d bytes\n", wlen);
    return wlen;
}

struct file_operations fops = {
	.open = pseudo_open,
	.release = pseudo_close,
	.write = pseudo_write,
	.read = pseudo_read
};


static int __init pseudo_init(void)
{
	int ret, i = 0;
	
	/* creating struct class */
	pclass = class_create(THIS_MODULE, "pseudo_class");

	/* Allocating Major number */
	ret = alloc_chrdev_region(&pdevid, 0, ndevices, "pseudo_sample");
	if(ret){
		printk("Pseudo: Failed to register driver\n");
		return -EINVAL;
	}
	printk("Device successfully registered, major=%d, minor=%d\n", MAJOR(pdevid), MINOR(pdevid));
	printk("Pseudo Driver Sample.. welcome\n");
	
	
	/* creating cdev structure */
	cdev_init(&cdev, &fops);
	
	kobject_set_name(&cdev.kobj, "pdevice%d", 0);
	
	/* creating Physical memory */
	pbuffer = kmalloc(MAX_SIZE, GFP_KERNEL);
	
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
	printk("pseudo driver removed successfully.. Bye\n");
	class_destroy(pclass);
	kfree(pbuffer);
}


module_init(pseudo_init);
module_exit(pseudo_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sridhar");
MODULE_DESCRIPTION("The pseudo character device driver");

