#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/types.h>
#include<sys/ioctl.h>
#include<sys/stat.h>

#define IOC_MAGIC 'p'
#define MY_IOCTL_LEN 		_IO(IOC_MAGIC,1)
#define MY_IOCTL_AVAIL		_IO(IOC_MAGIC,2)
#define MY_IOCTL_RESET		_IO(IOC_MAGIC,3)

int main(int argc, char *argv[])
{

    int ret,fd,len,nbytes,maxlen;
    fd=open("/dev/psample0",O_RDONLY);
    printf("Return value for fd is %d\n",fd);
    if(fd<0){
        perror("Open error");
        exit(1);
    }
    close(fd);

    return 0;

}
