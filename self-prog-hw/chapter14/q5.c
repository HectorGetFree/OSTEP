/*
 * 编写一个程序，使用malloc()创建一个名为data、大小为100的整数数组。
 * 然后，将data[100]设置为0。当你运行这个程序时会发生什么？
 * 当你使用valgrind运行这个程序时会发生什么？程序是否正确？
 * 
*/

#include <stdio.h>
#include <stdlib.h>

int main() {
    int *data = (int *)malloc(sizeof(int) * 100);
    data[100] = 0;
    free(data);
    return 0;
}

/*
            Invalid write of size 4 // 非法写入
==8916==    at 0x10918D: main (q5.c:13)
==8916==  Address 0x4a771d0 is 0 bytes after a block of size 400 alloc'd
==8916==    at 0x4846828: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==8916==    by 0x10917E: main (q5.c:12)
*/