#include "apue.h"
#include "time.h"
#include "lock.h"
#define N 5
static char* forks[N] = {"fork0", "fork1", "fork2", "fork3", "fork4"};
static int nsecs = 2;


/*
 * 拿叉子的定义
 * 如果哲学家中同时存在左撇子和右撇子，则哲学家问题有解
 */
void takeFork(int i) {
    if(i == N-1) { // 人为设定第N-1位哲学家是右撇子
        lock(forks[0]);
        lock(forks[i]);
    } else { // 其他是左撇子
        lock(forks[i]);
        lock(forks[i+1]);
    }
}

/* 放下叉子 */
void putFork(int i) {
    if(i == N-1) {
        unlock(forks[0]);
        unlock(forks[i]);
    }
    else {
        unlock(forks[i]);
        unlock(forks[i+1]);
    }
}

void thinking(int i, int nsecs) {
    printf("philosopher %d is thinking\n", i);
    sleep(nsecs);
}

void eating(int i, int nsecs) {
    printf("philosopher %d is eating\n", i);
    sleep(nsecs);
}

/* 哲学家行为 */
void philosopher(int i) {
    while(1) {
        thinking(i, nsecs);   // 哲学家i思考nsecs秒
        takeFork(i);      // 哲学家i拿起叉子
        eating(i, nsecs);   // 哲学家i进餐nsecs秒
        putFork(i);       // 哲学家i放下叉子
    }
}

int main(int argc, char * argv[]){
    int i;
    pid_t pid;

    /* 初始化叉子 */
    for(i = 0; i < N; i++) {
        initlock(forks[i]);
    }

    /* 处理输入 */
    if(argc == 3 && strcmp(argv[1], "-t") == 0) {
        nsecs = atoi(argv[2]);
        // if (!nsecs) err_quit("usage: philosopher [ -t <time> ]");
    } else if (argc != 1) {
        err_quit("usage: philosopher [ -t <time> ]");
    }

    /* 创建五个子进程来模拟五个哲学家 */
    for(i = 0; i < N; i++) {
        pid = fork();
        if (pid == 0) {
            philosopher(i);
        } else if (pid < 0) {
            err_quit("fork error");
        }
    }

    wait(NULL); /* 注意，如果父进程不等待子进程的结束，*/
    /* 那么需要终止程序运行时，就只能从控制台删除在后台运行的哲学家进程 */
}