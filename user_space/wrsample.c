
#include<unistd.h>
#include<fcntl.h>

#include<stdio.h>
#include<stdlib.h>
#include<string.h>


int main()
{
    int fd, nbytes;
    fd = open("/dev/psample0", O_WRONLY); //returns negative number if fileopen is failed
    if(fd<0)
    {
        perror("open");
        exit(1);
    }
    char str[] = "Hello World\n";
    int len = strlen(str);
    nbytes = write(fd, str, len);
    if(nbytes < 0)
    {
        perror("write");
        exit(2);
    }
    printf("\nwritten successfully, nbytes = %d\n", nbytes);
    close(fd);
    return 0; //exit(0);
}
