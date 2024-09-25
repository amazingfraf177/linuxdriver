#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/uaccess.h>


#define devbase_major 200
#define devbase_name "devbase"

static char readbuf [100];
static char writebuf [100];
static char kerneldata[] = {"hello i am frank!/r/n"}; 


static int devbase_open (struct inode * inode, struct file * filp) {
    printk("devbase_open\r\n");
    return 0;
}

static int devbase_release (struct inode * inode, struct file * filp) {
    printk("devbase_release\r\n");
    return 0;
}


static ssize_t devbase_read (struct file * filp , char __user * buf, size_t count, loff_t * ppos) {
    int ret = 0;
    memcpy(readbuf,kerneldata,sizeof(kerneldata));
    ret = copy_to_user(buf,readbuf,count);
    if (ret == 0 ){

    }
    else {

    }
    return 0;
} 

static ssize_t  devbase_write (struct file * filp , const char __user * buf, size_t count, loff_t * ppos) {
    int valid = 0;
    valid = copy_from_user(writebuf,buf,count);
    if(valid == 0 ){
        printk("kernel receive  %s\r\n",writebuf);
    }
    else {
        printk("write panic");
    }
    return 0;
} 


static struct file_operations devbase_file = {
    .owner = THIS_MODULE,
    .open = devbase_open,
    .release = devbase_release,
    .read =  devbase_read,
    .write =  devbase_write,
};

static int __init devbase_init(void) 
{ 
    int ret;
    printk("devbase_init\r\n");
    ret =  register_chrdev(devbase_major,devbase_name,&devbase_file);
    if(ret<0) {
        printk("panic : regisiter_chrdev");
    }
    return 0; 
} 

static void __exit devbase_exit(void)  
{ 
    printk("devbase_exit\r\n");
    unregister_chrdev(devbase_major,devbase_name);
} 

module_init(devbase_init);
module_exit(devbase_exit);
MODULE_LICENSE("GPL");
