#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(int arg,char *argv[])
{
int fd,closed;
char read_buf[1];
char buf[5]={0x21,0x01,0x55,0x77,0x21};
fd = open(argv[1], O_RDWR);
 if(fd == -1){
        printf("Couldn't open file: %s\r\n", strerror(errno));
        return -1;
    }
else
    printf("file opened\r\n");

//read(fd,read_buf,1);
//sleep(10);
//printf("%d has been readen to rpi from psoc",buf[0]);

write(fd,buf,5);

closed=close(fd);
if(closed== -1){
        printf("Couldn't close file: %s\r\n", strerror(errno));
        return -1;
    }
    else
        printf("file closed\r\n");
        

}
