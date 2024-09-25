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
#include <asm/mach/map.h> 
#include <asm/uaccess.h> 
#include <asm/io.h>

// backlight {
// 		compatible = "pwm-backlight";
// 		pwms = <&pwm1 0 5000000>;
// 		brightness-levels = <0 4 8 16 32 64 128 255>;
// 		default-brightness-level = <6>;
// 		status = "okay";
// 	};




static int __init of_init(void) {
    int ret = 0;
    struct device_node * nd = NULL;
    struct property    * pro = NULL; 
    nd = of_find_node_by_path("/backlight");
    if (nd==NULL) {
        ret = -EINVAL;
        goto fail;
    }

    pro = of_find_property(nd,"compatible",NULL);
    if(nd==NULL) {
        ret = -EINVAL;
        goto fail;
    }
    else {
        printk("compatible = %s/r/n",(char *) pro->value);
    }


    return 0;

fail:
    return ret;
}

static void __exit of_exit(void) {
    return;
}

module_init(of_init);
module_exit(of_exit);
MODULE_LICENSE("GPL");

