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
#include <linux/timer.h>
#include <linux/jiffies.h>


#define GPIOLED_CNT 1
#define GPIOLED_NAME  "GPIOLED" 


#define CLOSE_CMD      (_IO(0XEF,1))
#define OPEN_CMD       (_IO(0XEF,2))
#define SETPERIOD_CMD  (_IOW(0XEF,3,int))



struct templete {
    dev_t devid;
    int     major;
    int     minor;
    struct cdev    cdev;
    struct class  *class;
    struct device *device;
    struct device_node * nd;
    struct timer_list timer;
    unsigned int timer_time;
    int gpio_led;
};

struct templete templete;

static int templete_open(struct inode * inode,struct file * filp){
    // int ret = 0;

    filp->private_data = &templete; /* 设置私有数据 */ 
    // templete.timer_time = 500;  
    return 0;
}

static int templete_release(struct inode * inode,struct file * filp){

    return 0;
} 

static ssize_t templete_write(struct file * file,const char __user * buf,size_t count,loff_t * ppos) {
    return 0;
}



static ssize_t templete_read (struct file * filp , char __user * buf, size_t count, loff_t * ppos) {
    return 0;
} 

static long templete_ioctl (struct file * filp, unsigned int cmd ,unsigned long arg){
    int ret = 0;
    struct templete * save = (struct templete *)filp->private_data;    

    switch(cmd) {
        case CLOSE_CMD:
            del_timer_sync(&save->timer);
            break;
        case OPEN_CMD:
            mod_timer(&save->timer,jiffies + msecs_to_jiffies(save->timer_time));
            break;
        case SETPERIOD_CMD:

            break;
    } 


}






static const struct file_operations gpioled_fop = {
    .owner = THIS_MODULE,
    .write = templete_write,
    .open = templete_open,
    .release = templete_release,
    .unlocked_ioctl =  templete_ioctl, 
    .read = templete_read,
};


int led_init (struct templete * templete) {
    templete->nd = of_find_node_by_path("/gpioled");
    templete->gpio_led = of_get_named_gpio(templete->nd,"led-gpio",0);
    
    gpio_request(templete->gpio_led,"led");
    gpio_direction_output(templete->gpio_led,1);
    return 0;
}


static void timer_func(unsigned long arg) {
    struct templete *templete = (struct templete*) arg;
    static int set = 1;
    set = !set;
    gpio_set_value (templete->gpio_led,set);
    mod_timer(&templete->timer,jiffies + msecs_to_jiffies(templete->timer_time));
}


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

    led_init(&templete);

    init_timer(&templete.timer);
    templete.timer_time = 500;
    templete.timer.function = timer_func;
    templete.timer.expires = jiffies + msecs_to_jiffies(templete.timer_time); 
    templete.timer.data    = (unsigned long)&templete;
    add_timer(&templete.timer);
    return 0;
}





static void __exit gpioled_exit(void){
    gpio_set_value(templete.gpio_led,1);
    del_timer(&templete.timer);
    cdev_del(&templete.cdev);
    unregister_chrdev_region(templete.devid , GPIOLED_CNT);
    device_destroy(templete.class,templete.devid);
    class_destroy(templete.class);
    gpio_free(templete.gpio_led);
}

module_init(gpioled_init);
module_exit(gpioled_exit);
MODULE_LICENSE("GPL");



