#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>


dev_t pdevid;
int ndevices=1;
static struct cdev cdev;
struct device *pdev;
static struct class *pclass;

int pseudo_open(struct inode* inode, struct file* file)
{
	printk("Pseudo--open method\n");
	return 0;
}
int pseudo_close(struct inode* inode, struct file* file)
{
	printk("Pseudo--release method\n");
	return 0;
}
int pseudo_read(struct file* file, char __user* buf, size_t size, loff_t * off)
{
	printk("Pseudo--read method\n");
	return 0;
}
int pseudo_write(struct file* file, const char __user* buf, size_t size, loff_t * off)
{
	printk("Pseudo--write method\n");
	return -ENOSPC;
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
}


module_init(pseudo_init);
module_exit(pseudo_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sridhar");
MODULE_DESCRIPTION("The pseudo character device driver");

