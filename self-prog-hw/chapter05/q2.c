/*
 * 编写一个打开文件的程序（使用open()系统调用），然后调用fork()创建一个新进程。
 * 子进程和父进程都可以访问open()返回的文件描述符吗？
 * 当它们并发（即同时）写入文件时，会发生什么？-- 不会覆盖，而是追加
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    close(STDOUT_FILENO);
    int fd = open("./q2.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        printf("child can access fd: %d\n", fd);
    } else {
        printf("parent can accsee fd: %d\n", fd);
    }
    return 0;
}
// 无重定向命令行运行结果
/*
 * parent can accsee fd: 3
 * child can access fd: 3
*/
// 重定向结果
/*
 * parent can accsee fd: 1
 * child can access fd: 1
*/

