/*无名管道的单向通信*/
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
	int pipefd[2] = {0};
	bzero(pipefd, sizeof(pipefd));

	int ret = 0;

	ret = pipe(pipefd);
	
	if(ret == -1)
	{
		print_err("pipe failed");
	}

	ret = fork();

	if(ret == 0)
	{
		close(pipefd[1]);
		char buf[30] = {0};
		bzero(buf, sizeof(buf));
		
		while(1)
		{
			read(pipefd[0], buf, sizeof(buf));
			printf("child receive: %s\n", buf);
		}

	}
	else
	{
		close(pipefd[0]);

		while(1)
		{
			write(pipefd[1], "hello", 5);
			sleep(1);
		}
	}
	return 0;
}
