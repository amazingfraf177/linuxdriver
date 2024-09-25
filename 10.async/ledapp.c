#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <linux/ioctl.h>
#include <signal.h>

int fd;

static void signal_func(int num){
    int err;
    unsigned int keyvalue=0;

    err = read(fd,&keyvalue,sizeof(keyvalue));
    if(err<0) {
        printf("error");
    }
    else {
        printf("the key value is %d",keyvalue);
    }

}



int main (int argc, char * argv []) {
    int ret = 0;
    unsigned char data;

    if(argc !=2) {
        printf("usage\r\n");
    }

    char * filename = argv[1];


    fd = open(filename,O_RDWR);

    if (fd<0) {
        printf("the open was faild\r\n");
    }

    signal(SIGIO,signal_func);

    fcntl(fd, F_SETOWN, getpid()); 
    ret = fcntl(fd, F_GETFD); 
    fcntl(fd, F_SETFL, ret | FASYNC);


    while(1);
    close(fd);
}





