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
