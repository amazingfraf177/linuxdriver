#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>



// #define CCM_CCGR1_BASE           (0X020C406C) 
// #define SW_MUX_GPIO1_IO03_BASE   (0X020E0068) 
// #define SW_PAD_GPIO1_IO03_BASE   (0X020E02F4) 
// #define GPIO1_DR_BASE            (0X0209C000) 
// #define GPIO1_GDIR_BASE          (0X0209C004)
 
// static void __iomem  * VI_CCM_CCGR1_BASE;
// static void __iomem  * VI_SW_MUX_GPIO1_IO03_BASE;
// static void __iomem  * VI_SW_PAD_GPIO1_IO03_BASE;
// static void __iomem  * VI_GPIO1_DR_BASE;
// static void __iomem  * VI_GPIO1_GDIR_BASE;


#define led_on  1
#define led_off 0

#define led_name "newled"

static int newled_open (struct inode * inode, struct file * filp) {
    printk("devbase_open\r\n");
    return 0;
}

static int newled_release (struct inode * inode, struct file * filp) {
    printk("newled_release\r\n");
    return 0;
}

static ssize_t  newled_write (struct file * filp , const char __user * buf, size_t count, loff_t * ppos) {
    return 0;
} 



const struct file_operations file = {
    .owner   = THIS_MODULE,
    .open    = newled_open,
    .release = newled_release,
    .write    = newled_write,
};

struct newled_dev {
    struct cdev cdev;
    dev_t  devid;
    struct class * class;
    struct device * device;
    int    major;
    int    minor;
};


struct newled_dev newleddev;

static int __init newled_init(void) {
    int ret = 0;

    printk("newled_init\r\n");

    if (newleddev.major){
        newleddev.minor = MKDEV(newleddev.major,0);
        ret = register_chrdev_region(newleddev.major,1,led_name);
    }
    else {
        ret = alloc_chrdev_region(&newleddev.devid,0,1,led_name);
        newleddev.major = MAJOR(newleddev.devid);
        newleddev.minor = MINOR(newleddev.devid);
    }

    if (ret<0) {
        printk("chrdev_region_error\r\n");
        return -1;
    }

    printk("major = %d,minor = %d\r\n",newleddev.major,newleddev.minor);

    newleddev.cdev.owner = THIS_MODULE;

    cdev_init(&newleddev.cdev,&file);

    ret = cdev_add(&newleddev.cdev,newleddev.devid,1);

    newleddev.class = class_create(THIS_MODULE,led_name);

    newleddev.device = device_create(newleddev.class,NULL,newleddev.devid,NULL,led_name);

    return 0;
}


static void __exit newled_exit(void) {

    printk("newled_exit \r\n");
    unregister_chrdev_region(newleddev.devid,1);

    cdev_del(&newleddev.cdev);
    device_destroy(newleddev.class,newleddev.devid);
    class_destroy(newleddev.class);
}

module_init(newled_init);
module_exit(newled_exit);
MODULE_LICENSE("GPL");