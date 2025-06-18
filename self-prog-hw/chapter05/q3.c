/*
 * 使用fork()编写另一个程序。
 * 子进程应打印“hello”，父进程应打印“goodbye”。
 * 你应该尝试确保子进程始终先打印。
 * 你能否不在父进程调用wait()而做到这一点呢？
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    // 创建匿名管道
    int p[2];
    pipe(p);
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        // 关闭读端
        close(p[1]);
        printf("hello\n");
        // 写一个字节通知父进程
        write(p[1], "x", 1);
        // 关闭写端
        close(p[1]);
    } else {
        // 关闭写端
        close(p[1]);
        // 读取阻塞
        char buf;
        read(p[0], &buf, 1);
        close(p[0]);
        printf("goodbye\n");
    }
    return 0;
}