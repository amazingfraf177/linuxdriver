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
#include <linux/uaccess.h>
#include <asm/io.h>
#include <linux/atomic.h>
#include <linux/wait.h>
#include <linux/poll.h>


#define GPIOLED_CNT    1
#define GPIOLED_NAME  "KEY" 
#define KEY_NUM        1
#define KEY0VALUE      0X01
#define INVALUE        0XFF

struct t_key {
    int gpio;
    int irqnum;
    unsigned char value;
    char name[10];
    irqreturn_t (*handler) (int,void *);
};

struct templete {
    dev_t devid;
    int     major;
    int     minor;
    struct cdev    cdev;
    struct class  *class;
    struct device *device;
    struct device_node * nd;
    struct t_key key[KEY_NUM];
    struct timer_list timer;

    atomic_t keyvalue;
    atomic_t keyrelease;
    wait_queue_head_t r_wait;
};

struct templete templete;

static int templete_open(struct inode * inode,struct file * file){
    file->private_data = &templete;
    return 0;
}

static int templete_release(struct inode * inode,struct file * file){

    return 0;
} 

static ssize_t templete_write(struct file * file,const char __user * buf,size_t count,loff_t * ppos) {
    return 0;
}



static ssize_t templete_read (struct file * filp , char __user * buf, size_t count, loff_t * ppos) {
    int ret = 0;
    unsigned char keyvalue;
    unsigned char releasekey;
    struct templete * save = (struct templete *) filp->private_data;

    if (filp->f_flags & O_NONBLOCK) {
        if(atomic_read(&save->keyrelease)==0){
            return -EAGAIN;
        }
    }
    // wait_event(save->r_wait,atomic_read(&save->keyrelease));

    keyvalue = atomic_read(&save->keyvalue);
    releasekey = atomic_read(&save->keyrelease);

    if(releasekey) {
        if(keyvalue & 0X80){
            keyvalue &= ~0X80;
            ret = copy_to_user(buf,&keyvalue,sizeof(keyvalue));
        }
        atomic_set(&save->keyrelease,0); 
    }
    else {
        return -EINVAL;
    }    
    return 0;
} 

static unsigned int templete_poll (struct file * filp,struct poll_table_struct * wait){
    int mask = 0;
    struct templete * save = (struct templete * ) filp->private_data;
    if(atomic_read(&save->keyrelease)){
        mask = POLLIN | POLLRDNORM;
    }
    return mask;
}



static const struct file_operations gpioled_fop = {
    .owner = THIS_MODULE,
    .write = templete_write,
    .open = templete_open,
    .release = templete_release,
    .read = templete_read,
    .poll = templete_poll,
};

static void timer_func(unsigned long arg) {
    struct templete *templete = (struct templete*) arg;
    int value = 0;
    value = gpio_get_value(templete->key[0].gpio);
    if (value==0) {
        atomic_set(&templete->keyvalue,templete->key[0].value);
    }
    else if (value ==1) {
        atomic_set(&templete->keyvalue,0X80 | (templete->key[0].value));
        atomic_set(&templete->keyrelease,1);

    }

    if(&templete->keyrelease){
        wake_up(&templete->r_wait);
    }
}


static irqreturn_t key0_handle (int irq, void * savefunc) {

    struct templete * save = savefunc;
    save->timer.data = (unsigned long) save;
    mod_timer(&save->timer,jiffies + msecs_to_jiffies(10));


    // int value = 0;
    // struct templete * save = savefunc;
    // value = gpio_get_value(save->key[0].gpio);
    // if(value==0) {
    //     printk("the button is push");
    // } 
    // else if (value==1) {
    //     printk("the button is release");
    return IRQ_HANDLED;
}




static int t_key_init(struct templete * save) {
    int i =0;
    save->nd = of_find_node_by_path("/key");

    for (i=0; i<KEY_NUM;i++) {
        save->key[i].gpio = of_get_named_gpio(save->nd,"key-gpios",i);
    }

    for (i=0; i<KEY_NUM;i++) {
        memset(save->key[i].name,0,sizeof(save->key[i].name));
        sprintf(save->key[i].name,"key%d",i);
        gpio_request(save->key[i].gpio,save->key[i].name);
        gpio_direction_input(save->key[i].gpio);
        save->key[i].irqnum = gpio_to_irq(save->key[i].gpio);
    }

    save->key[0].handler = key0_handle;
    save->key[0].value   = KEY0VALUE;
    for (i = 0;i<KEY_NUM;i++) {
        request_irq(save->key[i].irqnum,save->key[i].handler,IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,save->key[i].name,save);
    }

    return 0;
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


    init_timer(&templete.timer);
    templete.timer.function = timer_func; 
    atomic_set(&templete.keyvalue,INVALUE);
    atomic_set(&templete.keyrelease,0);
    t_key_init(&templete);

    init_waitqueue_head(&templete.r_wait);
    return 0;
}





static void __exit gpioled_exit(void){
    int i = 0;
    for (i=0;i<KEY_NUM;i++) {
        free_irq(templete.key[i].irqnum,&templete);
    }
    for (i=0;i<KEY_NUM;i++) {
        gpio_free(templete.key[i].gpio);
    }

    del_timer_sync(&templete.timer);
    cdev_del(&templete.cdev);
    unregister_chrdev_region(templete.devid , GPIOLED_CNT);
    device_destroy(templete.class,templete.devid);
    class_destroy(templete.class);
}

module_init(gpioled_init);
module_exit(gpioled_exit);
MODULE_LICENSE("GPL");



