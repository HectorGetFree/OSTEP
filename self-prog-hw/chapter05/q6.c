/* 
 * 对前一个程序稍作修改，这次使用waitpid()而不是wait()。
 * 什么时候waitpid()会有用？
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        printf("rc = %d\n", rc);
        printf("hello this is child\n");
    } else {
        int status;
        pid_t i =  waitpid(rc, &status, 0);
        printf("parent waitpid: i = %d\n", i);
    }
}
