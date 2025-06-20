/* 编写一个使用malloc()来分配内存的简单程序，但在退出之前忘记释放它。
 * 这个程序运行时会发生什么？你可以用gdb来查找它的任何问题吗？
 * 用valgrind呢（再次使用--leak-check=yes标志）？
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int *p = (int *)malloc(sizeof(int));
    *p = 11;
    printf("%d\n", *p);
    return 0;
}

/*
 * 正常运行和使用gdb不会发现问题
 * 但是valgrind可以
 * 下面是他的输出
 * ==8611== 
==8611== HEAP SUMMARY:
==8611==     in use at exit: 4 bytes in 1 blocks
==8611==   total heap usage: 2 allocs, 1 frees, 1,028 bytes allocated
==8611== 
==8611== 4 bytes in 1 blocks are definitely lost in loss record 1 of 1 // 这里是关键信息
==8611==    at 0x4846828: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==8611==    by 0x109185: main (q4.c:10)
==8611== 
==8611== LEAK SUMMARY:
==8611==    definitely lost: 4 bytes in 1 blocks
==8611==    indirectly lost: 0 bytes in 0 blocks
==8611==      possibly lost: 0 bytes in 0 blocks
==8611==    still reachable: 0 bytes in 0 blocks
==8611==         suppressed: 0 bytes in 0 blocks
==8611== 
==8611== For lists of detected and suppressed errors, rerun with: -s
==8611== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
*/