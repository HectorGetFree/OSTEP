/* 
 * 编写一个调用fork()的程序。在调用fork()之前，让主进程访问一个变量（例如x）并将其值设置为某个值（例如100）。
 * 子进程中的变量有什么值？当子进程和父进程都改变x的值时，变量会发生什么？
*/
// 基本IO
#include <stdio.h>
// 提供通用工具函数，包括 exit()、malloc()、strtol() 等
#include <stdlib.h>
// Unix/Linux 系统调用函数的声明，如 fork()、execvp()、getpid()。
#include <unistd.h>

int main(int argc, char *argv[]) {
    int x = 100;
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        x += 1;
        printf("in child x = %d\n", x);
    } else {
        x += 2;
        printf("in parent x = %d\n", x);
    }
    return 0;
}
// 运行结果
/*
 * in parent x = 102
 * in child x = 101
*/
