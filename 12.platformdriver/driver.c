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
#include <linux/fcntl.h>
#include <linux/platform_device.h>
#include <asm/mach/map.h>


static int driver_probe (struct platform_device * device) {
    printk("led device probe");
    return 0;
}

static int driver_remove (struct platform_device * device) {
    printk("led driver remove");
    return 0;
}

struct of_device_id led[] = {
    {.compatible =  "alientek_gpio"},
    { /* sentinel */ },
};


struct platform_driver driver = {
    .driver = {
        .name           = "imx6ull_led",
        .of_match_table = led,
    },
    .probe  = driver_probe,
    .remove = driver_remove, 
};



static int templete_init(void)
{
    platform_driver_register(&driver);
    return 0;   
}


static void templete_exit(void){
    platform_driver_unregister(&driver);
}

module_init(templete_init);
module_exit(templete_exit);
MODULE_LICENSE("GPL");



