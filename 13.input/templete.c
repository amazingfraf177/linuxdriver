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
#include <linux/input.h>

#define GPIOLED_CNT    1
#define GPIOLED_NAME  "KEY_INPUT" 
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
    struct device_node * nd;
    struct t_key key[KEY_NUM];
    struct timer_list timer;
    struct input_dev * inputdev;
};

struct templete templete;

static void timer_func(unsigned long arg) {
    struct templete *templete = (struct templete*) arg;
    int value = 0;
    value = gpio_get_value(templete->key[0].gpio);
    if (value==0) {
        input_event(templete->inputdev,EV_KEY,KEY_0,1);   
        input_sync(templete->inputdev);    
    }
    else if (value ==1) {
        input_event(templete->inputdev,EV_KEY,KEY_0,0); 
        input_sync(templete->inputdev);
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

    init_timer(&save->timer);
    save->timer.function = timer_func; 

    return 0;
}




static int __init gpioled_init(void)
{

    t_key_init(&templete);
    templete.inputdev = input_allocate_device();
    templete.inputdev->name = GPIOLED_NAME; 
    __set_bit(EV_KEY,templete.inputdev->evbit);
    __set_bit(EV_REP,templete.inputdev->evbit);
    __set_bit(KEY_0,templete.inputdev->keybit);

    input_register_device(templete.inputdev);

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
    input_unregister_device(templete.inputdev);
    input_free_device(templete.inputdev);
}

module_init(gpioled_init);
module_exit(gpioled_exit);
MODULE_LICENSE("GPL");



