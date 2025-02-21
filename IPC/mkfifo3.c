/*有名管道的双向通信*/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<signal.h>

#define FIFONAME1 "./fifo1"
#define FIFONAME2 "./fifo2"

void signal_fun(int signo)
{
	remove(FIFONAME1);
	exit(-1);
}



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
	int fd1 = -1;
	int fd2 = -1;

	int ret = -1;

	char buf[10] = {0};

	fd1 = create_open_fifo(FIFONAME1, O_WRONLY);
	fd2 = create_open_fifo(FIFONAME2, O_RDONLY);
	
	ret = fork();

	if(ret > 0)
	{
		signal(SIGINT, signal_fun);
		while(1)
		{	
			bzero(buf, sizeof(buf));
			printf("transmit:");
			scanf("%s", buf);
			write(fd1, buf, sizeof(buf));
		}
	}
	if(ret == 0)
        {
                while(1)
                {    
                        bzero(buf, sizeof(buf));
                        read(fd2, buf, sizeof(buf));
			printf("\nreceive:%s\n", buf);
			printf("transmit:");
			fflush(stdout);
  			                 
                }
        }
	
	return 0;
}
