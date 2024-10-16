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
#include <linux/i2c.h>
#include <linux/delay.h>
#include "ap3216c.h"


#define AP3216C_CNT   1
#define AP3216C_NAME "ap3216c"


struct ap3216c_dev {       // char dev structure
    int major;
    int minor;
    dev_t devid;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    void * private_data;
};

static struct ap3216c_dev ap3216cdev;
/////////////////////////////////////////////////////////////////////////////ap3216c_i2c read and write func

static int ap3216c_read_regs(struct ap3216c_dev *dev, u8 regs, void *val,int len)  
{
    struct i2c_client * client = (struct i2c_client *)dev->private_data;

    struct i2c_msg msg[2];

    msg[0].addr = client->addr;
    msg[0].flags = 0;
    msg[0].buf = &regs;
    msg[0].len = 1;

    msg[1].addr = client->addr;
    msg[1].flags = I2C_M_RD;
    msg[1].buf = val;
    msg[1].len = len;

    int i = i2c_transfer(client->adapter,msg,2);
    printk("ri=%d\r\n",i);
    return 0;
} 

static int ap3216c_write_regs(struct ap3216c_dev * dev, u8 reg, u8 *buf,int len)
{
    struct i2c_client * client = (struct i2c_client *)dev->private_data;
    struct i2c_msg  msg;
    u8 b[256];
    b[0] = reg;
    memcpy(&b[1],buf,len);
    msg.addr = client->addr;
    msg.flags = 0;
    msg.buf = b;
    msg.len = len+1;
    int i = i2c_transfer(client->adapter,&msg,1);
    printk("wi=%d\r\n",i);
    return 0;
}

static unsigned char ap3216c_read_reg(struct ap3216c_dev * dev,u8 reg) 
{
    u8 data = 0;
    ap3216c_read_regs(dev,reg,&data,1);
    return data;
}

static void ap3216c_write_reg(struct ap3216c_dev * dev,u8 reg,u8 data) 
{
    ap3216c_write_regs(dev,reg,&data,1);
}


/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////  system call driver
static int ap3216c_open (struct inode * inode,struct file * file) 
{

    unsigned char value = 0;
    file->private_data = &ap3216cdev;
    ap3216c_write_reg(&ap3216cdev,AP3216C_SYSTEMCONG,0x04);
    mdelay(50);
    ap3216c_write_reg(&ap3216cdev,AP3216C_SYSTEMCONG,0x03);
    value = ap3216c_read_reg(&ap3216cdev,AP3216C_SYSTEMCONG);
    printk("AP3216C_SYSTEMCONG:=%#x\r\n",value);
    return 0;
}

ssize_t ap3216c_read (struct file * file, char __user *buf,size_t cnt,loff_t *off) 
{
    printk("read success");
    return 0;
}


static int ap3216c_release (struct inode * inode,struct file * file) 
{
    struct ap3216_cdev * dev = (struct ap3216_cdev *)file->private_data;
    printk("release success");
    return 0;
}





/////////////////////////////////////////////////////////////

static const struct file_operations ap3216c_fops = {
    .owner = THIS_MODULE,
    .open = ap3216c_open,
    .release = ap3216c_release,
    .read = ap3216c_read,
};



static int ap3216c_probe(struct i2c_client * client, const struct i2c_device_id *id)
{
    int ret = 0;
    printk("probe success\r\n");
    ap3216cdev.major = 0;                             // register device number
    if(ap3216cdev.major) {
        ap3216cdev.devid = MKDEV(ap3216cdev.major,0);
        ret = register_chrdev_region(ap3216cdev.devid,AP3216C_CNT,AP3216C_NAME);
    } else {
        ret = alloc_chrdev_region(&ap3216cdev.devid,0,AP3216C_CNT,AP3216C_NAME);
        ap3216cdev.major = MAJOR(ap3216cdev.devid);
        ap3216cdev.minor = MINOR(ap3216cdev.devid);
    }
    if(ret<0) {
        printk("Error in registering ap3216c's char device number\r\n");
        return 0;
    }
    printk("char_dev major=%d,minor=%d",ap3216cdev.major,ap3216cdev.minor);

    ap3216cdev.cdev.owner = THIS_MODULE;                 // register device
    cdev_init(&ap3216cdev.cdev,&ap3216c_fops);
    ret = cdev_add(&ap3216cdev.cdev,ap3216cdev.devid,AP3216C_CNT);
    if(ret<0) {
        printk("Error in registering ap3216c's char device");
    }
    ap3216cdev.class = class_create(THIS_MODULE,AP3216C_NAME);
    ap3216cdev.device = device_create(ap3216cdev.class,NULL,ap3216cdev.devid,NULL,AP3216C_NAME);

    ap3216cdev.private_data = client;

    return 0;
}

static int ap3216c_remove (struct i2c_client * client)
{
    cdev_del(&ap3216cdev.cdev);
    unregister_chrdev_region(ap3216cdev.devid,AP3216C_CNT);
    device_destroy(ap3216cdev.class,ap3216cdev.devid);
    class_destroy(ap3216cdev.class);
    return 0;
}


static struct i2c_device_id ap3216c_id[] = {                // The i2c_device_id is used to match the i2c device with an id in a
    {"alientek,ap3216c", 0},                                // traditional system, without a device tree.
    {}
};



static struct of_device_id ap3216c_match[]= {               //  The of_device_id is used to match device tree. 
    { .compatible = "alientek,ap3216c",},
    {}                                                    //   The last column in the array can be used for space.
};


static struct i2c_driver ap3216c_driver = {               //The i2c driver struct is like platform driver struct.                                  
    .probe =  ap3216c_probe,                                           //probe function
    .remove = ap3216c_remove,
    .driver = {                                           
        .owner = THIS_MODULE,
        .name = "ap3216c",
        .of_match_table = of_match_ptr(ap3216c_match),
    },
    .id_table = ap3216c_id,
};



static int __init ap3216c_init(void)
{
    int ret = 0;
    ret = i2c_add_driver(&ap3216c_driver);
    return 0;
}





static void __exit ap3216c_exit(void){
    i2c_del_driver(&ap3216c_driver);
}

module_init(ap3216c_init);
module_exit(ap3216c_exit);
MODULE_LICENSE("GPL");



