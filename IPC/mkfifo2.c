/*有名管道的单向通信的读端*/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFONAME "./fifo"

void print_err(char *estr)
{
	perror(estr);
	exit(-1);
}

int create_open_fifo(char *fifoname, int mode)
{
	int fd = -1;

	int ret = -1;

	ret = mkfifo(fifoname, 0664);

	if(ret == -1 && errno != EEXIST ) print_err("create failed");

	fd = open(fifoname, mode);

	if(fd == -1) print_err("open failed");

	return fd;
}

int main(void)
{
	int fd = -1;
	
	char buf[10] = {0};

	fd = create_open_fifo(FIFONAME, O_RDONLY);
	
	while(1)
	{	
		bzero(buf, sizeof(buf));
		read(fd, buf, sizeof(buf));
		printf("receive:%s\n", buf);
	}
	
	return 0;
}
