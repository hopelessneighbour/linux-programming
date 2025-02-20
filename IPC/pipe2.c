/*无名管道的双向通信*/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

void print_err(char *estr)
{
	perror(estr);
	exit(-1);
}
int main(void)
{
	int pipefd1[2] = {0};
	int pipefd2[2] = {0};
	bzero(pipefd1, sizeof(pipefd1));
	bzero(pipefd2, sizeof(pipefd2));

	int ret = 0;

	ret = pipe(pipefd1) + pipe(pipefd2);
	if(ret != 0)
	{
		print_err("pipe failed");
	}

	ret = fork();

	if(ret == 0)
	{
		close(pipefd1[1]);
		close(pipefd2[0]);
		char buf[30] = {0};
		bzero(buf, sizeof(buf));
		
		while(1)
		{       write(pipefd2[1], "world", 5); //子进程和父进程不能同时先读再写，不然会造成死锁（read函数是阻塞的）
			sleep(1);
			read(pipefd1[0], buf, sizeof(buf));
			printf("child receive: %s\n", buf);
		}

	}
	else
	{
		close(pipefd1[0]);
		close(pipefd2[1]);
		char buf[30] = {0};
		bzero(buf, sizeof(buf));
		while(1)
		{	
			read(pipefd2[0], buf, sizeof(buf));
			printf("parent receive: %s\n", buf);
			sleep(1);
			write(pipefd1[1], "hello", 5);
		}
	}
	return 0;
}
