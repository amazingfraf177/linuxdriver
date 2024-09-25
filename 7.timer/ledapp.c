#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <linux/ioctl.h>



#define CLOSE_CMD      (_IO(0XEF,1))
#define OPEN_CMD       (_IO(0XEF,2))
#define SETPERIOD_CMD  (_IOW(0XEF,3,int))


int main (int argc, char * argv []) {
    int fd,ret = 0;
    unsigned int cmd;
    unsigned int arg;
    unsigned char str [100];


    if(argc !=2) {
        printf("usage\r\n");
    }

    char * filename = argv[1];


    fd = open(filename,O_RDWR);

    if (fd<0) {
        printf("the open was faild\r\n");
    }

    while(1) {
        printf("input CMD");
        ret = scanf("%d",&cmd);
        if(ret!=1) {
            gets(str);
        }
        if (cmd ==1) {
            cmd = CLOSE_CMD;
        }
        else if (cmd == 2) {
            cmd = OPEN_CMD;
        }
        else if (cmd == 3) {
            // ioctl(fd, SETPERIOD_CMD, &arg);
        }
        ioctl(fd, cmd, arg);
    }
    close(fd);




}
















