#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include<signal.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PATH "./msgfile"
#define MSG_SIZE 10
struct msgbuf
{
	long mtype;
	char mtext[MSG_SIZE];
};

int msgid = -1;//将消息队列号设置为全局变量

void print_err(char *estr)
{
        perror(estr);
        exit(-1);
}


int create_or_get_msgque(void)
{
	int fd = -1;
	int ret = -1;

	fd = open(PATH, O_RDWR|O_CREAT, 0664);
	if(fd == -1) print_err("open fail");

	key_t key = ftok(PATH, 'a');

	ret = msgget(key, 0664|IPC_CREAT);
	if(ret == -1) print_err("create fail");

	return ret;
}


void signal_fun(int signo)
{
	remove(PATH);
	msgctl(msgid, IPC_RMID, NULL);
	exit(-1);
}
int main(int argv, char **argc)
{	
	if(argv != 2)
	{
		printf("%s recv_type\n", argc[0]);
		exit(-1);
	}

	int ret = -1;

	msgid = create_or_get_msgque();
	
	long recv_type = atol(argc[1]);

	ret = fork();
	
	if(ret > 0)
	{
		signal(SIGINT, signal_fun);
		struct msgbuf msg_buf = {0};
		while(1)
		{
			bzero(&msg_buf, sizeof(msg_buf));
			printf("enter mtype\n");
			scanf("%ld", &msg_buf.mtype);
			printf("enter mtext\n");
			scanf("%s", msg_buf.mtext);
			int i = msgsnd(msgid, &msg_buf, MSG_SIZE, 0);
		       if(i != 0)
		       		printf("fail");	       
		}
	}
	else
	{
		struct msgbuf msg_buf = {0};
		while(1)
		{
			bzero(&msg_buf, sizeof(msg_buf));
			msgrcv(msgid, &msg_buf, MSG_SIZE, recv_type, 0);
			printf("recv:%s\n", msg_buf.mtext);
		}
	}

	return 0;
}
