#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/uaccess.h>
#include<linux/semaphore.h>
#include<linux/types.h>
#include<linux/kthread.h>
#include<linux/wait.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pushpendra");
MODULE_DESCRIPTION("KERNEL SYNCHRONIZATION FOR MULTIPLE DEVICE");


uint32_t read_count =0;
uint32_t write_count =0;
static struct task_struct *wait_thread;
wait_queue_head_t wait_queue_dr;
int wait_queue_flag =0;

#define mem_size 1024
dev_t dev = 0;

static struct cdev char_cdev;
uint8_t *kernel_buffer;
struct semaphore my_sem;

static int __init char_driver_init(void);
static void __exit char_driver_exit(void);
static int char_open(struct inode *inode, struct file *file);
static int char_release(struct inode *inode, struct file *file);
static ssize_t char_read(struct file *filep, char __user *buf,size_t len, loff_t *off);
static ssize_t char_write(struct file *filep, const char *buf,size_t len, loff_t *off);


static struct file_operations fops =
{
	.owner = THIS_MODULE,
	.read = char_read,
	.write = char_write,
	.open = char_open,
	.release = char_release,
};

static int wait_function(void *unused)
{
	while(1)
	{
		pr_info("Waiting For Event...\n");
		wait_event_interruptible(wait_queue_dr,wait_queue_flag !=0);
		if(wait_queue_flag ==2)
		{
			pr_info("Event Came From Exit Function\n");
			return 0;
		}
		if(wait_queue_flag ==1)
		{
			pr_info("Event Came From Read Function - %d\n",++read_count);
		}
		if(wait_queue_flag ==3)
		{
			pr_info("Event Came From Write Function\n",++write_count);
			wait_queue_flag =0;
		}
		wait_queue_flag =0;
	}
	return 0;
}

static int char_open(struct inode *inode, struct file *filep)
{
	pr_info("\nDevice File Opened...\n");
	return 0;
}

static int char_release(struct inode *inode, struct file *filep)
{
	pr_info("Device File CLosed...\n");
	return 0;
}

static ssize_t char_read(struct file *filep, char __user *buf, size_t len, loff_t *off)
{
  if(copy_to_user(buf, kernel_buffer, mem_size))
	{
		pr_err("Data Read: Error\n");
	}
	up(&my_sem);
	pr_info("Data Read: Done\n");
	wait_queue_flag =1;
	wake_up_interruptible(&wait_queue_dr);
	return mem_size;
}

static ssize_t char_write(struct file *filep, const char __user *buf, size_t len, loff_t *off)
{
	down_interruptible(&my_sem);
	
	if(copy_from_user(kernel_buffer, buf, len))
	{
		pr_err("Data Write:Error\n");
	}
	pr_info("Data Write: Done\n");
	wait_queue_flag =3;
	wake_up_interruptible(&wait_queue_dr);
	return len;
}

static int __init char_driver_init(void)
{
	if((alloc_chrdev_region(&dev, 0, 2, "CharDevice"))<0)
	{
		pr_info("Cannot Allocate Major Number\n");
		return -1;
	}
	pr_info("Major->%d Minor->%d\n",MAJOR(dev),MINOR(dev));

	cdev_init(&char_cdev,&fops);


	if((cdev_add(&char_cdev,dev,1))<0)
	{
		pr_info("Cannot Add The Device To The System\n");
	}

	if((kernel_buffer=kmalloc(mem_size, GFP_KERNEL)) == 0)
	{
		pr_info("Cannot Allocate Memory In Kernel\n");
	}
	strcpy(kernel_buffer, "Hello World");

	init_waitqueue_head(&wait_queue_dr);
	
	wait_thread = kthread_create(wait_function,NULL,"WaitThread");
	if(wait_thread)
	{
		pr_info("Thread Created Successfully\n");
		wake_up_process(wait_thread);
	}
	else
	{
		pr_info("Thread Creation Failed\n");
	}


	pr_info("Device Driver Insert...done\n");
	sema_init(&my_sem,1);
	return 0;
}

static void __exit char_driver_exit(void)
{
	wait_queue_flag =2;
	wake_up_interruptible(&wait_queue_dr);
	kfree(kernel_buffer);
	cdev_del(&char_cdev);
	unregister_chrdev_region(dev,1);
	pr_info("Device Driver Removed\n");
}

module_init(char_driver_init);
module_exit(char_driver_exit);