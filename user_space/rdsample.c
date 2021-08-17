
#include<unistd.h>
#include<fcntl.h>

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define  MAX_LENGTH (128) // maximum values that can be stored inside the buf

int main(int argc, char** argv)
{
    int fd, nbytes;
    fd = open("/dev/psample", O_RDONLY); //returns negative number if fileopen is failed
    printf("Ret value of open = %d\n", fd);
    if(fd<0)
    {
        perror("open");
        exit(3);
    }
    char buf[MAX_LENGTH] = {'\0'};
    nbytes = read(fd, buf, MAX_LENGTH);
    if(nbytes < 0)
    {
        perror("read");
        exit(2);
    }
    printf("\n nbytes = %d, buf = %s\n", nbytes, buf);
    write(1, buf, nbytes);
    close(fd);
    return 0; //exit(0);
}
