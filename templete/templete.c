#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define GPIOLED_CNT 1
#define GPIOLED_NAME  "GPIOLED" 


struct templete {
    dev_t devid;
    int     major;
    int     minor;
    struct cdev    cdev;
    struct class  *class;
    struct device *device;
    struct device_node * nd;
};

static int templete_open(struct inode * inode,struct file * file){

    return 0;
}

static int templete_release(struct inode * inode,struct file * file){

    return 0;
} 

static ssize_t templete_write(struct file * file,const char __user * buf,size_t count,loff_t * ppos) {
    return 0;
}



static ssize_t templete_read (struct file * filp , char __user * buf, size_t count, loff_t * ppos) {
    return 0;
} 


static const struct file_operations gpioled_fop = {
    .owner = THIS_MODULE,
    .write = templete_write,
    .open = templete_open,
    .release = templete_release,
    .read = templete_read,
};

struct templete templete;



static int __init gpioled_init(void)
{
    templete.major = 0;
    if(templete.major){
        templete.devid = MKDEV(templete.major,0);
        register_chrdev_region(templete.devid,GPIOLED_CNT,GPIOLED_NAME);
    }
    else {
        alloc_chrdev_region(&templete.devid,0,GPIOLED_CNT,GPIOLED_NAME);
        templete.major = MAJOR(templete.devid);
        templete.minor = MINOR(templete.devid);
    }
    printk("gpio major = %d,gpio minor = %d\r\n",templete.major,templete.minor);
    templete.cdev.owner = THIS_MODULE;
    cdev_init(&templete.cdev, &gpioled_fop);
    cdev_add(&templete.cdev, templete.devid, GPIOLED_CNT);

    templete.class = class_create(THIS_MODULE,GPIOLED_NAME);
    templete.device = device_create(templete.class,NULL,templete.devid,NULL,GPIOLED_NAME);



    return 0;
}





static void __exit gpioled_exit(void){
    cdev_del(&templete.cdev);
    unregister_chrdev_region(templete.devid , GPIOLED_CNT);
    device_destroy(templete.class,templete.devid);
    class_destroy(templete.class);
}

module_init(gpioled_init);
module_exit(gpioled_exit);
MODULE_LICENSE("GPL");



