/*
 * 编写一个名为null.c的简单程序，它创建一个指向整数的指针，
 * 将其设置为NULL，然后尝试对其进行释放内存操作。(这里翻译错了，应该是解引用)
 * 把它编译成一个名为null的可执行文件。当你运行这个程序时会发生什么？
*/

#include <stdio.h>

int main(int argc, char *argv[]) {
    int *p = NULL;
    printf("%d\n", *p);
    return 0;
}

// 结果是：
// Segmentation fault (core dumped)

/*
    gdb调试信息
    Program received signal SIGSEGV, Segmentation fault.
    0x0000555555555168 in main (argc=1, argv=0x7fffffffdf08) at null.c:11
    11          printf("%d\n", *p);
*/

/*
valgrind --leak-check=yes ./null
==7327== Memcheck, a memory error detector
==7327== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==7327== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==7327== Command: ./null
==7327== 
==7327== Invalid read of size 4
==7327==    at 0x109168: main (null.c:11).  
==7327==  Address 0x0 is not stack'd, malloc'd or (recently) free'd
==7327== 
==7327== 
==7327== Process terminating with default action of signal 11 (SIGSEGV)
==7327==  Access not within mapped region at address 0x0
==7327==    at 0x109168: main (null.c:11)
==7327==  If you believe this happened as a result of a stack
==7327==  overflow in your program's main thread (unlikely but
==7327==  possible), you can try to increase the size of the
==7327==  main thread stack using the --main-stacksize= flag.
==7327==  The main thread stack size used in this run was 8388608.
==7327== 
==7327== HEAP SUMMARY:
==7327==     in use at exit: 0 bytes in 0 blocks
==7327==   total heap usage: 0 allocs, 0 frees, 0 bytes allocated
==7327== 
==7327== All heap blocks were freed -- no leaks are possible
==7327== 
==7327== For lists of detected and suppressed errors, rerun with: -s
==7327== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
Segmentation fault (core dumped)
*/