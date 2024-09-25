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


struct gpioled_dev {
    dev_t devid;
    int     major;
    int     minor;
    struct cdev    cdev;
    struct class  *class;
    struct device *device;
    struct device_node * nd;
    int  led_gpio;
};

static int led_open(struct inode * inode,struct file * file){
    return 0;
}

static int led_release(struct inode * inode,struct file * file){
    return 0;
} 

static ssize_t led_write(struct file * file,const char __user * buf,size_t count,loff_t * ppos) {
    return 0;
}


static const struct file_operations gpioled_fop = {
    .owner = THIS_MODULE,
    .write = led_write,
    .open = led_open,
    .release = led_release,
};

struct gpioled_dev gpioled;



static int __init gpioled_init(void)
{
    int ret = 0;
    gpioled.major = 0;
    if(gpioled.major){
        gpioled.devid = MKDEV(gpioled.major,0);
        register_chrdev_region(gpioled.devid,GPIOLED_CNT,GPIOLED_NAME);
    }
    else {
        alloc_chrdev_region(&gpioled.devid,0,GPIOLED_CNT,GPIOLED_NAME);
        gpioled.major = MAJOR(gpioled.devid);
        gpioled.minor = MINOR(gpioled.devid);
    }
    printk("gpio major = %d,gpio minor = %d\r\n",gpioled.major,gpioled.minor);
    gpioled.cdev.owner = THIS_MODULE;
    cdev_init(&gpioled.cdev, &gpioled_fop);
    cdev_add(&gpioled.cdev, gpioled.devid, GPIOLED_CNT);

    gpioled.class = class_create(THIS_MODULE,GPIOLED_NAME);
    gpioled.device = device_create(gpioled.class,NULL,gpioled.devid,NULL,GPIOLED_NAME);

    gpioled.nd = of_find_node_by_path ("/gpioled");
    gpioled.led_gpio = of_get_named_gpio(gpioled.nd,"led-gpio",0);

    printk("gpioled_num = %d",gpioled.led_gpio);

    ret = gpio_request(gpioled.led_gpio,"gpioled");

    ret = gpio_direction_output(gpioled.led_gpio,1);

    gpio_set_value (gpioled.led_gpio,0);

    return 0;
}





static void __exit gpioled_exit(void){
    cdev_del(&gpioled.cdev);
    unregister_chrdev_region(gpioled.devid , GPIOLED_CNT);
    device_destroy(gpioled.class,gpioled.devid);
    class_destroy(gpioled.class);
    gpio_free(gpioled.led_gpio);
}

module_init(gpioled_init);
module_exit(gpioled_exit);
MODULE_LICENSE("GPL");



