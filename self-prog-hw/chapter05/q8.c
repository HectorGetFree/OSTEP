/*
 * 编写一个程序，创建两个子进程，
 * 并使用pipe()系统调用，
 * 将一个子进程的标准输出连接到另一个子进程的标准输入
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int p[2];
    pipe(p);

    pid_t rc = fork();
    // 写者
    if (rc == 0) {
        // 关闭读端
        close(p[0]);
        // 将标准输出重定向到管道写端口
        dup2(p[1], STDOUT_FILENO);
        // 原始写端口可以关闭
        close(p[1]);
        execlp("ls", "ls", NULL);           // 执行命令
        perror("execlp ls failed");
        exit(1);
    }

    pid_t rc2 = fork();
    if (rc2 == 0) {
        // 关闭写端
        close(p[1]);
        // 将标准输入重定向到管道读端
        dup2(p[0], STDIN_FILENO);
        // 关闭读端口
        close(p[0]);
        execlp("wc", "wc", "-l", NULL);     // 执行命令
        perror("execlp wc failed");
        exit(1);
    }
    // 父进程：关闭两个 pipe 端口
    close(p[0]);
    close(p[1]);

    // 等待两个子进程
    waitpid(rc, NULL, 0);
    waitpid(rc2, NULL, 0);

    return 0;
}