#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#define led_major 200
#define led_name  "led"

#define CCM_CCGR1_BASE           (0X020C406C) 
#define SW_MUX_GPIO1_IO03_BASE   (0X020E0068) 
#define SW_PAD_GPIO1_IO03_BASE   (0X020E02F4) 
#define GPIO1_DR_BASE            (0X0209C000) 
#define GPIO1_GDIR_BASE          (0X0209C004)
 
static void __iomem  * VI_CCM_CCGR1_BASE;
static void __iomem  * VI_SW_MUX_GPIO1_IO03_BASE;
static void __iomem  * VI_SW_PAD_GPIO1_IO03_BASE;
static void __iomem  * VI_GPIO1_DR_BASE;
static void __iomem  * VI_GPIO1_GDIR_BASE;


#define led_on  1
#define led_off 0

u32 writebuf[1];


static int led_open (struct inode * inode, struct file * filp) {
    return 0;
}

static int led_release (struct inode * inode, struct file * filp) {
    return 0;
}

static ssize_t  led_write (struct file * filp , const char __user * buf, size_t count, loff_t * ppos) {
    int ret = 0;
    ret = copy_from_user(buf[0],buf,count);
    if (ret<0) {
        printk("panic write faild");
    }
    return 0;
} 


static struct file_operations led_file = {
    .owner = THIS_MODULE,
    .open = led_open,
    .release = led_release,
    .write =  led_write,
};




static int __init led_init(void){
    int ret = 0;
    int val = 0;
    VI_CCM_CCGR1_BASE         = ioremap(CCM_CCGR1_BASE,4);
    VI_SW_MUX_GPIO1_IO03_BASE = ioremap(SW_MUX_GPIO1_IO03_BASE,4);
    VI_SW_PAD_GPIO1_IO03_BASE = ioremap(SW_PAD_GPIO1_IO03_BASE,4);
    VI_GPIO1_DR_BASE          = ioremap(GPIO1_DR_BASE,4);
    VI_GPIO1_GDIR_BASE        = ioremap(GPIO1_GDIR_BASE,4);
    
    // initial
    val = readl(VI_CCM_CCGR1_BASE);
    val &= ~(3<<26);
    val |=  (3<<26);
    writel(val,VI_CCM_CCGR1_BASE);

    writel(0x5 , VI_SW_MUX_GPIO1_IO03_BASE);
    writel(0x10B0 , VI_SW_PAD_GPIO1_IO03_BASE);

    val = readl(VI_GPIO1_GDIR_BASE);
    val |=  (1<<3);
    writel(val , VI_GPIO1_GDIR_BASE);

    val = readl(VI_GPIO1_DR_BASE);
    val &=  ~(1<<3);
    writel(val,VI_GPIO1_DR_BASE);


    printk("led_init\r\n");
    ret =  register_chrdev(led_major,led_name,&led_file);
    if(ret<0) {
        printk("register faild");
        return -EIO;
    }
    return 0;
}

static void __exit led_exit(void){
    int val = 0;

    printk("led_exit\r\n");

    val = readl(VI_GPIO1_DR_BASE);
    val |=  1<<3;
    writel(val,VI_GPIO1_DR_BASE);


    iounmap(VI_CCM_CCGR1_BASE);
    iounmap(VI_SW_MUX_GPIO1_IO03_BASE);
    iounmap(VI_SW_PAD_GPIO1_IO03_BASE);
    iounmap(VI_GPIO1_DR_BASE);
    iounmap(VI_GPIO1_GDIR_BASE);

    unregister_chrdev(led_major,led_name);
    return;
}


module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");