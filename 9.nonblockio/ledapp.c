#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <linux/ioctl.h>
#include <linux/wait.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>



int main (int argc, char * argv []) {
    int fd,ret = 0;
    unsigned char data;
    fd_set readfds;
    struct timeval timeout;

    if(argc !=2) {
        printf("usage\r\n");
    }

    char * filename = argv[1];


    fd = open(filename,O_RDWR | O_NONBLOCK);

    if (fd<0) {
        printf("the open was faild\r\n");
    }

    while(1) {
        FD_ZERO(&readfds);
        FD_SET(fd,&readfds);
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;
        int ret = select(fd+1,&readfds,NULL,NULL,&timeout);
        switch(ret) {
            case 0:
                break;
            case -1:
                break;
            default:
                if(FD_ISSET(fd,&readfds)){
                    ret =  read(fd,&data,sizeof(data));
                    if (ret<0) {

                    }
                    else {
                        printf("key value = %#x\r\n",data);
                    }
                }


        }
    }
    close(fd);
}





