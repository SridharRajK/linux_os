#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/types.h>
#include<sys/ioctl.h>
#include<sys/stat.h>

struct pseudo_stat{
	int len;
	int avail;
};

#define IOC_MAGIC 'p'
#define MY_IOCTL_LEN 		_IO(IOC_MAGIC,1)
#define MY_IOCTL_AVAIL		_IO(IOC_MAGIC,2)
#define MY_IOCTL_RESET		_IO(IOC_MAGIC,3)
#define MY_IOCTL_PSTAT		_IOR(IOC_MAGIC, 4, struct pseudo_stat)

int main(int argc, char *argv[])
{

    int ret,fd,len,nbytes,maxlen;
    struct pseudo_stat stat;
    
    fd=open("/dev/psample0",O_RDONLY);
    printf("Return value for fd is %d\n",fd);
    if(fd<0){
        perror("Open error");
        exit(1);
    }
    ret=ioctl(fd, MY_IOCTL_PSTAT, &stat);
    if(ret<0){
        perror("ioctl");
        exit(4);
    }
	
	printf("Stat length: %d \nStat avail: %d\n", stat.len, stat.avail);
    close(fd);

    return 0;

}
