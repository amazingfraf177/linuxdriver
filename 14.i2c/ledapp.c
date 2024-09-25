#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int main (int argc, char * argv []) {
    int fd = 0;
    int val = 0;
    if(argc !=3) {
        printf("usage\r\n");
    }

    const char * filename = argv[1];


    fd = open(filename,O_RDWR);

    if (fd<0) {
        printf("the open was faild\r\n");
    }


    unsigned char save[1];

    save[0] = atoi(argv[2]);

    val = write(fd,save,sizeof(save));

    if (val < 0) {
        printf ("write was panic \r\n");
    }

    close(fd);




}
























// int main (int argc,char * argv []){
//     int fd = 0;
//     char * filename = argv[1];

//     int valid = 0;
//     char *  readbuf[100];
//     char *  writebuf[100];
//     static char userdata [] = {"writedata from user"};
//    // openfile 

//     fd = open(filename, O_RDWR);
//     if (fd<0) {
//         printf ("can't open %s\r\n",filename);
//         return -1;
//     }

//     if (argc != 3) {
//         printf ("panic! devbaseapp.c only receive 3 parameter");
//     }

     
//     if (atoi(argv[2])==1){
//          //readfile
//         valid = read(fd,readbuf,50);
//         if(valid<0){
//             printf("the %s is faild to read",filename);
//             return -1;
//         }
//         else {
//             printf ("the kernel data %s\r\n",readbuf);
//         }
//     }

//     if(atoi(argv[2])==2){
//         memcpy(writebuf,userdata,sizeof(userdata));
//         valid = write(fd,writebuf,50);
//         if(valid <0) {
//             printf("the %s is faild to write",filename);
//         return -1;
//         }
//         else {

//         }
//     }

//     valid = close(fd);
//     if(valid <0) {
//         printf("the %s is faild to close",filename);
//         return -1;
//     }
//     return 0;
// }