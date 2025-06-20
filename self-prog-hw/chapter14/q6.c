/*
创建一个分配整数数组的程序（如上所述），释放它们，
然后尝试打印数组中某个元素的值。程序会运行吗？
当你使用valgrind时会发生什么？
*/

#include <stdio.h>
#include <stdlib.h>

int main() {
    int *data = (int *)malloc(sizeof(int) * 10);
    for (int i = 0; i < 10; i++) {
        data[i] = i;
    }
    free(data);
    for (int i = 0; i < 10; i++) {
        printf("%d\n", data[i]);
    }
    return 0;
}

// 运行结果
/*
862745402
6
-2133447324
2115879031
4
5
6
7
8
9
*/

/*
==9133== Invalid read of size 4
==9133==    at 0x1091F8: main (q6.c:17)
==9133==  Address 0x4a77040 is 0 bytes inside a block of size 40 free'd // 指出问题：访问已经被free的内存
*/