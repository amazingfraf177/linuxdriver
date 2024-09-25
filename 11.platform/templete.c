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

#define CCM_CCGR1_BASE         (0X020C406C)
#define SW_MUX_GPIO1_IO03_BASE (0X020E0068)
#define SW_PAD_GPIO1_IO03_BASE (0X020E02F4)
#define GPIO1_DR_BASE          (0X0209C000)
#define GPIO1_GDIR_BASE        (0X0209C004)

#define REGISTER_LENGTH 4


void led_device_release(struct device * device) {
    printk("templetled is released");
}


static struct resource templete_resource [] =  {
    [0] = {
        .start = CCM_CCGR1_BASE,
        .end = (CCM_CCGR1_BASE + REGISTER_LENGTH -1),
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = SW_MUX_GPIO1_IO03_BASE,
        .end = (SW_MUX_GPIO1_IO03_BASE + REGISTER_LENGTH -1),
        .flags = IORESOURCE_MEM,
    },
    [2] = {
        .start = SW_PAD_GPIO1_IO03_BASE,
        .end = (SW_PAD_GPIO1_IO03_BASE + REGISTER_LENGTH -1),
        .flags = IORESOURCE_MEM,
    },
    [3] = {
        .start = GPIO1_DR_BASE,
        .end = (GPIO1_DR_BASE + REGISTER_LENGTH -1),
        .flags = IORESOURCE_MEM,
    },
    [4] = {
        .start = GPIO1_GDIR_BASE,
        .end = (GPIO1_GDIR_BASE + REGISTER_LENGTH -1),
        .flags = IORESOURCE_MEM,
    },
};


static struct platform_device templete = {
    .name = "templeteled",
    .id = -1,
    .dev = {
        .release = &led_device_release, 
    },
    .num_resources = ARRAY_SIZE(templete_resource),
    .resource = templete_resource,
}; 


static int __init templete_init(void)
{
    return platform_device_register(&templete);
}


static void __exit templete_exit(void){
    platform_device_unregister(&templete);
}

module_init(templete_init);
module_exit(templete_exit);
MODULE_LICENSE("GPL");



