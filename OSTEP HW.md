# OSTEP HW

## Chapter 04 

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

## Chapter 05 

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

## Chapter 07

`cd OSTEP/homework/cpu-sched`

``` bash
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

## Chapter 14

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

## Chapter 15

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

## Chapter 16

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

## Chapter 17 

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

## Chapter 18

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

## Chapter 20 

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

## Chapter 26

`cd OSTEP/homework/threads-intro`

```bash
Usage: x86.py [options]

Options:
  -h, --help            show this help message and exit
  -s SEED, --seed=SEED  the random seed
  -t NUMTHREADS, --threads=NUMTHREADS
                        number of threads
  -p PROGFILE, --program=PROGFILE
                        source program (in .s)
  -i INTFREQ, --interrupt=INTFREQ
                        interrupt frequency
  -r, --randints        if interrupts are random
  -a ARGV, --argv=ARGV  comma-separated per-thread args (e.g., ax=1,ax=2 sets
                        thread 0 ax reg to 1 and thread 1 ax reg to 2);
                        specify multiple regs per thread via colon-separated
                        list (e.g., ax=1:bx=2,cx=3 sets thread 0 ax and bx and
                        just cx for thread 1)
  -L LOADADDR, --loadaddr=LOADADDR
                        address where to load code
  -m MEMSIZE, --memsize=MEMSIZE
                        size of address space (KB)
  -M MEMTRACE, --memtrace=MEMTRACE
                        comma-separated list of addrs to trace (e.g.,
                        20000,20001)
  -R REGTRACE, --regtrace=REGTRACE
                        comma-separated list of regs to trace (e.g.,
                        ax,bx,cx,dx)
  -C, --cctrace         should we trace condition codes
  -S, --printstats      print some extra stats
  -v, --verbose         print some extra info
  -c, --compute         compute answers for me
```

