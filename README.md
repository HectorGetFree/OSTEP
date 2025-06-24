# OSTEP

`------------------CPU虚拟化------------------`

## HW01

`cd OSTEP/homework/cpu-intro`

默认情况下I/O需要5个时间单位

**题目2，3：**

发现交换次序是很重要的

交换前

```bash
Time        PID: 0        PID: 1           CPU           IOs
  1        RUN:cpu         READY             1          
  2        RUN:cpu         READY             1          
  3        RUN:cpu         READY             1          
  4        RUN:cpu         READY             1          
  5           DONE        RUN:io             1          
  6           DONE       BLOCKED                           1
  7           DONE       BLOCKED                           1
  8           DONE       BLOCKED                           1
  9           DONE       BLOCKED                           1
 10           DONE       BLOCKED                           1
 11*          DONE   RUN:io_done             1  
```

交换后

```bash
Time        PID: 0        PID: 1           CPU           IOs
  1         RUN:io         READY             1          
  2        BLOCKED       RUN:cpu             1             1
  3        BLOCKED       RUN:cpu             1             1
  4        BLOCKED       RUN:cpu             1             1
  5        BLOCKED       RUN:cpu             1             1
  6        BLOCKED          DONE                           1
  7*   RUN:io_done          DONE             1  
```

**cpu可以在I/O阻塞时执行另一个程序**

**题目4:**

```bash
./process-run.py -l 1:0,4:100 -S SWITCH_ON_END -c      ok | 3.10.13 py | 20:53:34 
Time        PID: 0        PID: 1           CPU           IOs
  1         RUN:io         READY             1          
  2        BLOCKED         READY                           1
  3        BLOCKED         READY                           1
  4        BLOCKED         READY                           1
  5        BLOCKED         READY                           1
  6        BLOCKED         READY                           1
  7*   RUN:io_done         READY             1          
  8           DONE       RUN:cpu             1          
  9           DONE       RUN:cpu             1          
 10           DONE       RUN:cpu             1          
 11           DONE       RUN:cpu             1  
```

发现设置标识会更改进程切换行为

**题目7:为什么运行一个刚刚完成I/O的进程是一个好主意**

这样的话如果该进程下有其他I/O操作，可以使用的CPU在I/O阻塞时去执行其他进程，避免最后只剩下I/O操作带来阻塞低效

**题目8：**

`./process-run.py -s 1 -l 3:50,3:50`

Case1: `-I IO_RUN_LATER -S SWITCH_ON_IO`

Stats: Total Time 15

Stats: CPU Busy 8 (53.33%)

Stats: IO Busy 10 (66.67%)

case 2: `-I IO_RUN_LATER -S SWITCH_ON_END`

Stats: Total Time 18

Stats: CPU Busy 8 (44.44%)

Stats: IO Busy 10 (55.56%)

case 3: `-I IO_RUN_IMMEDIATE -S SWITCH_ON_END`

Stats: Total Time 18

Stats: CPU Busy 8 (44.44%)

Stats: IO Busy 10 (55.56%)

case 4: -I `IO_RUN_IMMEDIATE -S SWITCH_ON_IO`

Stats: Total Time 15

Stats: CPU Busy 8 (53.33%)

Stats: IO Busy 10 (66.67%)

可以观察到在这种情况下，遇到IO时切换进程是一种明智的策略

`./process-run.py -s 2 -l 3:50,3:50`

Case1: `-I IO_RUN_LATER -S SWITCH_ON_IO`

Stats: Total Time 16

Stats: CPU Busy 10 (62.50%)

Stats: IO Busy 14 (87.50%)

case 2: `-I IO_RUN_LATER -S SWITCH_ON_END`

Stats: Total Time 30

Stats: CPU Busy 10 (33.33%)

Stats: IO Busy 20 (66.67%)

case 3: `-I IO_RUN_IMMEDIATE -S SWITCH_ON_END`

Stats: Total Time 30

Stats: CPU Busy 10 (33.33%)

Stats: IO Busy 20 (66.67%)

case 4: -I `IO_RUN_IMMEDIATE -S SWITCH_ON_IO`

Stats: Total Time 16

Stats: CPU Busy 10 (62.50%)

Stats: IO Busy 14 (87.50%)

结论一致，遇到IO切换仍然是很好的策略

`./process-run.py -s 3 -l 3:50,3:50`

Case1: `-I IO_RUN_LATER -S SWITCH_ON_IO`

Stats: Total Time 18

Stats: CPU Busy 9 (50.00%)

Stats: IO Busy 11 (61.11%)

case 2: `-I IO_RUN_LATER -S SWITCH_ON_END`

Stats: Total Time 24

Stats: CPU Busy 9 (37.50%)

Stats: IO Busy 15 (62.50%)

case 3: `-I IO_RUN_IMMEDIATE -S SWITCH_ON_END`

Stats: Total Time 24

Stats: CPU Busy 9 (37.50%)

Stats: IO Busy 15 (62.50%)

case 4: -I `IO_RUN_IMMEDIATE -S SWITCH_ON_IO`

Stats: Total Time 17

Stats: CPU Busy 9 (52.94%)

Stats: IO Busy 11 (64.71%)

在原有结论的基础上，可以发现``IO_RUN_IMMEDIATE -S SWITCH_ON_IO``

是很优秀的策略

## Chapter05 hw

**文件描述符**

```C
close(STDOUT_FILENO);
int fd = open("./q2.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
```

先关标准输出会使得文件描述符1空出来给open的文件，从而使得重定向是ok的，而先open再close会导致重定向不ok且看不到输出

**exec()系列函数变体存在的意义**

`execl()、execle()、execlp()、execv()、execvp()和execvP()`

参数传递方式

`l:以列表形式传递`

`v:以参数数组传递`

可执行文件查找方式

`无p:不查找PATH，需要提供完整路径`

`p:会查找PATH，不需要提供完整路径`

是否需要设置环境变量

`无e:默认行为`

`有e:自己设置环境变量`

**子进程中调用wait()的返回值**

首先在父进程中尝试

```
father wc = 95672
```

然后在子进程中wait()

```
child wc = -1
```

- rc == 0 时，是**子进程**。
- 子进程自己**没有调用过 fork()**，也就是说它**没有任何子进程**。

- 所以子进程调用 wait() 的时候，**找不到子进程可等**，于是：

  - wait() 返回 -1
  - errno 被设置为 ECHILD（即：No child processes）


**重定向**

`dup2(oldfd, newfd);`

> 把 oldfd 的**文件描述符所指的对象**复制一份到 newfd 上，并**关闭原来的 newfd**（如果已打开）。

- **文件描述符（fd）只是内核中 open file table 的一个索引**。
- dup2() 会让两个 fd 指向同一个内核对象（比如 pipe、文件等）；
- 但它们的 **fd 编号独立、互不依赖**。

## Chapter07 进程调用介绍

`cd OSTEP/homework/cpu-sched`

**SJF**:最短任务优先

**STCF**:最短完成时间优先

**response time 响应时间** = 首次运行时间 - 到达时间

**turnaround time 周转时间** = 完成时间 - 到达时间

**wait time 等待时间**

```bash
Usage: scheduler.py [options]

Options:
  -h, --help            show this help message and exit
  -s SEED, --seed=SEED  the random seed
  -j JOBS, --jobs=JOBS  number of jobs in the system
  -l JLIST, --jlist=JLIST
                        instead of random jobs, provide a comma-separated list
                        of run times
  -m MAXLEN, --maxlen=MAXLEN
                        max length of job
  -p POLICY, --policy=POLICY
                        sched policy to use: SJF, FIFO, RR
  -q QUANTUM, --quantum=QUANTUM
                        length of time slice for RR policy
  -c                    compute answers for me
```



> 1．使用SJF和FIFO调度程序运行长度为200的3个作业时，计算响应时间和周转时间

两种策略都是 (同时到达)

响应时间是：200 

周转：400

> 2. 现在做同样的事情，但有不同长度的作业，即100、200和300。

两种策略都是 (同时到达)

响应：400 / 3

周转：1000 / 3

> 3．现在做同样的事情，但采用RR调度程序，时间片为1

响应：1

周转：(298 + 499 + 600) / 3 

> 4．对于什么类型的工作负载，SJF提供与FIFO相同的周转时间？

所有作业长度相同 或者 作业按照非降序到达

> 5．对于什么类型的工作负载和量子长度，SJF与RR提供相同的响应时间？

对于**所有作业的运行时间都相等**的工作负载，且**时间片长度大于等于每个作业的运行时间**时，**SJF 与 RR 会提供相同的响应时间**。

> 6．随着工作长度的增加，SJF的响应时间会怎样？你能使用模拟程序来展示趋势吗？

显然增加

> 7．随着量子长度的增加，RR的响应时间会怎样？你能写出一个方程，计算给定N个工作时，最坏情况的响应时间吗？

随着量子长度增加，RR的响应时间显然增加

$T_{worst} = \frac{N}{2} \times q$  (q 表示量子长度)

## Chapter 08  调度：多级反馈队列

多级反馈队列 **MLFQ** (Multiple-level Feedback Queue)

运用历史经验预测未来

MLFQ的基本规则

● 规则1：如果A的优先级 > B的优先级，运行A（不运行B）

● 规则2：如果A的优先级 = B的优先级，==轮转==运行A和B

● 规则3：工作进入系统时，放在最高优先级（最上层队列）

● 规则4：一旦工作用完了其在某一层中的时间配额（无论中间主动放弃了多少次CPU），就降低其优先级（移入低一级队列）

● 规则5：经过一段时间S，就将系统中所有工作重新加入最高优先级队列 （解决饥饿问题 -- CPU一直被多个IO程序占用）

**经常放弃CPU的任务被视为可能为交互型程序，我们不想让用户等太长时间，所以这些需要放在高优先级快速执行**

**长时间占用CPU的任务会被放在低优先级**



> 如何判断工作长短并改变优先级

如果不知道工作是短工作还是长工作，那么就在开始的时候假设其是短工作，并赋予最高优先级。如果确实是短工作，则很快会执行完毕，否则将被慢慢移入低优先级队列，而这时该工作也被认为是长工作了

## Chapter 09 调度：比例份额

**彩票调度**

只需要一个不错的随机数生成器来选择中奖彩票和一个记录系统中所有进程的数据结构（一个列表），以及所有彩票的总数。

**步长调度**

当需要进行调度时，选择目前拥有最小行程值的进程，并且在运行之后将该进程的行程值增加一个步长

**为什么有了精准的步长调度我们依然需要彩票调度？**

因为彩票调度不需要维护全部变量，可以更好地处理新加入的进程（如果用步长调度的话，如何设置新进程的行程值是一个难以解决的问题）

**以上两种调度方式都没有被广泛使用**，一个原因是这两种方式都不能很好地适合I/O[AC97]；另一个原因是其中最难的票数分配问题并没有确定的解决方式

通用的调度程序还是上一章介绍的MLFQ

## Chapter 10 多处理器调度

**单队列多处理器调度 SQMS** Single Queue Multiprocessor Scheduling 

- 优点：简单，直接讲但处理器中的策略拓展到多处理器
- 缺点：
  - 缺乏可拓展性：随着CPU数目增加，对锁的争用浪费了很多时间
  - 较差的缓存亲和性：可能会有任务分散到不同CPU时太过分散
    - 解决方法是利用一些 **亲和度机制 ** 尽可能让进程在同一个CPU上运行

**多队列多处理器调度 MQMS** Multi-Queue Multiprocessor Scheduling

为每个CPU分别维护一个任务队列，能够解决SQMS的问题，但是带来了新的问题

- 负载不均：即一个CPU可能执行完它队列中所有任务而空闲，但是其他CPU还在工作
- 解决方法是：进行任务迁移
- 基本方法：**工作窃取** 通过这种方法，工作量较少的（源）队列不定期地“偷看”其他（目标）队列是不是比自己的工作多。如果目标队列比源队列（显著地）更满，就从目标队列“窃取”一个或多个工作，实现负载均衡

**Linux多处理器调度**

存在三种不同的调度

- O(1):类似于MLFQ
- 完全公平调度程序 **CFS** ：类似步长调度
- BFS调度程序 **Brain Fuck Scheduling** ：单队列 + 比例调度

`------------------内存虚拟化------------------`

## Chapter 13 抽象：地址空间

> **分时系统 = 多道程序系统 + 快速切换 + 用户交互机制**

如果你在一个程序中打印出一个地址，那就是一个虚拟的地址

## Chapter 14 插叙：内存操作API

`void *malloc(size_t size)`

只需要包含头文件 stdlib.h 就可以使用malloc 了。但实际上，甚至都不需这样做，因为C库是C程序默认链接的，其中就有mallock()的代码，加上这个头文件只是让编译器检查你是否正确调用了malloc()（即传入参数的数目正确且类型正确）

- 为字符串分配空间，习惯用法是：`malloc(strlen(s) + 1)`

- 对结果得到的指针进行强制类型转换不是必须的，只是提供一些信息给读程序的人

`free()`

`calloc()` 分配内存并初始化为0

`realloc()` 重新分配内存大小

### HW

**gcc使用说明**

| 选项        | 作用说明                               |
| ----------- | -------------------------------------- |
| -o filename | 指定输出文件名                         |
| -c          | 编译为目标文件（.o），不链接           |
| -Wall       | 打开所有常见警告                       |
| -Werror     | 将所有警告当作错误                     |
| -g          | 生成调试信息（可用 gdb 调试）          |
| -O2、-O3    | 启用优化级别（越大越快，但编译更复杂） |
| -std=c99    | 指定使用 C99 标准编译                  |
| -lm         | 链接数学库（例如使用 sin、pow 等函数） |

**valgrind使用说明**

| 命令行参数            | 含义                                      |
| --------------------- | ----------------------------------------- |
| --leak-check=full     | 检查所有内存泄漏（强烈推荐）              |
| --show-leak-kinds=all | 显示各种类型泄漏（definitely / possibly） |
| --track-origins=yes   | 追踪未初始化变量的来源（更慢但更准确）    |
| -v                    | 输出更详细信息                            |

*输出解析*

```bash
==7327== Invalid read of size 4 # 非法读取4字节
==7327==    at 0x109168: main (null.c:11).  # 问题在程序的第11行
==7327==  Address 0x0 is not stack'd, malloc'd or (recently) free'd # 访问了地址0x0，也就是NULL
```

```bash
==8916==  Invalid write of size 4 # 非法写入
==8916==    at 0x10918D: main (q5.c:13)
==8916==  Address 0x4a771d0 is 0 bytes after a block of size 400 alloc'd # 这里说明的是数组越界写入 -- 关键词是after
```

```bash
==9133== Invalid read of size 4
==9133==    at 0x1091F8: main (q6.c:17)
==9133==  Address 0x4a77040 is 0 bytes inside a block of size 40 free'd // 指出问题：访问已经被free的内存
```

## Chapter 15 机制：地址转换

受限直接访问 LDE Limited Direct Exection

LDE背后的想法很简单：让程序运行的大部分指令直接访问硬件，只在一些关键点（如进程发起系统调用或发生时钟中断）由操作系统介入来确保“在正确的时间，正确的地点，做正确的事”

**地址转换** ：每个CPU需要两个寄存器，**基址寄存器 和 界限寄存器**

### HW

`cd OSTEP/homework/vm-mechanism`

```bash
Usage: relocation.py [options]

Options:
  -h, --help            show this help message and exit
  -s SEED, --seed=SEED  the random seed
  -a ASIZE, --asize=ASIZE
                        address space size (e.g., 16, 64k, 32m, 1g)
  -p PSIZE, --physmem=PSIZE
                        physical memory size (e.g., 16, 64k, 32m, 1g)
  -n NUM, --addresses=NUM
                        number of virtual addresses to generate
  -b BASE, --b=BASE     value of base register
  -l LIMIT, --l=LIMIT   value of limit register
  -c, --compute         compute answers for me
```

虚拟地址从零开始计算，所以界限寄存器最起码是最大的虚拟地址+1

## Chapter 16 分段

地址空间中有三个逻辑不同的段：代码，栈和堆

分段的做法是：虚拟地址的前2位标识段寄存器，后面的位用作段内偏移

说白了就是不同进程的相同段都放在一起，段与段之间是分开的，每个段都维护有基址寄存器和界限寄存器

**反向偏移量计算**：用偏移量减去每个段的最大偏移（比如说二进制地址标识偏移的位数为6位，那么最大偏移就是2^6^），然后将计算结果加到基址上得到物理地址

- 效果跟虚拟地址的10进制大小减去虚拟空间总大小然后加上物理基址是一样的

### HW

`cd OSTEP/homework/vm-segmentation`

```bash
Usage: segmentation.py [options]

Options:
  -h, --help            show this help message and exit
  -s SEED, --seed=SEED  the random seed
  -A ADDRESSES, --addresses=ADDRESSES
                        a set of comma-separated pages to access; -1 means
                        randomly generate
  -a ASIZE, --asize=ASIZE
                        address space size (e.g., 16, 64k, 32m, 1g)
  -p PSIZE, --physmem=PSIZE
                        physical memory size (e.g., 16, 64k, 32m, 1g)
  -n NUM, --numaddrs=NUM
                        number of virtual addresses to generate
  -b BASE0, --b0=BASE0  value of segment 0 base register
  -l LEN0, --l0=LEN0    value of segment 0 limit register
  -B BASE1, --b1=BASE1  value of segment 1 base register
  -L LEN1, --l1=LEN1    value of segment 1 limit register
  -c                    compute answers for me
```

```bash
ARG seed 0
ARG address space size 128
ARG phys mem size 512

Segment register information:

  Segment 0 base  (grows positive) : 0x00000000 (decimal 0)
  Segment 0 limit                  : 20

  Segment 1 base  (grows negative) : 0x00000200 (decimal 512)
  Segment 1 limit                  : 20

Virtual Address Trace
  VA  0: 0x0000006c (decimal:  108) --> VALID in SEG1: 0x000001ec (decimal:  492)
  VA  1: 0x00000061 (decimal:   97) --> SEGMENTATION VIOLATION (SEG1)
  VA  2: 0x00000035 (decimal:   53) --> SEGMENTATION VIOLATION (SEG0)
  VA  3: 0x00000021 (decimal:   33) --> SEGMENTATION VIOLATION (SEG0)
  VA  4: 0x00000041 (decimal:   65) --> SEGMENTATION VIOLATION (SEG1)
```

注意第一个转换`0x0000006c (decimal:  108)`，转化为2进制，他的首位是1，标识位SEG1，说明是反向增长，然后用上面介绍的计算方法得到物理地址

## Chapter 17 空闲空间管理

关键词：

**空闲列表** 

**分割与合并**

保证外部碎片最小化的策略：

- 最优匹配：遍历空间列表，找到最合适的空闲块
- 最差匹配：遍历，找到最大的空闲块
- 首次匹配：找到第一个足够大的块
- 下次匹配：从上一次查找结束的地方开始接着找

其他方式：

- 分离空闲列表
- 二分伙伴系统

### HW

`cd OSTEP/homework/vm-freespace`

```bash
Usage: malloc.py [options]

Options:
  -h, --help            show this help message and exit
  -s SEED, --seed=SEED  the random seed
  -S HEAPSIZE, --size=HEAPSIZE
                        size of the heap
  -b BASEADDR, --baseAddr=BASEADDR
                        base address of heap
  -H HEADERSIZE, --headerSize=HEADERSIZE
                        size of the header
  -a ALIGNMENT, --alignment=ALIGNMENT
                        align allocated units to size; -1->no align
  -p POLICY, --policy=POLICY
                        list search (BEST, WORST, FIRST)
  -l ORDER, --listOrder=ORDER
                        list order (ADDRSORT, SIZESORT+, SIZESORT-, INSERT-
                        FRONT, INSERT-BACK)
  -C, --coalesce        coalesce the free list?
  -n OPSNUM, --numOps=OPSNUM
                        number of random ops to generate
  -r OPSRANGE, --range=OPSRANGE
                        max alloc size
  -P OPSPALLOC, --percentAlloc=OPSPALLOC
                        percent of ops that are allocs
  -A OPSLIST, --allocList=OPSLIST
                        instead of random, list of ops (+10,-0,etc)
  -c, --compute         compute answers for me
```

策略与空闲列表排序之间的相互影响 -- 摘自微信读书用户评论

比如：

- 【-p BEST】配合【-l SIZESORT+】，能提升效率；
- 【-p WORST】配合【-l SIZESORT-】，能提升效率；
- 【-p FIRST】配合【-l SIZESORT+/-】，就等效于【-p BEST/WORST】
- 【-l ADDRSORT】对【合并】free list，有积极作用。

## Chapter 18 分页：介绍

**分页** 

- 不是将一个进程的地址空间分割成几个不同长度的逻辑段（即代码、堆、段），而是分割成固定大小的单元，每个单元称为一页

- 相应地将物理内存的页概念叫做 **页帧**

**页表**

- 操作系统为每个进程保存一个 **存储虚拟地址和物理地址转换的** ==页表==；*说白了就是一组虚拟地址到物理地址的映射*

- 存储在内存

- 页表基址寄存器 -- 包含页表起始位置的物理地址

- **页表条目 PTE**

  内容

  - 物理页帧号 **PFN**

  - 有效位 **valid bit** 表示特定的地址转换是否有效
  - 保护位 **protection bit** 表明页是否可以读取、写入或执行
  - 存在位 **present bit** 表明该页实在物理内存还是磁盘
  - 脏位 **dirty bit** 表明页在被带入内存后是否被修改过
  - 参考位 **reference bit** 用于追踪页面是否被访问，在页面替换时要进行考虑
  - 保留位

此时的虚拟地址包括两部分：**虚拟页面号VPN** + **页内偏移量offset**

- 例如：虚拟地址21 对应 0x010101假设前两位是VPN，那么要找的就是*在虚拟页“01”（或1）的第5个（“0101”）字节处*

- **物理页面号** 叫 **PFN 或者 PPN**

**翻译规则：以VPN作为索引访问页表的第VPN项得到对应的PFN，然后物理地址等于PFN跟offset的拼接

### HW

`cd OSTEP/homwork/vm-paging`

```bash
Usage: paging-linear-translate.py [options]

Options:
  -h, --help            show this help message and exit
  -A ADDRESSES, --addresses=ADDRESSES
                        a set of comma-separated pages to access; -1 means
                        randomly generate
  -s SEED, --seed=SEED  the random seed
  -a ASIZE, --asize=ASIZE
                        address space size (e.g., 16, 64k, 32m, 1g)
  -p PSIZE, --physmem=PSIZE
                        physical memory size (e.g., 16, 64k, 32m, 1g)
  -P PAGESIZE, --pagesize=PAGESIZE
                        page size (e.g., 4k, 8k, whatever)
  -n NUM, --numaddrs=NUM
                        number of virtual addresses to generate
  -u USED, --used=USED  percent of virtual address space that is used
  -v                    verbose mode
  -c                    compute answers for me
```

线性页表大小随着地址空间的增大而增大

线性页面大小随着页大小的增长而减小

## Chapter 19 分页：快速地址转换 TLB

TLB **T**ranslation-**l**ookaside **B**uffer -- 地址转换缓存

- 缓存了最近使用过的 **VPN -> PFN转换**

- 谁来处理 **TLB未命中**

  - 一种方式是硬件（通常为CISC采取的策略）

    需要知道页表在内存中的位置（通过页表基址寄存器）

    然后遍历找表项即可

  - 另一种是软件来处理（通常为RISC采取的策略）

    发生TLB未命中时，硬件系统抛出一个异常，这会暂停当前的指令流，将特权级别提升至内核模式，跳转到陷阱处理程序

    然后处理未命中，完毕后回到导致陷阱的指令

- TLB的内容

  - 典型的TLB是全关联的

  - 一条TLB的内容可能如下

    VPN ｜ PFN ｜ 其他位

- 上下文切换时对TLB的处理

  目的是为了隔离不同进程之间的数据

  - 一种方式是：简单的清零 -- 将TLB的全部有效位置0，标记为无效

    开销大

  - 减小开销的方式是：在每条TLB内容中加上 **地址空间标识符 ASID ** 

    用来区别不同进程的数据，从而实现TLB在进程之间的共享

-  替换策略
  - LRU
  - 随机

## Chapter 20 分页：较小的页

页表太大会占用大量物理内存

**如何让页表更小**？

- **更大的页** 

  - 缺点

    大内存页会导致没页内内存的浪费，产生大量内部碎片

- **混合方法：分页+分段**

  为每个逻辑分段提供一个页表

  - 在这里依然使用 **基址寄存器** 和 **界限寄存器** （在MMU内存管理单元中）

    基址寄存器保存该段的页表的物理地址

    界限寄存器用于指示页表的结尾

  - 虚拟地址的结构是

    ｜ 前两位 -> 标识逻辑段 ｜ VPN ｜ Offset ｜

- **多级页表**

  - 使用页目录

    每个页目录条目 **PDE** 指向一个页表

    其结构至少是：

    ｜ 有效位 ｜ PFN ｜

    这里的PFN指向的是该==页表所在的物理页帧==

    注意区别PTE中的PFN，他指向的是==实际映射物理页帧==

    - 有效位为0: 如果整页的PTE无效，就完全不分配该页的页表，从而达到节省内存的目的
    - 有效位为1:说明该页表存有数据

    - ###### 因此多级页表分配的页表空间跟你正在使用的地址空间内存量成正比

    - 此时的虚拟地址为（如果是两级页表）

      |                                VPN                               |                  offset                  |

      |           页目录索引        ｜       页表索引     ｜                 offset                  ｜

- **反向页表**

  每个页表项对应的是系统的每个 **物理页** ，记录的信息是

  - 该物理页被哪个进程引用
  - 以及该进程的哪个虚拟页映射到这个物理页

  因为它是以物理页来组织的，之前页表是通过虚拟页来组织映射的，所以才叫反向页表

### HW

`cd OSTEP/homework/vm-smalltables`

```bash
Usage: paging-multilevel-translate.py [options]

Options:
  -h, --help            show this help message and exit
  -s SEED, --seed=SEED  the random seed
  -a ALLOCATED, --allocated=ALLOCATED
                        number of virtual pages allocated
  -n NUM, --addresses=NUM
                        number of virtual addresses to generate
  -c, --solve           compute answers for me
```



如果在多级页表中发生了TLB未命中，我们只需要一个 定位第一级别页表或者页目录的 **基址寄存器**

因为剩余的位置信息我们都可以通过索引计算得到

## Chapter 21 超越物理内存：机制

为了支持更大的地址空间，操作系统需要把没有在用的的那部分地址空间存到 **硬盘** -- 这也就是我们讨论的 **超越物理内存**

**交换空间**

硬盘上用于物理页的移入和移出的空间，操作系统需要记住给定页的硬盘地址

- 硬件是如何发现页不在物理内存的？

  使用PTE中的 **存在位** 这一信息	

- **页错误**：访问不在物理内存中的页

  如果一个页不存在，它已被交换到硬盘，在处理页错误的时候，操作系统需要将该页交换到内存中

  操作系统通过页表中的PTE的某些位来存储硬盘地址，然后根据这一信息将页读到内存中

  然后将此页标记为存在，并更新PTE的PFN字段

- 页替换

  当内存快满、有需要从硬盘换入物理页时，需要进行替换，因此需要替换策略

- 交换何时真实发生

  - 为了保证少量的空闲内存，操作系统会设置

    - 高水位线 HW high watermark
    - 低水位线 LW low watermark

  - 交换守护进程 **swap deamon**

    如果一个页不存在，它已被交换到硬盘，在处理页错误的时候，操作系统需要将该页交换到内存中

    完成工作后该进程会进入休眠

  - 提升性能

    同时执行多个交换过程

## Chapter 22 超越物理内存：策略

由于内存只包含系统和所有页的子集，因此可以将其视为系统中虚拟内存页的缓存

衡量硬件缓存的指标 ： 平均内存访问时间 AMAT Average Memory Access Time

$\text{AMAT} = \text{Hit Time} + \text{Miss Rate} \times \text{Miss Penalty}$

替换策略

- **最优替换策略**

  即替换内存中在最远将来才会被访问到的页，可以达到缓存未命中率最低

  基本无法实现，但是可以作为衡量其他策略性能的参考

- 简单策略 **FIFO**

  先进入队列的页被踢出

- 随机

- 利用历史数据：**LRU**

- **近似LRU**

  完全的LRU需要很大的开销

  为了减少开销，我们可以使用近似的LRU

  需要在硬件中增加一个使用位，当页被引用时设置为1，但是硬件不会把它置0，这由操作系统负责

  - 时钟算法

    操作系统检查当前指向的页P的使用位是1还是0。如果是1，则意味着页面P最近被使用，因此不适合被替换。然后，P的使用位设置为0，时钟指针递增到下一页（P + 1）。该算法一直持续到找到一个使用位为0的页，使用位为0意味着这个页最近没有被使用过（在最坏的情况下，所有的页都已经被使用了，那么就将所有页的使用位都设置为0）

- 考虑脏页

  如果页已被修改（modified）并因此变脏（dirty），则踢出它就必须将它写回磁盘，这很昂贵。如果它没有被修改（因此是干净的，clean），踢出就没成本

  因此我们更倾向于踢出干净页

  需要硬件增加脏位

- 页选择

  **预取**：操作系统可以提前预测某个页面即将被使用从而提前载入

  **聚集写入**：在内存中收集一些待完成写入，并以一种（更高效）的写入方式将它们写入硬盘

## Chapter 23 VAX/VMS虚拟内存系统

其实是前面的理论的实际例子

下面是一些其他的虚拟内存技巧

页替换 -- **分段FIFO**

- 驻留集大小 RSS Resident Set Size

  每个进程可以保存在内存中的最大页数 -- 为了解决某些程序一直占用大量内存的问题

  每个页都保存在FIFO列表中。当一个进程超过其RSS时，“先入”的页被驱逐

- 二次机会列表

  全局的干净页空闲列表 和 脏页列表

  如果另一个进程Q需要一个空闲页，它会从全局干净列表中取出第一个空闲页。但是，==如果原来的进程P在回收之前在该页上出现页错误，则P会从空闲（或脏）列表中回收，从而避免昂贵的磁盘访问== *这就是所谓的二次机会*

**按需置零**

在一个初级实现中，操作系统响应一个请求，在物理内存中找到页，将该页添加到你的堆中，并将其置零（安全起见，这是必需的。否则，你可以看到其他进程使用该页时的内容。）

但是初级时间是昂贵的，特别是页没有被进程使用

所以要用按需置零

- 操作系统通过在将页加入地址空间时，在页表中放入一个标记页不可访问的条目
- 如果进程引用页，则向操作系统发送陷阱
- 操作系统梳理陷阱，注意到PTE的某些保留位，然后得知这是一个按需置零页
- 才会将它置零

**写时复制** COW copy-on-write

如果操作系统需要将一个页面从一个地址空间复制到另一个地址空间，不是实际复制它，而是将其映射到目标地址空间，并在两个地址空间中将其标记为只读

但是，如果其中一个地址空间确实尝试写入页面，就会陷入操作系统。操作系统会注意到该页面是一个COW页面，因此（惰性地）分配一个新页，填充数据，并将这个新页映射到错误处理的地址空间。该进程然后继续，现在有了该页的私人副本
