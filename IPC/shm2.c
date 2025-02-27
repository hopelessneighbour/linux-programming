//共享内存通信的读端
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <strings.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include<string.h>

#define FIFOFILE "./fifofile"
#define SHMFILE "./shmfile"
#define SHM_SIZE 4096

int shmid = -1;
void *shmaddr = NULL;
char buf[20] = "hello world";


void print_err(char* estr)
{
    perror(estr);
    exit(-1);
}

void signal_fun(int signo)
{
    if(signo == SIGINT)
    {
        //删除虚拟内存和共享内存的映射
        shmdt(shmaddr);
        //删除共享内存，但是要等到所有映射都被删除后才能执行
        shmctl(shmid, IPC_RMID, NULL);

        //删除文件
        remove(FIFOFILE);
        remove(SHMFILE);
        
        exit(-1);
    }
    
}

pid_t recv_peer_pid(void)
{
    int fd = -1;
    int ret = -1;
    pid_t pid = -1;

    //创建有名管道文件
    ret = mkfifo(FIFOFILE, 0664|IPC_CREAT);
    if(ret == -1&&errno != EEXIST) print_err("mkfifo fail");

    //打开管道文件
    fd = open(FIFOFILE, O_RDONLY);
    if(fd == -1) print_err("open fail");

    read(fd, &pid, sizeof(pid));
    return pid;
}

void get_or_create_shm(void)
{
    int fd = -1;
    key_t key = -1;

    fd = open(SHMFILE, O_RDWR|O_CREAT, 0664);
    if(fd == -1) print_err("open fail");

    key = ftok(SHMFILE, 'a');
    if(key == -1) print_err("ftok fail");

    shmid = shmget(key, SHM_SIZE, 0664|IPC_CREAT);
    if(shmid == -1) print_err("shmget fail");
}

int main(void)
{   
    pid_t peer_pid = -1;

    //注册信号捕获函数
    signal(SIGINT, signal_fun);

    //获取或创建一个共享内存空间 赋值共享内存id
    get_or_create_shm();

    //接收进程的pid
    peer_pid = recv_peer_pid();

    //映射虚拟地址到共享内存
    shmaddr = shmat(shmid, NULL, 0);
    if(shmaddr ==  (void*)-1) print_err("shmat fail");

    while(1)
    {
        memcpy(shmaddr,buf, sizeof(buf));
        kill(peer_pid, SIGUSR1);
        sleep(1);
    }
    
    return 0;
}
