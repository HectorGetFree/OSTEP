/* 
 * 现在编写一个程序，在父进程中使用wait()，等待子进程完成。
 * wait()返回什么？
 * 如果你在子进程中使用wait()会发生什么？
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) { 
        int wc = wait(NULL);
        printf("child wc = %d\n", wc);
    } else {
       
        printf("father\n");
    }
    return 0;
}