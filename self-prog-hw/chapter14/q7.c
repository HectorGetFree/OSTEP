/*
现在传递一个有趣的值来释放（例如，在上面分配的数组中间的一个指针）。 
会发生什么？你是否需要工具来找到这种类型的
*/

#include <stdio.h>
#include <stdlib.h>


int main() {
    int *data = (int *)malloc(sizeof(int) * 10);
    for (int i = 0; i < 10; i++) {
        data[i] = i; 
    }
    free(data+1);
    return 0;
}

// Invalid free() / delete / delete[] / realloc()