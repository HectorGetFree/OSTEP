/*
 * 编写一个调用fork()的程序，然后调用某种形式的exec()来运行程序/bin/ls。
 * 看看是否可以尝试exec()的所有变体，
 * 包括execl()、execle()、execlp()、execv()、execvp()和execvP()。
 * 为什么同样的基本调用会有这么多变种？
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
        char *myargs[2] = {"/bin/ls", NULL};
        int key = 1;
        switch (key)
        {
            case 1:
                execl("/bin/ls", "ls", NULL);
                break;
            case 2:
                execle("/bin/ls", "ls", NULL, NULL);
                break;
            case 3:
                execlp("ls", "ls", NULL);
                break;
            case 4:
                execv("/bin/ls", myargs);
                break;
            case 5:
                execvp("ls", myargs);
                break;
            default:
                execvP("ls", "/bin", myargs);
                break;
        }
    }
    return 0;
}