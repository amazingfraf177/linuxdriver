#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <linux/ioctl.h>
#include <linux/input.h>

static struct input_event inputevent;

int main (int argc, char * argv []) {
    int fd,ret = 0;
    unsigned char data;

    if(argc !=2) {
        printf("usage\r\n");
    }

    char * filename = argv[1];


    fd = open(filename,O_RDWR);

    if (fd<0) {
        printf("the open was faild\r\n");
    }

    while(1) {
        ret =  read(fd,&inputevent,sizeof(inputevent));
        if (ret>0) {
            switch (inputevent.type)
            {
                case EV_KEY:
                    printf("key\r\n");
                case EV_SYN:
                    printf("SYN\r\n");
                default:
                    break;
            }
        }
    }
    close(fd);
}





