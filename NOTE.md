# OSTEP

## 虚拟化

`------------------CPU虚拟化------------------`

### Chapter 04 抽象：进程

**进程**

用来描述一个运行的程序，以及它所包含的与其有关的信息

- 时分共享

  让一个进程只运行一个时间片，然后切换到其他进程，操作系统提供了存在多个虚拟CPU的假象

- 进程API

  - 创建 create
  - 销毁 destroy
  - 等待 wait
  - 其他控制 miscellaneous control

  - 状态 status

- 进程状态

  - 运行 running
  - 就绪 ready
  - 阻塞 blocked
  - 初始 initial
  - 僵尸 final 处于已经退出但是尚未清理的最终状态

- 数据结构 

  进程控制块 PCB *process control block*-- 用于存储与进程有关的信息

  进程列表 -- 用于记录所有的进程的信息

### Chapter05 插叙：进程API

**fork()**

- 用于创建新的进程
- 新创建的进程称为子进程
  - 子进程不会从main函数开始执行，而是从fork()后开始执行
  - 子进程不是完全拷贝父进程，虽然有 **独立的地址空间**，但是父进程从fork()得到的返回值是子进程的PID(process identifier 进程描述符) ，而子进程得到的返回值是0

**pid_t wait(int *status)**

- 调用wait()让父进程等待子进程结束

- 更完整的兄弟接口 **pid_t waitpid(pid_t pid, int *status, int options)**

  区别是waitpid()可以指定要等待的进程

  options的参数：

  | **选项**  | **含义**                                                |
  | --------- | ------------------------------------------------------- |
  | 0         | 默认行为，阻塞直到目标子进程退出                        |
  | WNOHANG   | 非阻塞，若没有子进程退出则立即返回 0                    |
  | WUNTRACED | 也返回停止（如被 SIGSTOP 信号停止）但未终止的子进程信息 |

**exec()** 

- exec()会从可执行程序中加载代码和静态数据，并用它覆写自己的代码段（以及静态数据），堆、栈及其他内存空间也会被重新初始化

- 也就是从父进程创建了一个新的进程，但是子进程运行的是另外一个程序

- exec()代表的是一组API，根据用途不同具有不同的参数和签名

  | **函数名**           | **签名**                                                     |
  | -------------------- | ------------------------------------------------------------ |
  | execl                | int execl(const char *path, const char *arg, ..., NULL);     |
  | execle               | int execle(const char *path, const char *arg, ..., NULL, char *const envp[]); |
  | execlp               | int execlp(const char *file, const char *arg, ..., NULL);    |
  | execv                | int execv(const char *path, char *const argv[]);             |
  | execvp               | int execvp(const char *file, char *const argv[]);            |
  | execvpe ⬅️（GNU扩展） | int execvpe(const char *file, char *const argv[], char *const envp[]); |

### Chapter 06 机制：受限直接执行

受限直接执行 limited direct execution

- **用户程序在用户模式直接执行（运行在真实 CPU 上），只有在特定操作时（如系统调用、异常、时间片用完）才“陷入”内核由操作系统接管（内核模式），完成操作后从陷阱返回到用户模式**

- 系统调用

  用户通过系统调用执行一些特权指令

- 陷阱表

  - 内核通过在启动时设置陷阱表，标识陷阱需要运行的程序

- 在进程之间切换

  - 协作方式：等待系统调用

    - 在这种风格下，操作系统相信系统的进程会合理运行。运行时间过长的进程被假定会定期放弃CPU，以便操作系统可以决定运行其他任务

    - 在协作调度系统中，OS通过等待系统调用，或某种非法操作发生，从而重新获得CPU的控制权
    - 如果某个进程不进行系统调用，情况就比较棘手

  - 非协作方式：操作系统进行控制

    - 时钟中断

      时钟设备可以编程为每隔几毫秒产生一次中断。产生中断时，当前正在运行的进程停止，操作系统中预先配置的中断处理程序（interrupt handler）会运行。此时，操作系统重新获得CPU的控制权，因此可以做它想做的事：停止当前进程，并启动另一个进程

    - 硬件在发生中断时要保存程序的状态，以便于恢复

  - 保存和恢复上下文

    - 保存当前进程的一些寄存器的值（例如，到内核栈中）
    - 为即将执行的进程恢复一些寄存器的值

### Chapter 07 进程调用：介绍

**SJF**:最短任务优先

**STCF**:最短完成时间优先

**response time 响应时间** = 首次运行时间 - 到达时间

**turnaround time 周转时间** = 完成时间 - 到达时间

**wait time 等待时间**

**调度策略**

- 先进先出 FIFO
  - 简单且易于实现
  - 但是可能会出现 *耗时较少的资源消费者排在重量级的资源消费者的后面* （比如说超市结账的时候你前面排了一个买了很多东西的人，但是你只需要结算几件商品）
- 最短任务优先 SJF  Shortest Job First
  - 先运行最短的任务，然后是次短的任务，如此下去（ 非抢占 ）
  - 但是纯SFJ遇到重量级消费者先到达的情况下，依然会出现上述问题
- 最短完成时间优先 STCF Shortest Time-to-Completion First
  - 其实是为SFJ添加了抢占
  - 每当新工作进入系统时，它就会确定剩余工作和新工作中，谁的剩余时间最少，然后调度该工作
  - 在衡量条件是**周转时间**的情况下 **STCF**是最优的

引入分时系统导致用户需要系统的交互性好，也就是比如输入终端命令期望得到更快的响应，因此我们还要有 **响应时间** 这一度量

- 轮转 RR Round-Robin
  - RR在一个时间片内运行一个工作，然后切换到运行队列的下一个任务，反复执行，直到所有任务完成
  - 时间长度必须是时钟中断周期的倍数 -- 方便操作系统接管调度
  - 时间片长度越短，响应时间越好
  - 但是上下文切换会有相应的成本 -- 可以用 **摊销技术**（减少成本的频度，系统的总成本就会降低）
  - RR对周转时间比较糟糕

> 任何公平（fair）的政策（如RR），即在小规模的时间内将CPU均匀分配到活动进程之间，在周转时间这类指标上表现不佳。事实上，这是固有的权衡：如果你愿意不公平，你可以运行较短的工作直到完成，但是要以响应时间为代价。如果你重视公平性，则响应时间会较短，但会以周转时间为代价

抢占式与非抢占式调度程序

- 非抢占式调度程序会将每项工作做完再考虑是否运行新工作
- 抢占式则与之相反

### Chapter 08  调度：多级反馈队列

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

### Chapter 09 调度：比例份额

**彩票调度**

只需要一个不错的随机数生成器来选择中奖彩票和一个记录系统中所有进程的数据结构（一个列表），以及所有彩票的总数。

**步长调度**

当需要进行调度时，选择目前拥有最小行程值的进程，并且在运行之后将该进程的行程值增加一个步长

**为什么有了精准的步长调度我们依然需要彩票调度？**

因为彩票调度不需要维护全部变量，可以更好地处理新加入的进程（如果用步长调度的话，如何设置新进程的行程值是一个难以解决的问题）

**以上两种调度方式都没有被广泛使用**，一个原因是这两种方式都不能很好地适合I/O[AC97]；另一个原因是其中最难的票数分配问题并没有确定的解决方式

通用的调度程序还是上一章介绍的MLFQ

### Chapter 10 多处理器调度

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

### Chapter 13 抽象：地址空间

> **分时系统 = 多道程序系统 + 快速切换 + 用户交互机制**

如果你在一个程序中打印出一个地址，那就是一个虚拟的地址

### Chapter 14 插叙：内存操作API

`void *malloc(size_t size)`

只需要包含头文件 stdlib.h 就可以使用malloc 了。但实际上，甚至都不需这样做，因为C库是C程序默认链接的，其中就有mallock()的代码，加上这个头文件只是让编译器检查你是否正确调用了malloc()（即传入参数的数目正确且类型正确）

- 为字符串分配空间，习惯用法是：`malloc(strlen(s) + 1)`

- 对结果得到的指针进行强制类型转换不是必须的，只是提供一些信息给读程序的人

`free()`

`calloc()` 分配内存并初始化为0

`realloc()` 重新分配内存大小

### Chapter 15 机制：地址转换

受限直接访问 LDE Limited Direct Exection

LDE背后的想法很简单：让程序运行的大部分指令直接访问硬件，只在一些关键点（如进程发起系统调用或发生时钟中断）由操作系统介入来确保“在正确的时间，正确的地点，做正确的事”

**地址转换** ：每个CPU需要两个寄存器，**基址寄存器 和 界限寄存器**

### Chapter 16 分段

地址空间中有三个逻辑不同的段：代码，栈和堆

分段的做法是：虚拟地址的前2位标识段寄存器，后面的位用作段内偏移

说白了就是不同进程的相同段都放在一起，段与段之间是分开的，每个段都维护有基址寄存器和界限寄存器

**反向偏移量计算**：用偏移量减去每个段的最大偏移（比如说二进制地址标识偏移的位数为6位，那么最大偏移就是2^6^），然后将计算结果加到基址上得到物理地址

- 效果跟虚拟地址的10进制大小减去虚拟空间总大小然后加上物理基址是一样的

### Chapter 17 空闲空间管理

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

### Chapter 18 分页：介绍

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

**翻译规则**：以VPN作为索引访问页表的第VPN项得到对应的PFN，然后物理地址等于PFN跟offset的拼接

### Chapter 19 分页：快速地址转换 TLB

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

### Chapter 20 分页：较小的页

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

### Chapter 21 超越物理内存：机制

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

### Chapter 22 超越物理内存：策略

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

### Chapter 23 VAX/VMS虚拟内存系统

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

## 并发

### Chapter 26 并发：介绍

**线程**

- 每个线程类似于独立的进程，区别是：*线程共享地址空间，从而能够访问相同的数据*

  - 每个线程都有一个栈

- 线程有一个程序计数器 **PC**，记录程序从哪里取指令

- 进程上下文切换

  - 需要一个或者多个线程控制块 **TCB** Thread Control Block
  - 用于保存每个线程的状态

- **竞态条件**

  多个线程进入临界区更新共享的数据导致结果异常

- **临界区**

  访问共享变量的，会因为多线程导致竞争的代码段

- **互斥**

  即临界区内只能有一个线程

- **原子操作**

  *要么没有运行，要么运行完成，不存在中间状态*

  - 需要硬件提供一些有用的指令 -- **同步原语**

### Chapter 27 插叙：线程API

**线程创建**

```C
#include <pthread.h>

int pthread_create(pthread_t *thread,
                   const pthread_attr_t *attr,
                   void *(*start_routine)(void *),
                   void *arg);
```

- 第一个参数 thread 是指向pthread_t的指针，用于保存新线程的ID
- 第二个参数用于指定该线程可能具有的属性 -- 如：栈大小，调度优先级信息
- 第三个参数是一个函数指针，表明这个新创建的线程应该在哪个函数里运行
- 第四个参数用于传递给线程函数（第三个参数）所需要的参数

**等待线程完成**

```C
pthread_join(pthread_t tid, void **retval)
```

- 第一个参数用于指定要等待的线程
- 第二个参数是一个指针，指向线程函数的返回值，若不关心可以传入NULL

**锁**

```C
int pthread_mutex_lock(pthread_mutex_t *mutex); ￼
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```

- 使用例子

  ```C
  pthread_mutex_t lock; ￼
  pthread_mutex_lock(&lock);￼
  x = x + 1; // or whatever your critical section is ￼
  pthread_mutex_unlock(&lock);
  ```

- 意思是

​	如果在调用pthread_mutex_lock()时没有其他线程持有锁，线程将获取该锁并进入临界区。如果另一个线程	确实持有该锁，那么尝试获取该锁的线程将不会从该调用返回，直到获得该锁

**初始化锁**

- `pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;`

  将锁设置为默认值

- 动态初始化：`int rc = pthread_mutex_init(&lock, NULL); `

  第一个参数是锁本身的地址

  第二个参数是一组可选属性， NULL就是默认值

用完锁后记得销毁`int pthread_mutex_destroy(pthread_mutex_t *mutex);`

然后记得使用带有基本断言的包装函数 -- 首字母大写即可

**获取锁**

```C
int pthread_mutex_trylock(pthread_mutex_t *mutex); ￼
int pthread_mutex_timedlock(pthread_mutex_t *mutex,￼
                            struct timespec *abs_timeout);
```

这两个调用用于获取锁，如果所已经被占用，则trylock版本失败

通常应该避免使用这两种版本，但是在研究死锁时会比较有用

**条件变量**

用于实现线程间的通信

```C
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_signal(pthread_cond_t *cond);
```

要使用条件变量，必须另外有一个与此条件相关的锁。在调用上述任何一个函数时，应该持有这个锁

- 第一个函数使得调用线程进入休眠状态，因此等待其他线程发出信号

  典型用法

  ```C
  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; ￼
  pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
  Pthread_mutex_lock(&lock);
  while (ready == 0)
    Pthread_cond_wait(&cond, &lock);
  Pthread_mutex_unlock(&lock);
  ```

  唤醒上面这个线程的代码如下：

  ```C 
  Pthread_mutex_lock(&lock); // 保有锁
  ready = 1;
  Pthread_cond_signal(&cond);
  Pthread_mutex_unlock(&lock);
  ```

- 注意到第一个函数第二个参数是锁，但是第二个函数却不需要

  这是因为 *等待调用除了使调用线程进入睡眠状态外，还会让调用者睡眠时释放锁*

  如果不释放，其他线程就无法获得这锁然后给他发信号让他唤醒

### Chapter 28 锁

并发编程的基本问题：

由于单处理器上的中断，或者是多线程在多处理器上的并发，无法保证指令被原子地执行（如`a++`,要先load，再add，最后再store）

**锁变量**

保存了锁在某一时刻的状态

- 可用的 available
- 占用的 acquired

*POSIX库将锁称为互斥量 mutex*

为了实现锁，各种计算机体系结构的指令集都增加了一些不同的硬件原语

**评价锁**

- 是否能提供互斥
- 公平性：是否每一个竞争线程有公平的机会抢到锁
- 性能
  - 没有竞争下的开支
  - 一个CPU上多个线程抢锁的开支
  - 多个CPU多个线程竞争时的性能

**控制中断** -- 锁的实现

- **在临界区关闭中断**

  可以利用特殊的硬件指令在进入临界区前关闭中断，保证临界区内指令原子地执行，然后重新打开中断

  缺点很多

  - 这种方法要求我们允许所有调用线程执行特权操作（打开关闭中断）

    因此会出现程序独占处理器或者操作系统失去控制的情况

  - 不支持多处理器

    因为多处理器上关闭中断也没用，毕竟线程可以运行在其他处理器上

  - 关闭中断导致中断丢失，可能会导致严重的系统问题
  - 效率低

- **测试并设置指令（原子交换） *test-and-set instruction***

  例如

  ```C
  int TestAndSet(int *old_ptr, int new) {￼
    int old = *old_ptr; // fetch old value at old_ptr￼
    *old_ptr = new;    // store 'new' into old_ptr￼
    return old;        // return the old value￼
  }
  ```

  > 这里我们用C代码解释，实际上测试并设置是有硬件支持的同步原语

  ```C
  typedef struct  lock_t {
    int flag;
  } lock_t;￼
  void init(lock_t *lock) {￼
    // 0 indicates that lock is available, 1 that it is held
    lock->flag = 0;
  }￼
  void lock(lock_t *lock) {￼
    while (TestAndSet(&lock->flag, 1) == 1)
      ; // spin-wait (do nothing)
  }
  void unlock(lock_t *lock) {
    lock->flag = 0;
  }
  ```

  - 首先假设一个线程在运行，调用lock()，没有其他线程持有锁，所以flag是0。当调用TestAndSet(flag, 1)方法，返回0，线程会跳出while循环，获取锁。同时也会原子的设置flag为1，标志锁已经被持有。当线程离开临界区，调用unlock()将flag清理为0。
  - 第二种场景是，当某一个线程已经持有锁（即flag为1）。本线程调用lock()，然后调用TestAndSet(flag, 1)，这一次返回1。只要另一个线程一直持有锁，TestAndSet()会重复返回1，本线程会一直自旋。当flag终于被改为0，本线程会调用TestAndSet()，返回0并且原子地设置为1，从而获得锁，进入临界区。

  - 上面的这种锁叫做 **自旋锁** 

    无法在单CPU上使用，因为自旋的线程不会放弃CPU

    满足互斥

    但是不提供任何公平性 *自旋的线程在竞争条件下可能会永远自旋。自旋锁没有公平性，可能会导致饿死*

    单CPU性能消耗大，但是多CPU性能不错

- **比较并交换 *compare-and-swap***

  C语言伪代码

  ```C
  int CompareAndSwap(int *ptr, int expected, int new) {￼
    int actual = *ptr;
   	if (actual == expected)
      *ptr = new;
   	return actual;￼
  }
  ```

  基本思路是检测ptr指向的值是否和expected相等；如果是，更新ptr所指的值为新值。否则，什么也不做。不论哪种情况，都返回该内存地址的实际值，让调用者能够知道执行是否成功

- **链接的加载和条件式存储指令 *Load-Link Conditional Store***

  ```C
  int LoadLinked(int *ptr) { 
    return *ptr;
  }￼
  int StoreConditional(int *ptr, int value) {￼
    if (no one has updated *ptr since the LoadLinked to this address) {￼
    	*ptr = value;￼
      return 1; // success!￼
    } else {￼
      return 0; // failed to update￼
  	}￼
  }
  ```

  链接的加载指令和典型的加载指令类似

  区别在于

  条件式存储指令，只有上一次加载的地址在期间都没有更新时，才会成功，（同时更新刚才链接的加载的地址的值）。成功时，条件存储返回1，并将ptr指的值更新为value。失败时，返回0，并且不会更新值。

  - 相应的锁实现

    ```c
    void lock(lock_t *lock) {￼
      while (1) {
        while (LoadLinked(&lock->flag) == 1)￼
          ; // spin until it's zero￼
        if (StoreConditional(&lock->flag, 1) == 1)￼
          return; // if set-it-to-1 was a success: all done￼
        // otherwise: try it all over again￼
         }
    }￼
    void unlock(lock_t *lock) {￼
      lock->flag = 0;￼
    }
    ```

- **获取并增加 *fetch-and-add***

  原子地返回特定地址的旧值，并且让该值自增一

  ticket锁

  ```C
  int FetchAndAdd(int *ptr) {￼
    int old = *ptr;￼
    *ptr = old + 1;
    return old;
  }￼
  typedef struct  lock_t {￼
    int ticket;
    int turn;
  } lock_t;￼
    
  void lock_init(lock_t *lock) {
    lock->ticket = 0;￼
    lock->turn = 0;￼
  }
  void lock(lock_t *lock) {￼
    int myturn = FetchAndAdd(&lock->ticket);￼
    while (lock->turn != myturn)
        ; // spin￼
  }￼
  void unlock(lock_t *lock) {￼
    FetchAndAdd(&lock->turn);￼
  }
  ```

  - 如果线程希望获取锁，==首先对一个ticket值执行一个原子的获取并相加指令==。这个值作为该线程的“turn”（顺位，即myturn）。根据全局共享的lock->turn变量，==当某一个线程的（myturn == turn）时，则轮到这个线程进入临界区==。unlock则是增加turn，从而下一个等待线程可以进入临界区

  -  本方法能够保证所有线程都能抢到锁。只要一个线程获得了ticket值，它最终会被调度。之前的方法则不会保证。比如基于测试并设置的方法，一个线程有可能一直自旋，即使其他线程在获取和释放锁

**减少自旋**

硬件+操作系统

- **第一种方法：在要自旋的时候放弃CPU**

  ```C
  void init() {￼
    flag = 0;￼
  }￼
  void lock() {￼
    while (TestAndSet(&flag, 1) == 1)
      yield(); // give up the CPU￼
  }￼
  void unlock() {￼
    flag = 0;
  }
  ```

  yield()系统调用能够让运行（running）态变为就绪（ready）态，从而允许其他线程运行。因此，让出线程本质上取消调度（deschedules）了它自己

  - 单CPU下运行良好
  - 多线程反复竞争一把锁 -- 上下文切换花销很大
  - 还要考虑一个线程一直处于让出的循环而导致饿死

- **使用队列：休眠代替自旋**

  park()能够让调用线程休眠，unpark(threadID)则会唤醒threadID标识的线程

  锁的实现：

  ```C
  typedef struct  lock_t {￼
    	int flag;￼
    	int guard;￼
    	queue_t *q;
  } lock_t;￼
  void lock_init(lock_t *m) {
    	m->flag = 0;￼
    	m->guard = 0;￼
    	queue_init(m->q);￼
  }￼   
  void lock(lock_t *m) {￼
      while (TestAndSet(&m->guard, 1) == 1)￼
         ; //acquire guard lock by spinning￼
      if (m->flag == 0) {￼
          m->flag = 1; // lock is acquired￼
          m->guard = 0;￼
    	} else {
          queue_add(m->q, gettid());
          m->guard = 0;
          park();￼
      }
  }￼
  
  void unlock(lock_t *m) {￼
      while (TestAndSet(&m->guard, 1) == 1)￼
   		   ; //acquire guard lock by spinning￼
      if (queue_empty(m->q))￼
         m->flag = 0; // let go of lock; no one wants it￼
      else￼
         unpark(queue_remove(m->q)); // hold lock (for next thread!)￼
         m->guard = 0;￼
  }
  ```

  - 我们将之前的测试并设置和等待队列结合，实现了一个更高性能的锁。

  - 我们通过队列来控制谁会获得锁，避免饿死。

  - guard基本上起到了自旋锁的作用，围绕着flag和队列操作。因此，这个方法并没有完全避免自旋等待

  - 但是以上的代码可能会导致 **唤醒/等待竞争**

    > 当一个线程发送了“唤醒”信号，但**没有任何线程在等待**，这次唤醒信号就被“浪费”了，导致**接下来真正等待的线程可能永远无法被唤醒**，进而**死锁**

    问题出在park()调用附近：如果一个线程将要park，这时切换到另一个线程，如果该线程随后释放了锁，那么第一个线程的park会永远睡下去，因为唤醒信号被浪费了

    修改是：

    - 通过setpark()，一个线程表明自己马上要park

    ```C
    queue_add(m->q, gettid());￼
    setpark(); // new code
    m->guard = 0
    ```

    - 另外一种方案就是将guard传入内核。在这种情况下，内核能够采取预防措施，保证原子地释放锁，把运行线程移出队列

**Linux采用的两阶段锁**

两阶段锁的第一阶段会先自旋一段时间，希望它可以获取锁。
但是，如果第一个自旋阶段没有获得锁，第二阶段调用者会睡眠，直到锁可用

### Chapter 29 基于锁的并发数据结构

如何给数据结果正确加锁

**懒惰计数器**

懒惰计数器通过多个局部计数器和一个全局计数器来实现一个逻辑计数器，其中每个CPU核心有一个局部计数器

- 如果一个核心上的线程想增加计数器，那就增加它的局部计数器，访问这个局部计数器是通过对应的局部锁同步的
- 因为每个CPU有自己的局部计数器，不同CPU上的线程不会竞争，所以计数器的更新操作可扩展性好
- 为了保持全局计数器更新（以防某个线程要读取该值），局部值会定期转移给全局计数器，方法是获取全局锁，让全局计数器加上局部计数器的值，然后将局部计数器置零

- 局部转全局的频度，取决于一个阈值，这里称为S（表示sloppiness）。S越小，懒惰计数器则越趋近于非扩展的计数器。S越大，扩展性越强，但是全局计数器与实际计数的偏差越大

**并发链表**

代码插入函数入口处获取锁，结束时释放锁

```C
void List_Init(list_t *L) {
     L->head = NULL;
     pthread_mutex_init(&L->lock,  NULL);
}
void List_Insert(list_t *L, int key) {
  	// synchronization not needed
  	node_t *new = malloc(sizeof(node_t));
		if (new == NULL) {
 				perror("malloc");
 				return;
   	}
  	new->key = key;

  	// just lock critical section
   	pthread_mutex_lock(&L->lock);
   	new->next = L->head;
    L->head = new;
    pthread_mutex_unlock(&L->lock);
}
int List_Lookup(list_t *L, int key) {
    int rv = -1;
		pthread_mutex_lock(&L->lock);
    node_t *curr = L->head;
    while (curr) {
        if (curr->key == key) {
            rv = 0;
            break;
        }
        curr = curr->next;
 		}
    pthread_mutex_unlock(&L->lock);
    return rv; // now both success and failure
}
```

**拓展链表**

锁耦合

- 每个节点都有一个锁

- 遍历链表的时候，首先抢占下一个节点的锁，然后释放当前节点的锁
- 但是实际上，在遍历的时候，每个节点获取锁、释放锁的开销巨大，很难比单锁的方法快。即使有大量的线程和很大的链表，这种并发的方案也不一定会比单锁的方案快。

**并发队列**

```C
typedef struct node_t {
    int value;
    struct node_t *next;
} node_t;

typedef struct queue_t {
    node_t *head;
    node_t *tail;
    pthread_mutex_t headLock;
    pthread_mutex_t tailLock;
} queue_t;

void Queue_Init(queue_t *q) {
    node_t *tmp = malloc(sizeof(node_t));
    tmp->next = NULL;
    q->head = q->tail = tmp;
    pthread_mutex_init(&q->headLock, NULL);
    pthread_mutex_init(&q->tailLock, NULL);
}

void Queue_Enqueue(queue_t *q, int value) {
    node_t *tmp = malloc(sizeof(node_t));
    assert(tmp != NULL);
    tmp->value = value;
    tmp->next = NULL;

    pthread_mutex_lock(&q->tailLock);
    q->tail->next = tmp;
    q->tail = tmp;
    pthread_mutex_unlock(&q->tailLock);
}

int Queue_Dequeue(queue_t *q, int *value) {
    pthread_mutex_lock(&q->headLock);
    node_t *tmp = q->head;
    node_t *newHead = tmp->next;

    if (newHead == NULL) {
        pthread_mutex_unlock(&q->headLock);
        return -1; // queue was empty
    }

    *value = newHead->value;
    q->head = newHead;
    pthread_mutex_unlock(&q->headLock);
    free(tmp);
    return 0;
}
```

两个锁，一个负责队列头，另一个负责队列尾。

这两个锁使得入队列操作和出队列操作可以并发执行，因为入队列只访问tail锁，而出队列只访问head锁

**并发散列表**

```C
#define BUCKETS (101)

typedef struct hash_t {
    list_t lists[BUCKETS];
} hash_t;

void Hash_Init(hash_t *H) {
    int i;
    for (i = 0; i < BUCKETS; i++) {
        List_Init(&H->lists[i]);
    }
}

int Hash_Insert(hash_t *H, int key) {
    int bucket = key % BUCKETS;
    return List_Insert(&H->lists[bucket], key);
}

int Hash_Lookup(hash_t *H, int key) {
    int bucket = key % BUCKETS;
    return List_Lookup(&H->lists[bucket], key);
}
```

每个散列桶（每个桶都是一个列表）都有一个锁，而不是整个散列表只有一个锁

### Chapter 30 条件变量

锁并不是并发程序设计的唯一原语

**条件变量**

- **基本思想**

  - 条件变量是一个显式队列，当某些执行状态（即条件，condition）不满足时，线程可以把自己加入队列，等待（waiting）该条件

  - 另外某个线程，当它改变了上述状态时，就可以唤醒一个或者多个等待线程（通过在该条件上发信号），让它们继续执行

- 声明 `pthread_cond_t c`

- 建议调用signal（也就是pthread_cond_signal()的简写）时总要持有锁 -- 为了避免一些麻烦

  调用wait时持有锁（这里不是建议，而是强制要求）

**生产者/消费者（有界缓冲区）问题**

假设有一个或多个生产者线程和一个或多个消费者线程。生产者把生成的数据项放入缓冲区；消费者从缓冲区取走数据项，以某种方式消费

- **Mesa语义**

> 当某线程调用 signal() 或 broadcast() 唤醒等待线程时，**唤醒的线程不会立即执行，而是继续排队，直到获得互斥锁后才执行**。

​	发信号只是唤醒线程，暗示状态发生了变化，但并不会保证在他运行之前状态一直是期望的情况

​	==所以使用while循环来判断是否需要等待，而不是使用if==

- **信号的指向性**

  消费者不应该唤醒消费者，反之亦然

  -- ==解决方案是使用两个条件变量==

- 提高并发和效率

  也就是增加更多的缓冲区槽位--用于容纳多个值

- 最终方案

  ```C
  int buffer[MAX];
  int fill_ptr = 0;
  int use_ptr = 0;
  int count = 0;
  
  void put(int value) {
      buffer[fill_ptr] = value;
      fill_ptr = (fill_ptr + 1) % MAX;
      count++;
  }
  
  int get() {
      int tmp = buffer[use_ptr];
      use_ptr = (use_ptr + 1) % MAX;
      count--;
      return tmp;
  }
  
  cond_t empty, fill;
  mutex_t mutex;
  
  void *producer(void *arg) {
      int i;
      for (i = 0; i < loops; i++) {
          Pthread_mutex_lock(&mutex);
          while (count == MAX)
              Pthread_cond_wait(&empty, &mutex);
          put(i);
          Pthread_cond_signal(&fill);
          Pthread_mutex_unlock(&mutex);
      }
  }
  
  void *consumer(void *arg) {
      int i;
      for (i = 0; i < loops; i++) {
          Pthread_mutex_lock(&mutex);
          while (count == 0)
              Pthread_cond_wait(&fill, &mutex);
          int tmp = get();
          Pthread_cond_signal(&empty);
          Pthread_mutex_unlock(&mutex);
          printf("%d\n", tmp);
      }
  }
  ```

**覆盖条件**

> **对等待条件（共享状态）的充分检查与保护，确保所有可能的线程唤醒情形都被正确处理。**

例如：`pthread_cond_broadcast()`代替`pthread_cond_signal()`，**唤醒所有的等待线程**

- 这样做，确保了所有应该唤醒的线程都被唤醒。
- 不利的一面是可能会影响性能，因为不必要地唤醒了其他许多等待的线程，它们本来（还）不应该被唤醒。这些线程被唤醒后，重新检查条件，马上再次睡眠

### Chapter 31 信号量

由Dijkstra及同事发明

**信号量**

信号量是有一个整数值的对象，可以用两个函数来操作它。

在POSIX标准中，是sem_wait()和sem_post()

- 初始化 

  ```C
  #include <semaphore.h>￼
  sem_t s;￼
  sem_init(&s, 0, 1);
  ```

  第二个参数是0表示该信号量可以被同一进程的多个线程共享，当然也可以是其他值（具体看手册）

  第三个参数是赋给信号量的值

- `int sem_wait(sem_t *s)`

  逐一减少信号量s的值,如果s是负的就进行等待

  - sem_wait()要么立刻返回（调用sem_wait()时，信号量的值大于等于1）
  - 要么会让调用线程挂起，直到之后的一个post操作。
  - 当信号量为负数时，这个值就是等待线程的个数

- `int sem_post(sem_t *s)`

  逐一增加信号量s的值,如果有至少一个线程在等待则唤醒一个

  - sem_post()并没有等待某些条件满足。它直接增加信号量的值

- **二值信号量 -- 用信号量表示锁** 

  ```C
  sem_t m;
  sem_init(&m, 0, X);  // X = 1 表示互斥锁，X > 1 表示多个线程可并发进入
  sem_wait(&m);
  // === 临界区开始 ===
  // 在此区域访问共享资源
  // === 临界区结束 ===
  sem_post(&m);  // V 操作，释放信号量，允许其他线程进入
  ```

  为什么初始化为1

  - 场景1

    第一个线程（线程0）调用了sem_wait()，它把信号量的值减为0。然后，它只会在值小于0时等待。因为值是0，调用线程从函数返回并继续，线程0现在可以自由进入临界区。线程0在临界区中，如果没有其他线程尝试获取锁，当它调用sem_post()时，会将信号量重置为1（因为没有等待线程，不会唤醒其他线程）

  - 场景2

    如果线程0持有锁（即调用了sem_wait()之后，调用sem_post()之前），另一个线程（线程1）调用sem_wait()尝试进入临界区，那么更有趣的情况就发生了。这种情况下，线程1把信号量减为−1，然后等待（自己睡眠，放弃处理器）。线程0再次运行，它最终调用sem_post()，将信号量的值增加到0，唤醒等待的线程（线程1），然后线程1就可以获取锁。线程1执行结束时，再次增加信号量的值，将它恢复为1

- **信号量用作条件变量**

  例子

  ```C
  sem_t s;
  void *child(void *arg) {
      printf("child\n");
      sem_post(&s);  // signal: child is done
      return NULL;
  }
  int main(int argc, char *argv[]) {
      sem_init(&s, 0, X);  // X 应为 0，表示初始时父线程需要等待子线程
      printf("parent: begin\n");
      pthread_t c;
      pthread_create(&c, NULL, child, NULL);
      sem_wait(&s);  // wait for child to signal
      printf("parent: end\n");
      return 0;
  }
  ```

  为什么初始化信号量为0？

  - 情况一

    父线程创建了子线程，但是子线程并没有运行。这种情况下，父线程调用sem_wait()会先于子线程调用sem_post()。我们希望父线程等待子线程运行。为此，唯一的办法是让信号量的值不大于0。因此，0为初值。父线程运行，将信号量减为−1，然后睡眠等待；子线程运行的时候，调用sem_post()，信号量增加为0，唤醒父线程，父线程然后从sem_wait()返回，完成该程序

  - 情况二

    子线程在父线程调用sem_wait()之前就运行结束。在这种情况下，子线程会先调用sem_post()，将信号量从0增加到1。然后当父线程有机会运行时，会调用sem_wait()，发现信号量的值为1。于是父线程将信号量从1减为0，没有等待，直接从sem_wait()返回，也达到了预期效果

- **生产者/消费者问题**

  我们把获取和释放互斥量的操作调整为紧挨着临界区，把full、empty的唤醒和等待操作调整到锁外面。结果得到了简单而有效的有界缓冲区，多线程程序的常用模式

  ```C
  sem_t empty;
  sem_t full;
  sem_t mutex;
  
  void *producer(void *arg) {
      int i;
      for (i = 0; i < loops; i++) {
          sem_wait(&empty);            // line p1
          sem_wait(&mutex);            // line p1.5 (MOVED MUTEX HERE...)
          put(i);                      // line p2
          sem_post(&mutex);            // line p2.5 (... AND HERE)
          sem_post(&full);             // line p3
      }
  }
  
  void *consumer(void *arg) {
      int i;
      for (i = 0; i < loops; i++) {
          sem_wait(&full);             // line c1
          sem_wait(&mutex);            // line c1.5 (MOVED MUTEX HERE...)
          int tmp = get();             // line c2
          sem_post(&mutex);            // line c2.5 (... AND HERE)
          sem_post(&empty);            // line c3
          printf("%d\n", tmp);
      }
  }
  
  int main(int argc, char *argv[]) {
      // ...
      sem_init(&empty, 0, MAX);  // MAX buffers are empty to begin with...
      sem_init(&full, 0, 0);     // ... and 0 are full
      sem_init(&mutex, 0, 1);    // mutex=1 because it is a lock
      // ...
  }
  ```

- **读者-写者锁**

  > 多个线程可以**同时读**资源，但当**写线程要写**时，必须**独占访问**，不能有其他读者或写者存在。

  ```C
  typedef struct _rwlock_t {
    sem_t lock;      // binary semaphore (basic lock)
    sem_t writelock; // used to allow ONE writer or MANY readers
    int    readers;  // count of readers reading in critical section
  } rwlock_t;
  
  void rwlock_init(rwlock_t *rw) {
    rw->readers = 0;
    sem_init(&rw->lock, 0, 1);
    sem_init(&rw->writelock, 0, 1);
  }
  
  void rwlock_acquire_readlock(rwlock_t *rw) {
    sem_wait(&rw->lock);
    rw->readers++;
    if (rw->readers == 1)
      sem_wait(&rw->writelock); // first reader acquires writelock
    sem_post(&rw->lock);
  }
  
  void rwlock_release_readlock(rwlock_t *rw) {
    sem_wait(&rw->lock);
    rw->readers--;
    if (rw->readers == 0)
      sem_post(&rw->writelock); // last reader releases writelock
    sem_post(&rw->lock);
  }
  
  void rwlock_acquire_writelock(rwlock_t *rw) {
    sem_wait(&rw->writelock);
  }
  
  void rwlock_release_writelock(rwlock_t *rw) {
    sem_post(&rw->writelock);
  }
  ```

  - 读者写者锁很慢 -- 性能上没有优势
  - 有些公平性上的缺陷 -- 读者很容易饿死写者

- 哲学家就餐问题

  解决问题最简单的方法，就是修改某个或者某些哲学家的取餐叉顺序

- **实现信号量**

  我们可以用锁和条件变量这些同步原语来实现信号量（包括两种函数操作）

### Chapter 32 常见并发问题

非死锁缺陷与死锁缺陷

**非死锁缺陷**

占并发问题的大多数

- 第一类：**违背原子性缺陷**

  违反了多次内存访问中预期的可串行性（即代码段本意是原子的，但在执行中并没有强制实现原子性）

- 第二类：**错误顺序缺陷**

  两个内存访问的预期顺序被打破了

**死锁缺陷**

产生条件（缺一不可）

- 互斥：线程对于需要的资源进行互斥的访问（例如一个线程抢到锁）
- 持有并等待：线程持有了资源（例如已将持有的锁），同时又在等待其他资源（例如，需要获得的锁）
- 非抢占：线程获得的资源（例如锁），不能被抢占
- 循环等待：线程之间存在一个环路，环路上每个线程都额外持有一个资源，而这个资源又是下一个线程要申请的

**预防**

- 让代码不会循环等待

  - 直接的方法就是获取锁时提供一个**全序**。假如系统共有两个锁（L1和L2），那么我们每次都先申请L1然后申请L2，就可以避免死锁。这样严格的顺序避免了循环等待，也就不会产生死锁

  - 但是全序有时很难做到，我们可以用**偏序**

- 持有并等待

  - 通过原子地抢锁

    ```C
    lock(prevention);￼
    lock(L1);￼
    lock(L2);￼
    ...￼
    unlock(prevention);
    ```

    通过锁prevention保证了在抢锁的过程中，不会有不合时宜的线程切换，从而避免了死锁

  - 但是这个方案不适合封装 -- 因为这个方案需要我们准确地知道要抢哪些锁，并且提前抢到这些锁。因为要提前抢到所有锁（同时），而不是在真正需要的时候，所以可能降低了并发

- 非抢占

  很多线程库提供更为灵活的接口来避免这种情况。

  具体来说，`trylock()`函数会尝试获得锁，或者返回−1，表示锁已经被占有

- 互斥

  但是代码都会存在临界区，所以很难避免互斥

  - 可以通过强大的硬件指令，构造出**无等待**即不需要锁的数据结构

  例如：

  ```C
  void AtomicIncrement(int *value, int amount) {￼
      do {￼
   			  int old = *value;￼
  		} while (CompareAndSwap(value, old, old + amount) == 0);￼
  }
  ```

  反复尝试将值更新到新的值。这种方式没有使用锁，因此不会有死锁（有可能产生活锁）

  ```C
  // 插入链表
  void insert(int value) {
      node_t *n = malloc(sizeof(node_t));
      assert(n != NULL);
      n->value = value;
      do {
          n->next = head;
      } while (CompareAndSwap(&head, n->next, n) == 0);
  }
  ```

  首先把next指针指向当前的链表头（head），然后试着把新节点交换到链表头。但是，如果此时其他的线程成功地修改了head的值，这里的交换就会失败，导致这个线程根据新的head值重试

- **避免**

  让需要同一把锁的线程不同时进行 -- **利用操作系统的调度**

- **检查与恢复**

  允许死锁偶尔发生，检查到死锁时再采取行动

- 活锁

  两个线程有可能一直重复这一序列，又同时都抢锁失败。这种情况下，系统一直在运行这段代码（因此不是死锁），但是又不会有进展，因此名为**活锁**。

  也有活锁的解决方法：例如，可以在循环结束的时候，先随机等待一个时间，然后再重复整个动作，**这样可以降低线程之间的重复互相干扰**

### Chapter 33 基于事件的并发

基于事件的并发是有别于线程实现并发的另一种策略

基本想法：**事件循环** 我们等待某事（即“事件”）发生；当它发生时，检查事件类型，然后做少量的相应工作

```C
while (1) {￼
  events = getEvents(); ￼
  for (e in events)￼
    processEvent(e);￼
}
```

**API**

- ```C
  int select(int nfds,￼
             fd_set *restrict readfds, ￼
             fd_set *restrict writefds, ￼
             fd_set *restrict errorfds,￼
             struct timeval *restrict timeout);
  ```

  select()检查**I/O描述符集合**，它们的**地址通过readfds、writefds和errorfds传入**，分别查看它们中的某些描述符是否已准备好读取，是否准备好写入，或有异常情况待处理。**在每个集合中检查前nfds个描述符**，即检查描述符集合中从0到nfds-1的描述符。返回时，select()用给定请求操作准备好的描述符组成的子集替换给定的描述符集合。**select()返回所有集合中就绪描述符的总数**

  时间参数传入NULL表示select()无限期地阻塞，直到某个描述符准备就绪

- `poll()` 与select相似

**使用select()**

例子

```C
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    // open and set up a bunch of sockets (not shown)
    // main loop
    while (1) {
        // initialize the fd_set to all zero
        fd_set readFDs;
        FD_ZERO(&readFDs);

        // now set the bits for the descriptors
        // this server is interested in
        // (for simplicity, all of them from min to max)
        int fd;
        for (fd = minFD; fd < maxFD; fd++)
            FD_SET(fd, &readFDs);

        // do the select
        int rc = select(maxFD + 1, &readFDs, NULL, NULL, NULL);

        // check which actually have data using FD_ISSET()
        for (fd = minFD; fd < maxFD; fd++)
            if (FD_ISSET(fd, &readFDs))
                processFD(fd);
    }
}
```



我们在基于事件的系统中必须遵守一条规则：**不允许阻塞调用**

- 也就是不可以有阻止调用者执行的调用
- 因为一次只处理一个事件，所以不需要获取或释放锁。基于事件的服务器不能被另一个线程中断，因为它确实是单线程的
- 如果阻塞，系统将处于闲置状态而浪费资源

*利用异步I/O解决*

一些接口使应用程序能够发出I/O请求，并在I/O完成之前立即将控制权返回给调用者，另外的接口让应用程序能够确定各种I/O是否已完成。

- 在没有异步I/O的系统中，纯基于事件的方法无法实现
- 但是可以使用线程池来管理未完成的I/O



**手工栈管理**

事件处理程序发出异步I/O时，它必须打包一些程序状态，以便下一个事件处理程序在I/O最终完成时使用

- 在某些数据结构中，记录完成处理该事件需要的信息。当事件发生时（即磁盘I/O完成时），查找所需信息并处理事件



注意：当转向**多个CPU时，基于时间的并发会比较复杂**

因为它不能跟某些类型的系统活动集成，如分页



**阻塞**（或同步，synchronous）接口在返回给调用者之前完成所有工作

**非阻塞**（或**异步**，asynchronous）接口开始一些工作，但立即返回，从而让所有需要完成的工作都在后台完成

## 持久性

### Chapter 36 I/O设备

**标准协议**

- 简化的设备接口

  - 状态寄存器
  - 命令寄存器
  - 数据寄存器

  通过读写这些寄存器，操作系统可以控制设备的行为

- 协议 4步

  1. **轮询 *polling*** 操作系统通过反复读取状态寄存器，等待设备进入可以接收命令的就绪状态

  2. 操作系统下发数据到数据寄存器
  3. 操作系统将命令写入命令寄存器；这样设备就知道数据已经准备好了，它应该开始执行命令
  4. 操作系统再次通过不断轮询设备，等待并判断设备是否执行完成命令（有可能得到一个指示成功或失败的错误码）

  - 由于轮询而效率低下

- 利用**中断**减少开销

  有了中断，CPU 不再需要不断轮询设备，而是向设备发出一个请求，然后就可以让对应进程睡眠，切换执行其他任务。

  当设备完成了自身操作，会抛出一个硬件中断，引发CPU跳转执行操作系统预先定义好的中断服务例程

  中断处理程序是一小段操作系统代码，它会结束之前的请求（比如从设备读取到了数据或者错误码）并且唤醒等待I/O的进程继续执行

  **提升效率的关键是：**中断允许CPU计算和I/O重叠（*overlap*）

- 最佳实践

  如果设备非常**快**，那么最好的办法反而是**轮询**。如果设备比较**慢**，那么采用允许发生重叠的**中断**更好。

  如果设备的速度未知，或者时快时慢，可以考虑使用**混合**（hybrid）策略，先尝试轮询一小段时间，如果设备没有完成操作，此时再使用中断

- **基于中断的优化 -- 合并**

  设备在抛出中断之前往往会**等待一小段时间**，在此期间，其他请求可能很快完成，**因此多次中断可以合并为一次中断抛出**，从而降低处理中断的代价

  

**利用DMA高效传输数据**

如果使用编程的I/O将一大块数据传给设备，CPU又会因为琐碎的任务而变得负载很重，浪费了时间和算力

解决方案就是使用DMA *Direct Memory Access* -- 不需要CPU介入

- 工作过程

  操作系统会通过编程告诉DMA引擎数据在内存的位置，要拷贝的大小以及要拷贝到哪个设备。

  在此之后，操作系统就可以处理其他请求了。

  当DMA的任务完成后，DMA控制器会抛出一个中断来告诉操作系统自己已经完成数据传输。



**硬件与设备通信**

- 第一种方法：明确的I/O指令 （通常是特权指令）

- 第二种方法：内存映射I/O

  硬件将设备寄存器作为内存地址提供。当需要访问设备寄存器时，操作系统装载（读取）或者存入（写入）到该内存地址；然后硬件会将装载/存入转移到设备上，而不是物理内存



**设备驱动程序**

> **把具体、复杂、多样的硬件设备抽象为一套统一的软件接口**，使得应用程序和上层操作系统不需要关心硬件的细节



**可编程的I/O PIO *programmed I/O***

> 如果主CPU参与数据移动，我们就称之为编程的I/O（programmed I/O，PIO）



### Chapter 37 磁盘驱动器

旋转延迟 *rotational delay* :等待期望的扇区旋转到磁头下

寻道时间 

停放时间 *settling time* ：在磁头完成寻道后，磁头停止在目标磁道上并稳定下来的时间，使得磁头能可靠地进行数据读写

磁道偏斜 *track skew* :以确保即使在跨越磁道边界时，顺序读取也可以方便地服务

$T~I/O~ = T~寻道~ + T~旋转~ + T~传输~$



工作负载

- 随机工作负载 -- 向随机位置发出读取请求
- 顺序工作负载 -- 从磁盘连续读取大量扇区，不会跳过

显然顺序读取更快



磁盘调度

给定一组I/O请求，磁盘调度程序检查请求并决定下一个要调度的请求

磁盘调度程序将尝试在其操作中遵循SJF 最短任务优先

- 早期的磁盘调度方法 -- 最短寻道时间优先 SSTF *Shortest-Seek-Time-First*

  问题

  - 一：主机操作系统无法利用驱动器的几何结构，而是只会看到一系列的块
  - 二：极饿 如果对磁头当前所在位置的内圈磁道有稳定的请求。然后，纯粹的SSTF方法将完全忽略对其他磁道的请求

  解决 -- 电梯算法 SCAN

  - 简单地以跨越磁道的顺序来服务磁盘请求。我们将一次跨越磁盘称为扫一遍。因此，如果请求的块所属的磁道在这次扫一遍中已经服务过了，它就不会立即处理，而是排队等待下次扫一遍
  - 变体C-SCAN 即循环SCAN
    - 不是在一个方向扫过磁盘，该算法从外圈扫到内圈，然后从内圈扫到外圈，如此下去
  - 并不是最好的调度技术，特别是SCAN没有严格遵循SJF的原则

- 最短定位时间优先 SPTF *Shortest Positioning Time First*

  > **优先处理定位时间（寻道 + 停放时间）最短的 I/O 请求，以最小化平均访问延迟。**

- 调度程序还应该合并I/O请求从而降低开销

- 非工作保全

  通过等待，新的和“更好”的请求可能会到达磁盘，从而整体效率提高

  相应的 工作保全就是 即使有一个磁盘I/O，也应立即向驱动器发出请求

### Chapter 38 廉价冗余磁盘阵列 RAID

冗余使得数据更佳可靠

而且使用多个磁盘可以大大加快I/O时间

容量也可以更大

RAID对于主机是透明的，也就是看起来像一块大磁盘

评估要点：**容量，可靠性和性能**

我们假设磁盘可以在连续工作负载下以**S** MB/s传输数据，并且在随机工作负载下以**R** MB/s传输数据。一般来说，S比R大得多

**RAID 0级：条带化**

没有冗余，就是多个磁盘的组合，每个磁盘被分为块，按照顺序从磁盘0一直到磁盘n排列形成跨越磁盘的条带

映射：给定逻辑地址A

- 磁盘 = A % 磁盘数
- 偏移量 = A / 磁盘数

大块

- 较小的大块意味着许多文件将跨多个磁盘进行条带化，从而增加了对单个文件的读取和写入的并行性。但是，跨多个磁盘访问块的定位时间会增加
- 较大的大块减少了这种文件内的并行性，因此依靠多个并发请求来实现高吞吐量。但是，较大的大块大小减少了定位时间

*任何磁盘的损坏都会导致数据丢失*

性能

- 循序访问的性能很好 -- 大部分时间都在传输数据上
- 随机访问 -- 大部分时间都在寻道和旋转等待上

吞吐量

- 等于 $N \times S$ 
- N是磁盘数量
- S时单个磁盘的顺序带宽

**RAID 1级：镜像**

也就是说要生成数据的副本，这些副本保存在一个单独的磁盘上

读取 -- 可以读取任意副本

更新 -- 必须全部更新

容量 -- 对于N个磁盘，有效容量为 $\frac{N}{2}$

可靠性 -- 容许一个磁盘故障

性能 

- 读取性能跟单个磁盘上的延迟相同 -- 所有RAID-1都会读取导向一个副本

  - 顺序读取带宽是 $\frac{N}{2} \times S$ 
  - 因为要考虑到块副本不会带来有效的带宽
  - 随机读取带宽是 $N \times R$ 
- 但是写入需要完成两次物理写入，并行进行，时间大致等于单次写入时间

  - 然而逻辑写入必须等待两个物理写入的完成，所以它遭遇到两个请求中最差的寻道和旋转延迟，因此（平均而言）比写入单个磁盘略高
  - 顺序写入带宽是 $\frac{N}{2} \times S$ 
  - 随机写入带宽是 $\frac{N}{2} \times R$ 

**RAID 4级 ：通过奇偶校验节省空间**

利用XOR对一个条带的每个位进行运算

- 1的数量为偶数，结果为0
- 1的数量为奇数，结果为1

这样的话，如果某一位丢失，就可以根据奇偶校验位进行数据重构



容量：由于一个磁盘用来存储条带的奇偶校验位，故为N - 1

可靠性：容许一个磁盘故障，如果更多则无法进行数据重建

性能：

连续读取可以利用除奇偶校验磁盘以外的所有磁盘 带宽为  $（N-1） \times S$ 

随机读取 $（N-1） \times R$

顺序写入 -- 全条带写入 $（N-1） \times S$

- 此时RAID可以简单的重新计算校验位的新值，然后并行写入

随机写入 

- 注意要更新数据的时候同时更新 *奇偶校验位*

- 第一种方法是 *加法奇偶校验*

  - 并行读取条带中所有其他数据块，然后跟新块进行异或
  - 需要进行大量读取

- 第二种方法是 *减法奇偶校验*

  - 比较旧数据和新数据 -- 相同的话校验位不变，否则反转
  - 表达式 $P_{\text{new}} = (C_{\text{old}} ⊕ C_{new} ) ⊕ P_{old}$
  - 由于我们处理的是块而不是位，所以需要对块中的所有位进行这样的计算

- 选择的交叉点在 $N > 4$ 时使用减法 -- 使得I/O数目尽可能减少

- 性能分析

  - 奇偶校验磁盘是性能瓶颈 -- 被称为 *小写入问题 small-write problem*

    > 当系统执行 **小规模写入操作（如修改几个块）** 时，**由于需要额外的读改写和校验更新操作**，会导致写入性能显著下降。

  - 奇偶检验磁盘不会实现任何并行，必须为每个逻辑I/O执行两次I/O，一次读取，一次写入
  - 得到带宽为$\frac{R}{2}$

**RAID 5级：旋转奇偶校验**

基本同RAID-4，但是奇偶校验位没有都在一个磁盘上，而是类似旋转的分布在不同磁盘上的块中

基本的性能没有差别

但是随机读取性能稍微好一点

*随机写入性能明显提高* -- 因为校验位的旋转分布使得并行成为可能

- 如果有大量的随机请求，我们将能够保持所有磁盘均匀忙碌。如果是这样的话，那么我们用于小写入的总带宽将是￼·R MB/s。4倍损失是由于每个RAID-5写入仍然产生总计4个I/O操作，这就是使用基于奇偶校验的RAID的成本

### Chapter 39 插叙：文件和目录

存储虚拟化

- 两个抽象
  - **文件**file -- 低级名称通常称为inode *index* *node* 号
  - **目录**directory --也有低级的inode号，并且包含（ 用户可读名字，低级名字）对的列表

文件系统接口

**创建文件**

`int fd = open("foo", O_CREAT | O_WRONLY | O_TRUNC);`

| **标志** | **说明**                                |
| -------- | --------------------------------------- |
| O_RDONLY | 只读打开                                |
| O_WRONLY | 只写打开                                |
| O_RDWR   | 读写打开                                |
| O_CREAT  | 如果文件不存在就创建                    |
| O_EXCL   | 与 O_CREAT 一起使用，若文件已存在则失败 |
| O_TRUNC  | 打开时清空原文件内容（与写模式一起用）  |
| O_APPEND | 写入时追加到文件末尾                    |

>  旧式的创建方法是 调用 `creat()`，可以看做`open()`加上标识 O_CREAT | O_WRONLY | O_TRUNC

`open()`返回的是这个文件的文件描述符，每个进程私有，用于系统的访问



**读取文件**

`ssize_t read(int fd, void *buf, size_t count);`

第一个参数是文件描述符

第二个参数指向一个用于放置读取结果的缓冲区

第三个参数是缓冲区的大小

返回读取到的字节数



**写入文件**

`ssize_t write(int int fd, const void *buf, size_t count);`

参数：文件描述符，要写入的缓冲区，写入的字节数

对于较大的文件，可能重复调用write(),然后调用close()



**随机的读取和写入**

有时可能会从文件中的某些随机偏移量中读取数据

我们使用`lseek()`系统调用

`off_t lseek(int fildes, off_t offset, int whence);`

- 第一个参数是文件描述符

- 第二个参数是偏移量

- 第三个参数是搜索方式

  | **值**   | **含义**                   |
  | -------- | -------------------------- |
  | SEEK_SET | 从文件开头偏移 offset 字节 |
  | SEEK_CUR | 从当前位置偏移 offset 字节 |
  | SEEK_END | 从文件末尾偏移 offset 字节 |

  - 由此可见，对于每个进程打开的文件，操作系统都会跟踪一个当前的偏移量，以此来决定在文件中读取或写入时，下一次读取或写入开始的位置

- 成功：返回移动后的偏移量（字节数，类型是 off_t）

  失败：返回 -1，并设置 errno

- 偏移量的更新方式

  - 第一种：当发生N个字节的读或者写时，N被添加到当前偏移 -- 每次读取或写入都会隐式更新偏移量
  - 第二种：明确的`lseek()`调用改变偏移量



**立即写入**

- 大多数情况下，`write()`调用会将需要写入的内容写入在内存中的缓冲区一段时间，然后才会实际发送到存储设备
- 但是一些应用如 *数据库* 需要立即强制写入磁盘
- 使用 `fsync(int fd)`系统调用
  - 调用后，文件系统强制将所有脏数据写入磁盘
  - 写入完成后返回



**文件重命名**

`int rename(const char *oldpath, const char *newpath);`

这通常是一个原子的调用



**获取文件信息**

- 文件元数据 -- 关于文件的一些基本信息

- 使用 `stat()` 或 `fstat()`查看文件元数据

  这些调用会填充一个如下的数据结构

  ```C
  struct stat {
      dev_t     st_dev;     /* ID of device containing file */
      ino_t     st_ino;     /* inode number */
      mode_t    st_mode;    /* protection (file type and mode) */
      nlink_t   st_nlink;   /* number of hard links */
      uid_t     st_uid;     /* user ID of owner */
      gid_t     st_gid;     /* group ID of owner */
      dev_t     st_rdev;    /* device ID (if special file) */
      off_t     st_size;    /* total size, in bytes */
      blksize_t st_blksize; /* blocksize for filesystem I/O */
      blkcnt_t  st_blocks;  /* number of blocks allocated */
      time_t    st_atime;   /* time of last access */
      time_t    st_mtime;   /* time of last modification */
      time_t    st_ctime;   /* time of last status change */
  };
  ```

- 以上的信息被文件系统保存在 **inode** 中



**删除文件**

`unlink()`



**创建目录**

永远不能直接写入目录

因为目录的格式被视为文件系统元数据

所以只能间接更新

调用`mkdir()`创建目录

*空目录也有两个条目*

- 一个是引用自身的条目 “.”
- 一个是引用其父目录的条目 “..”



读取条目

- 打开

  ```C
  #include <dirent.h>
  DIR *opendir(const char *name);
  ```

  参数是路径（可以使用 “.” “..”）

  成功返回一个目录指针

  失败返回NULL, 并设置errno

- 读取

  ```C
  struct dirent *readdir(DIR *dirp);
  ```

  读取目录下一项

  成功 返回一个结构指针

  ```C
  struct dirent {
      ino_t          d_ino;       // inode号
      off_t          d_off;       // 偏移量
      unsigned short d_reclen;    // 目录项长度
      unsigned char  d_type;      // 文件类型
      char           d_name[256]; // 文件名（最长255字节+1终止符）
  };
  ```

  读到末尾或出错：返回 NULL

- 关闭

  `int closedir(DIR *dirp);`

  成功：返回 0

  失败：返回 -1，并设置 errno



**删除目录**

`rmdir()`



**硬链接**

- `link()`调用 -- 一个参数是旧路径名，另一个是新路径名
- 可以在命令行执行ln达到同样效果
- 原理
  - link只是在要创建链接的目录中创建了另一个名称
  - 并将其指向原有文件的相同inode号
  - 该文件不以任何方式复制
- 相应的删除调用 `unlink()`
  - 计数引用 -- 允许文件系统跟踪有多少不同的文件名已链接到这个inode
  - 调用unlink()时，会删除人类可读的名称与给定inode号之间的“链接”
  - 并减少引用计数
  - 只有当引用计数达到零时
  - 文件系统才会释放inode和相关数据块，从而真正“删除”该文件
- 不能创建目录的硬链接 -- 可能会在目录树中形成一个环
- 不能跨越磁盘分区

**符号链接** -- 软链接

命令行下使用 `ln -s` 创建软链接

形成符号链接的方式，就是**将链接指向文件的路径名作为链接文件的数据**

所以**除符号链接（symlink）不会删除原始文件**

- 符号链接本身实际上是一个不同类型的文件，它是文件系统知道的第三种类型*symbolic link*

- 符号链接可能造成悬空引用

  删除原始文件会导致符号链接指向不再存在的路径



**创建并搭载文件系统**

创建文件系统

- 使用工具 *mkfs*
- 为该工具提供 **一个设备（例如磁盘分区 /dev/sda1）** 和 **一种文件系统类型（如：ext3）**
- 即可工作

挂载文件系统

- 使用工具mount (底层调用`mount()`)
- 以现有目录作为目标挂载点，本质上是将新的文件系统粘贴到目录树的这个点上

文件系统类型

- ext3 标准的基于磁盘的文件系统
- proc 用于访问当前进程信息的文件系统
- tmpfs 仅用于临时文件的文件系统
- AFS 分布式文件系统

### Chapter 40 文件系统实现

**VSFS** *Very Simple File System* 简单文件系统

文件系统是纯软件

- 第一个方面 ： 文件系统的数据结构 -- 文件系统在磁盘上使用哪些类型的结构来组织数据和元数据

  - **首先将磁盘分块**

    - 绝大部分用于存储用户数据 -- D区

    - 一部分用于存放 inode -- 这部分称为inode表 I区

    - 还有一部分来记录inode或者数据块是空闲还是已经分配 -- 使用位图 *bitmap*

      > 每个位用于指示相应的对象/块是空闲的还是正在使用的

    - 最后一部分是 超级块 -- 包含特定文件系统的信息 ：如inode数量，数据块数量还有inode表的开始位置，文件系统类型标识等

    - 挂载文件系统时，操作系统将首先读取超级块，初始化各种参数，然后将该卷添加到文件系统树中。当卷中的文件被访问时，系统就会知道在哪里查找所需的磁盘上的结构

  - **设计inode结构**

    - inode被我们之前提到的文件低级名称隐式引用，我们称之为 inumber

    - 根据inumber我们可以很简单的在inode表中定位inode -- 表起始地址+offset（$inumber \times \text{inode-size}$ ）

    - **决定inode如何引用数据块的位置**

      - 方法1:在inode中有一个或者多个直接指针(磁盘地址)，每个指针指向属于该文件的一个磁盘块

        但是在指向很大的文件时就比较困难（一个指针指向一个块，但是块可能有很多）

      - 方法2:多级索引 -- 使用间接指针指向包含更多指针的块，这个块里每个指针再指向数据

        这样就可以支持大文件了

        还可以使用双重间接指针 -- 它指向一个包含间接指针的块

        这种**不平衡树**被称为指向文件块的多级索引方法

        所谓不平衡树，也即是到达数据块的层级深度是不一样的，可能是1级，也可能是三级

        为什么用不平衡树

        原因是大多数文件很小，仅有少部分文件很大，所以可以优化小文件快速访问，同时支持大文件

      典型inode的内容

      | **字段**   | **含义**                          |
      | ---------- | --------------------------------- |
      | st_ino     | inode 编号（唯一标识文件）        |
      | st_mode    | 文件类型和权限（rwx 位）          |
      | st_uid     | 所有者用户 ID                     |
      | st_gid     | 所有者组 ID                       |
      | st_nlink   | 硬链接数量                        |
      | st_size    | 文件大小（字节）                  |
      | st_atime   | 最后访问时间                      |
      | st_mtime   | 最后修改时间（内容）              |
      | st_ctime   | 状态变更时间（权限/链接数变化等） |
      | st_blocks  | 占用的磁盘块数                    |
      | st_blksize | 建议的 I/O 块大小                 |
      | st_rdev    | 如果是设备文件，这里是设备编号    |
      | st_dev     | 所在设备编号（哪个磁盘分区）      |

  - **目录组织**

    - 目录基本只包含一个列表，每一项是一个二元组$(条目名称，inumber)$
    - 对于给定目录的每个文件或者目录，目录的数据块中都有一个字符串和一个数字，对于每个字符串可能还有一个长度
    - 删除一个文件会在目录中间留下空白（一般用保留的inode号0来标记），这是便于新条目使用旧的更大的条目，节省空闲分配的开支
    - 目录是特殊类型的文件 -- **也有inode，也有inode指向的数据块**
    - 当然也可以用其他数据结构组织目录

  - **空闲空间管理**

    可以用位图，空闲列表

    - 当我们创建一个文件时，文件系统通过位图搜索空闲内存并分配
    - 然后标记为已使用

- 第二个方面：访问方法

  访问路径

  - 读取磁盘文件

    发出open调用时

    - 文件系统先找到相应文件的inode -- 由于只有路径名，所以必须遍历所有路径名从而找到所有的inode

      文件遍历从根目录开始，跟目录的inode号为文件系统的已知量，这里是2

      下一步递归遍历路径名

    - 找到后将文件的inode读入内存
    - 操作系统进行权限检查
    - 在进程的打开文件表中，为此进程分配一个文件描述符，返回给用户

    发出read调用

    - 第一次读取将在文件的第一个块中开始（除非lseek()已经被调用，则在偏移量为0处）
    - 查阅inode以找到这个块的位置
    - 然后用新的最后访问时间更新inode 			注意：分配结构如位图只会在需要分配时才会访问 读取请求不会访问
    - 最后释放文件描述符
    - 不会发生磁盘I/O

    open导致的I/O量与路径名的长度成正比。对于路径中的每个增加的目录，我们都必须读取它的inode及其数据

  - 写入磁盘

    写入文件将导致5个I/O

    - 一个读取数据位图 ：然后更新来标记被分配的块
    - 一个写入位图：将他的新状态存入磁盘
    - 一次读取inode
    - 一次写inode：更新块的位置
    - 真正写入数据块本身

    创建一个文件时文件系统不仅要分配一个inode，还要在包含新文件的目录中分配空间

    仅仅是创建一个新文件就需要大量的工作

    *如何降低成本*

    - 采用系统内存DRAM来缓存重要的块

      早期文件系统用固定大小的内存来缓存，这种静态划分可能导致浪费

      现代系统采用**动态划分**：将虚拟内存页面和文件系统页面统一到页面缓存中

      读取缓冲可以减少打开统一文件的开销

      尽管缓存无法减少写入开销，写缓冲依然有很多优点 -- 通过拖延/懒惰来节省I/O

### Chapter 41 局部性和快速文件系统

上一章描述的VSFS具有较差的性能，而且会导致文件系统的碎片化

我们搭建了更好的文件系统

**FFS *Fast File System*** 快速文件系统

- 更改磁盘结构

  - FFS将磁盘划分为 **柱面组** 这样的小组 （也叫 **块组**）

  - 通过在同一组中放置两个文件，FFS可以确保先后访问两个文件不会导致穿越磁盘的长时间寻道

  - 因此我们对磁盘的设计如下

    - 在**每个组**中我们延续了VSFS的整体设计思路：超级块+bitmap+I区+D区

      每个组都有超级块的副本，所以提供了更好的可靠性

      但是我们将位图分成了两部分 ：ib *inode bitmap* 和 db *data bitmap* 分别针对I区和D区

- 分配文件与目录的策略

  - 目录的放置

    找到分配数量少的柱面组：我们希望跨组平衡目录

    找到大量自由的inode：我们希望随后能分配一堆文件

    然后将目录数据和inode放在该分组中

  - 文件放置

    确保文件数据块分配到与其inode相同的组中，从而防止inode和数据之间长时间的寻道

- 例外 -- 大文件

  为何例外 -- 因为如果没有不同的规则，大文件将填满它首先放入的块组，这将妨碍随后相关的文件放在该组中，从而破坏了文件访问的局部性

  我们执行以下操作

  - 先将一定数量的块分配到一个块组
  - 然后将文件的下一个大块分配到另一个块组
  - 知道分配完毕

  如果大块足够小，我们的大部分时间仍然在传输数据上，相对减少了在大块之间寻道的时间

  - 如果块太大，且磁盘空间不够连续，容易被分配到**不相邻的磁道** ➜ 增加寻道频率

- 其他

  - 对于小文件 -- 指的是那种大小小于每个块大小的文件

    这种会导致空间利用率不高

    引入 **子块** 来解决这个问题

    原理是：随着文件的增长，当文件大小达到正常块大小时，将这一整个块复制到另一个块，释放子块来备用

    FFS修改libc库来将缓冲写入，然后通过正常块的形式发送到文件系统，从而避免使用子块带来的大量额外I/O开销

  - 优化磁盘布局

    在传统的磁盘布局上，顺序读取会出现问题：FFS请求块0，读取完成后发出块1的请求时，块1已经在磁头下方旋转，所以真正读取需要导致完全旋转

    FFS通过每次跳过一个块来使得有足够的时间发出请求

    实际上FFS可以确定特定磁盘在布局时应跳过多少块避免额外的旋转 -- 这叫 **参数化**

    FFS可以找出磁盘的特定性能参数，并利用他们来确定准确的交错布局方案

### Chapter 42 崩溃一致性：FSCK和日志

文件系统的另一个挑战是：如何在出现断电或者系统崩溃时更新持久数据结构

也即是 **崩溃一致性问题**

我们以追加内容作为工作负载来讨论

追加文件我们需要更新的内容是

- inode ：必须指向新块
- 新数据块Db *data block*
- 新版本的数据位图

我们之前讨论过：wirte()调用后不会立即写入，而是写入缓冲区

崩溃场景

- 只将数据块写入磁盘

  这时没有指向它的inode，也没有表示块已分配的位图

  所以就是 “如写”，没有任何问题

- 只更新inode

  可能读到垃圾数据

  或者遇到 **文件系统不一致** ：位图告诉我们这块没被分配，但是inode说他已经被分配了

- 只更新位图

  位图指示分配块，但是没有指向他的inode，文件系统再次不一致，这种写入会导致空间泄露

- 更新位图+inode

  指示正常，但是数据是垃圾

- 更新inode和数据块

  inode和位图的旧版本之间存在不一致

- 更新位图和数据块

​	inode和位图的旧版本之间存在不一致

#### **fsck *file system checker*** 文件系统检查程序

基本上：让不一致的事情发生，然后再修复他们

Unix有一个工具，fsck，用于查找不一致并修复

它在文件系统挂载并可用之前运行

- fsck先检查**超级块**是否合理：进行健全性检查，目的是找到可疑的超级块，然后系统可以决定使用超级块的备用副本

- 然后fsck扫描inode、间接块、双重间接块等，**生成正确版本的位图**

  如果位图和inode之间不一致，通过信任inode来解决

- 接着检查每个**inode是否存在损坏或其他问题**，如果存在且不易修复，则会被fsck清除，位图相应更新

- **验证每个inode的链接数**：通过遍历目录树，计算链接数，如果不匹配，则修复inode中的计数

  如果发现已分配但没有目录引用的inode，则会移动到lost+found目录

- fsck还将**检查重复指针**，即两个inode共用一个块的情况，如果一个inode明显不好，可能会被清除。或者，可以复制指向的块，从而根据需要为每个inode提供其自己的副本
- fsck在扫描所有指针时还有**检查坏块指针**，所谓坏块就是明显超出有效范围的块，在这种情况下，fsck不能做任何太聪明的事情。它只是从inode或间接块中删除（清除）该指针。
- fsck对每个**目录的内容执行额外的完整性检查**，确保“.”和“..”是前面的条目，目录条目中引用的每个inode都已分配，并确保整个层次结构中没有目录的引用超过一次。

🥲但是这种很慢



#### **日志记录 *journaling*** -- 这是从数据库管理系统借鉴的

数据日志基本思路

- 在真正修改文件系统的数据前，**先将修改操作写入日志**（磁盘特定的块，位置很容易找到），

- 然后再实际更新数据。

- 如果系统崩溃，可以通过**日志记录进行恢复**，保持一致性

日志结构 -- 5个块

- TxB事务开始：告诉我们有关此次更新的信息以及事务标识符 **TID *transaction identifier***
- 中间三个块依次是 inode，位图，数据块更新的确切物理信息
- 最后一个块是TxE，标识事务结束，也包含TID

操作顺序：

1. 日志写入
2. 加检查点：将待处理的元数据和数据更新写入文件系统中的最终位置

如果在写入日志期间发生崩溃：

​	理想情况下，我们希望一次发出所有 5 个块写入，因为这会将 5 个写入转换为单个顺序写入，因此更快

​	但是不安全：给定如此大的写入，磁盘内部可以执行调度并以任何顺序完成大批写入的小块

​	但是如果在中间发生崩溃

​	日志可能有完整的开始和结束标识

​	但是中间的数据可能由于崩溃而是垃圾

为了避免这个问题

- 首先，它将除TxE块之外的所有块写入日志，同时发出这些写入

- 当这些写入完成时，文件系统会发出TxE块的写入，从而使日志处于最终的安全状态

- 此过程中磁盘提供了原子性保证：磁盘保证任何512字节写入都会发生或不发生

  因此为了确保TxE的写入是原子的，应该使它成为一个512字节的块

新的操作步骤：

1. 日志写入
2. 日志提交：将事务提交块（包括TxE）写入日志，等待写完成，事务被认为已提交
3. 加检查点

OS利用日志从崩溃中恢复

- 如果崩溃发生在第二步完成之前：我们抛弃这个事务
- 如果事务提交后发生崩溃：我们可以恢复 -- 重做日志
  - 系统引导时，文件系统恢复过程将扫描日志，并查找已提交到磁盘的事务
  - 然后，这些事务被重放，文件系统再次尝试将事务中的块写入它们最终的磁盘位置

批处理日志更新

​	可以将所有更新缓冲到全局事务中

​	通过缓冲更新，文件系统在许多情况下可以避免对磁盘的过多的写入流量

日志大小有限问题

​	当日志已满（或接近满）时，不能向磁盘提交进一步的事务

​	**日志文件系统将日志视为循环数据结构，一遍又一遍地重复使用**。这就是为什么日志有时被称为循环日志	

​	为此，一旦事务被加检查点，文件系统应释放它在日志中占用的空间，允许重用日志空间

​	可以在日志超级块中标记最旧和最新的事务

**最终的数据日志协议**

1. 日志写入
2. 日志提交
3. 加检查点
4. 释放：一段时间后，通过更新日志超级块，在日志中标记该事务为空闲

**元数据日志**

- 每次写入磁盘，我们现在也要先写入日志，从而使写入流量加倍
- 在写入日志和写入主文件系统之间，存在代价高昂的寻道，这为某些工作负载增加了显著的开销

-  **先前写入日志的数据块Db将改为写入文件系统，避免额外写入**

​	数据写入的顺序对于仅元数据日志很重要

​	因为如果Db没有写入磁盘，但是这里文件系统是一致的，所以可能指向垃圾数据

​	为了确保不出现这种情况，在将相关元数据写入磁盘之前，一些文件系统（例如，Linux ext3）先将数据块写入磁盘

**元数据日志协议**

1. 数据写入

2. 日志元数据写入

3. 日志提交

4. 加检查点元数据

5. 释放

   唯一真正的要求，是在发出日志提交块之前完成步骤1和步骤2

特殊情况：块复用

- 可能在日志中复用了之前释放了的部分，而导致错误
- 解决方法是：记录 删除目录这样的 **撤销记录**，这样的被撤销的数据都不会被系统重放

#### 其他方法

**写时复制 COW *Copy-On-Write***

这种技术永远不会覆写文件或者目录

他会对磁盘上从未使用的位置进行更新，然后会翻转文件系统的根结构（更新根指针），以包含指向刚更新结构的指针

**反向指针一致性** BBC *Backpointer-Based Consistency*

它在写入之间不强制执行排序。为了实现一致性，系统中的每个块都会添加一个额外的反向指针

访问文件时，文件系统可以检查正向指针（inode或直接块中的地址）是否指向引用它的块，从而确定文件是否一致

**乐观崩溃一致性**

尽可能多的向磁盘发出写入，并利用事务校验和等形式检验不一致

### Chapter 43 日志结构文件系统

#### 背景

- 内存大小不断增长
- 随机I/O性能与顺序I/O性能之间存在巨大的差距且不断扩大
- 现有文件系统在许多常见的工作负载上表现不佳
- 文件系统不支持RAID

#### 日志结构文件系统 LFS *Log-structured File System*

LFS **永远不会覆写现有数据，而是始终将段写入空闲位置，由于段很大，因此可以有效地使用磁盘，并且文件系统的性能接近其峰值**

挑战是：如何将文件系统功能状态的所有更新转变为对磁盘的一系列顺序写入

- 我们在写入数据块的后面紧接着写入其元数据inode，然后新的写入紧随其后

另外考虑磁盘结构，简单的顺序写入不足以实现最佳性能

- 采用**写入缓冲**技术

  在写入磁盘前跟踪内存中的更新，达到一定数目后再写入

  每次写入的大块更新叫做段

  实际的段中更新数目由磁盘决定

如何查找inode？

- 在LFS中，inode分散在磁盘上

- **使用inode映射 imap *inode map*** 这一数据结构

  相当于在inumber 和 inode之间引入间接层，以inumber作为输入，生成最新的inode磁盘地址

  通常被实现为一个数组，每个条目是一个磁盘指针

- imap需要写入磁盘--以便崩溃时仍能记录inode位置

imap应该在磁盘的那个位置？

- 他经常更新，需要更新文件结构，由于每次的更新与固定位置之间会有更多的磁盘寻道，所以 **固定位置不是一个好选择**

- **LFS将imap放在它写入所有其他新信息的位置旁边**
- 当将数据块追加到文件k时，LFS实际上将新数据块，其inode和一段inode映射一起写入磁盘

这时imap也散落在磁盘，我们应该如何查找他们？

- LFS在磁盘上有一个固定的位置 -- **检查点区 CR *checkpoint region***
- 它包含指向最新的inode映射片段的指针，因此可以通过首先读取CR来找到inode映射片段
- 检查点区定时更新，因此对性能影响不大

 **LFS的目录结构与传统的UNIX文件系统基本相同**

- 访问某名称的文件时，需要先查看imap
- 找到目录的inode位置
- 然后读取目录的inode得到目录数据的位置
- 然后读取该数据块得到名称到inode的映射
- 在查询inode映射同理得到文件

**LFS解决了递归更新（更新了文件inode也会向上递归对目录的更新）**，实际上任何原地更新的文件系统都有这样的问题

如何处理旧版本？

- 可以保留 -- 版本控制系统
- LFS只保留最新活版本 
- LFS清理程序定期读入许多旧的段，确定哪些块在这些段中存在，然后写出一组新的段，只包含其中活着的块，从而释放旧块用于写入
- 由于我们是按照顺序段写入的，所以清理时也会更加高效

LFS如何确定块的死活？

- 对于每个数据块D，LFS包括其inode号（它属于哪个文件）及其偏移量（这是该文件的哪一块）

- 该信息记录在一个数据结构中，位于**段头部**，称为段摘要块
- 对于一个块 ，通过其地址查看该段摘要找到inode与offset，然后通过inode得到此文件的第offset个块在磁盘上的位置
- 然后将这个位置与之前的地址进行比较来判断死活
- LFS也会对被截断和删除的文件增加版本号并在imap中记录，通过与段中记录的版本号对比来判断死活

要清理那些块？

- 优先清理那些没有被经常覆盖内容的段（冷段）
- 延迟清理热段

如果LFS写入磁盘时崩溃会发生什么？

- LFS在日志（log）中组织这些写入，即指向头部段和尾部段的检查点区域，并且每个段指向要写入的下一个段。LFS还定期更新检查点区域（CR）
- 写入CR时崩溃
  - LFS实际上保留了两个CR，每个位于磁盘的一端，并交替写入它们
  - 更新CR时，它首先写出一个头（带有时间戳），然后写出CR的主体，然后最后写出最后一部分（也带有时间戳）
  - 如果系统在CR更新期间崩溃，LFS可以通过查看一对不一致的时间戳来检测到这一点
  - LFS将始终选择使用具有一致时间戳的最新CR，从而实现CR的一致更新
- 写入段时崩溃
  - 由于LFS每隔30s左右写入一次CR，因此文件系统的最后一致快照可能很旧。因此，在重新启动时，LFS可以通过简单地读取检查点区域、它指向的imap片段以及后续文件和目录，从而轻松地恢复。但是，最后许多秒的更新将会丢失
  - LFS使用数据库社区的 **前滚 *roll forward*** 技术
  - 从最后一个检查点区域开始，找到日志的结尾（包含在CR中），然后使用它来读取下一个段，并查看其中是否有任何有效更新。如果有，LFS会相应地更新文件系统，从而恢复自上一个检查点以来写入的大部分数据和元数据

### Chapter 44 数据完整性和保护

磁盘故障模式

- 潜在扇区错误 LSE *Latent-Sector Errors* -- 因为一些原因导致磁盘上的数据不可读或者内容错误

  - 利用磁盘驱动器的磁盘内纠错码 ECC *Error Correcting Code* 来确定磁盘位良好与否
  - 如果可以则修复
  - 不可修复就报错
  - 不过我们可以利用冗余机制（如RAID）简单的恢复

- 块讹误 *block corruption*

  - 磁盘本身无法检测

  - 解决机制是使用数据 **校验和 *checksum***

    > 校验和是一个以一块数据作为输入的函数的运算结果

- 校验和 -- 获得的保护越多，成本越高

  常见的工作校验和函数

  - 基于XOR的校验和

    但是如果每个校验和单元内同位置的两个位发生变化，则校验和不会检测到讹误

  - 基于加法的校验和

    计算它只需要在每个数据块上执行二进制补码加法，忽略溢出

    它可以检测到数据中的许多变化，但如果数据被移位，则不好

  - Fletcher校验和

    设计两个校验字节s1与s2的计算

    假设块D由字节d1,…, dn组成。s1简单地定义如下：s1 = s1 + di mod 255（在所有di上计算）。s2依次为：s2 = s2 +      s1 mod 255（同样在所有di上）

    可以检测所有单比特错误，所有双比特错误和大部分突发错误

  - 循环冗余校验和 CRC

    将块中数据视为一个大的二进制数（毕竟它只是一串位）并将其除以约定的值。该除法的其余部分是CRC的值

  碰撞 -- 两个具有不相同内容的数据块可能具有相同的校验和

- 校验和在磁盘上的布局

  驱动器制造商采用520字节扇区格式化驱动器，每个扇区额外的8个字节可用于存储校验和

  没有此类功能的磁盘上，文件系统必须提供一个方案

  - 如n个校验和一起存储在一个扇区中，后跟n个数据块，接着是类似的结构

- 如何使用校验和

  - 读取某块数据时，客户端读取其校验和
  - 然后客户端计算该块数据的校验和
  - 比较 -- 相等则可能没被破坏，反之检测出讹误
  - 如果有冗余副本就使用它，没有就报错

- 错误位置的写入

  - 处理方式是：在每个校验和中添加更多信息--如块的磁盘和扇区号（偏移量）

- 丢失的写入

  设备通知上层写入已完成，但是从未持久化就可能丢失写入

  上面的所有校验和策略没有办法检测这种问题

  经典方法是：**执行写入验证**

  - 通过在写入后立即读回数据，系统可以确保数据确实到达磁盘表面

- 校验和什么时候得到实际的检查

  磁盘擦净 *disk scrubbing*

  通过定期读取系统的每个块，并检查校验和是否仍然有效，磁盘系统可以减少某个数据项的所有副本都被破坏的可能性

- 校验和的开销

  空间上很小

  但是时间上可能会比较明显 -- 已经有了一些有效的改善方法

### Chapter 47 分布式系统

通信基础

- 现代网络的核心原则是：通信基本是不可靠的

如何处理丢包？

- 方法一：不处理

  不可靠层：UDP/IP网络栈

  - 发送方永远不会被告知丢失
  - 但是UDP包含校验和来检测某种形式的数据包损坏
  - 由于许多应用程序只是想将数据发送到目的地，而不想考虑丢包，我们需要在不可靠的网络之上进行可靠的通信

- 方法二：可高的通信层

  - 使用 **确认 ack**技术让发送方知道接收方实际收到了消息

    - 想法是：发送方向接收方发送消息，接收方然后发回短消息确认收到

  - 如果在一定时间内没收到确认，则发送方断定该消息已丢失

    发送方然后就重试发送，再次发送相同的消息，这需要发送方必须保留一份消息副本，以防它需要再次发送

    这种机制叫 **超时/重试**

  - 如果丢失了确认信息，则可能发送方会发送重复的信息

    发送方必须独特地标识每个信息，接收方也要以某种方式追踪它是否已经看到每个信息 -- 保证可靠性

  - **顺序计数器**
    - 双方就起始值达成一致
    - 计数器的当前值都与消息一起发送。此计数器值（N）作为消息的ID。
    - 发送消息后，发送方递增该值（到N + 1）。
    - 接收方使用其计数器值，作为发送方传入消息的ID的预期值。
    - 如果接收的消息（N）的ID与接收方的计数器匹配（也是N），它将确认该消息
    - 若断定是第一次收到，则递增接收方计数器
    - 若确认信息丢失，则发送方超时，重新发送，但是接收器会确认该信息但不会传递给应用程序
  - 常用的可靠通信层为**TCP/IP**，简称为TCP，他的机制更为复杂

通信抽象

- 分布式共享内存 DMS *Distributed Shared Memory* -- 共享一个虚拟空间

  很难处理故障 ，性能也不好，没有广泛使用

- **远程过程调用 RPC *Remote Procedure Call***

  通常有两部分：存根生成器（协议编译器）和 运行时库

  - **存根生成器** -- 通过自动化，消除将函数参数和结果打包成消息的一些痛苦

  - 这种编译器的输入就是服务器希望导出到客户端的一组调用

    概念上如下：

    ```C
    interface {￼
      int func1(int arg1);￼
      int func2(int arg1, int arg2);￼
    }
    ```

    存根生成器接受这样的接口，为客户端生成客户端存根--包含接口中指定的每个函数，**客户端**将链接次存根然后调用他进行RPC

    在内部执行如下操作

    - 创建消息缓冲区
    - 参数封送处理（消息序列化）：将所需要信息打包到消息缓冲区 --包括调用函数的标识符以及函数的所有参数
    - 将消息发送到目标RPC服务器：细节由运行时库处理
    - 等待回复 -- 函数调用通常不是异步的，需要等待调用完成
    - 解封送处理（反序列化）：解包返回代码（也就是函数的返回结果）和其它参数

    - 返回调用者：从客户端存根返回到客户端代码

    对于**服务器**：

    - 解包消息
    - 调用实际函数
    - 打包结果
    - 发送回复

    构造时需要考虑如何发送复杂数据结构，以及如何组织并发式的服务器（可以用线程池）

  - 运行时库处理的挑战

    如何找到远程服务？ --  最简单的是 *当前互联网协议提供的主机名和端口号*

    构建RPC的传输级协议 -- 可靠的通信层会比较低效（需要客户端和服务端的额外确认）

    - 因此我们使用 **不可靠通信层**
    - 但需要RPC提供可靠性 -- 如使用 超时/重试 确认等机制

    还有一些其他问题

    比如：某些RPC是异步的

### Chapter 48 Sun的网络文件系统（NFS）

Sun开发了一种开放协议（open protocol），它只是指定了客户端和服务器用于通信的确切消息格式，而不是构建专有的封闭系统、

下面讨论经典的NFS协议 **NFSv2** -- 主要目标是 *简单快速的服务器崩溃恢复*

有状态的设计使得两端之间有共享状态，这让崩溃恢复变得复杂

通过设计无状态（stateless）协议，NFSv2实现了这个简单的目标

- 服务器不会追踪客户正在做什么
- 其设计要求在每个协议请求中提供所有需要的信息，以便完成该请求

- 文件句柄 *file handle*

  用于唯一的描述文件或者目录，因此，许多协议请求会包含一个文件句柄

  三个重要组件

  - 卷标识符:通知服务器，请求指向哪个文件系统
  - inode号:告诉服务器，请求访问该分区中的哪个文件
  - 世代号:通过在复用inode号时递增世代号，服务器确保具有旧文件句柄的客户端不会意外地访问新分配的文件

- 协议示例

  ```bash
  NFSPROC_GETATTR 
  	expects: file handle 
  	returns: attributes  
  NFSPROC_SETATTR 
  	expects: file handle, attributes 
  	returns: nothing  
  NFSPROC_LOOKUP 
  	expects: directory file handle, name of file/directory to look up 
  	returns: file handle  
  NFSPROC_READ 
  	expects: file handle, offset, count 
  	returns: data, attributes  
  NFSPROC_WRITE 
  	expects: file handle, offset, count, data 
  	returns: attributes  
  NFSPROC_CREATE 
  	expects: directory file handle, name of file, attributes 
  	returns: nothing  
  NFSPROC_REMOVE 
  	expects: directory file handle, name of file to be removed 
  	returns: nothing  
  NFSPROC_MKDIR 
  	expects: directory file handle, name of directory, attributes 
  	returns: file handle  
  NFSPROC_RMDIR 
  	expects: directory file handle, name of directory to be removed 
  	returns: nothing  
  NFSPROC_READDIR 
  	expects: directory handle, count of bytes to read, cookie 
  	returns: directory entries, cookie (to get more entries)
  ```

  假设客户端已经有一个文件系统根目录的目录文件句柄（/）

  如果客户端上运行的应用程序打开文件/foo.txt，则客户端文件系统会向服务器发送查找请求，并向其传递根文件句柄和名称foo.txt。如果成功，将返回foo.txt的文件句柄和属性
  属性就是文件系统追踪每个文件的元信息

  有了文件句柄，客户端可以对一个文件发出READ和WRITE协议消息，读取和写入该文件

  然后，服务器就能发出读取请求，并将数据返回给客户端

- **幂等操作处理服务器故障**

  客户端以唯一的方式处理所有这些故障：就是**重试请求**

  是因为大多数NFS请求有一个重要的特性：它们是幂等的 *idempotent*

  > 如果操作执行多次的效果与执行一次的效果相同，该操作就是幂等的

- 提升性能 -- 客户端缓存

  依靠网络发送多有读写请求太过低效

  我们利用客户端缓存提高效率

  - NFS客户端文件系统缓存文件数据和元数据
  - 尽管第一次比较昂贵，但是后续效率会很高
  - 缓存还可用作写入的临时缓冲区

  问题：缓存一致性

  - 问题一：客户端可能将它的写入缓存一段时间，然后再将它们发送给服务器，导致其他客户端读到的都是旧版本
  - 问题二：可能服务器被某个客户端写入而具有新版本，但是另一个客户端的缓存是旧的，可能会被程序读取

  解决：

  - 客户端有时会：关闭时刷新

    当应用程序写入文件并随后关闭文件时，客户端将所有更新（即缓存中的脏页面）刷新到服务器

  - 另外：利用GETATTR请求获取文件属性--文件修改信息

    如果文件修改的时间晚于文件提取到客户端缓存的时间，则客户端会让文件无效，因此将它从客户端缓存中删除，并确保后续的读取将转向服务器，取得该文件的最新版本

    如果客户端看到它持有该文件的最新版本，就会继续使用缓存的内容，从而提高性能

    注意⚠️：这时NFS服务器充斥着GETATTR请求，然而大部分时间没有人修改文件

    解决是：**为每个客户端添加了一个属性缓存**，客户端在访问文件之前仍会验证文件，但大多数情况下只会查看属性缓存以获取属性。首次访问某文件时，该文件的属性被放在缓存中，然后在一定时间（例如3s）后超时

- 还要考虑服务器端的缓存问题

  NFS服务器必须在通知客户端成功之前，将每次写入提交到稳定（持久）存储。这样做可以让客户端在写入期间检测服务器故障，从而重试，直到它最终成功 -- 但是很可能出现性能瓶颈

## Chapter 49 Andrew文件系统（AFS）

目标是扩展性

AFS与NFS的不同之处也在于，**从一开始，合理的、用户可见的行为就是首要考虑的问题**。在NFS中，缓存一致性很难描述，因为它直接依赖于低级实现细节，包括客户端缓存超时间隔。在AFS中，缓存一致性很简单且易于理解：**当文件打开时，客户端通常会从服务器接收最新的一致副本。**

所有AFS版本的基本原则之一，是在访问文件的**客户端计算机的本地磁盘上，进行全文件缓存**

NFS缓存块并且在内存中

#### AFSv1

首次访问：

- 当客户端应用程序首次调用open()时，AFS客户端代码（AFS设计者称之为Venus）将向服务器发送Fetch协议消息。

​	Fetch协议消息会将所需文件的整个路径名传递给文件服务器（它们称为Vice的组），然后将沿着路径名，查找所需的文件，并将整	个文件发送回客户端。

​	然后，客户端代码将文件缓存在客户端的本地磁盘上，**在本地进行系统调用**

​	AFS还使用客户端内存来缓存它在本地磁盘中的块副本。

​	最后，AFS客户端完成后检查文件是否已被修改（即它被打开并写入）。如果被修改，它会用**Store协议消息，将新版本刷写回服务器，将整个文件和路径名发送到服务器以进行持久存储**

再次访问：

​	客户端代码首先联系服务器（使用TestAuth协议消息），以确定文件是否已更改。

​	如果未更改，客户端将使用本地缓存的副本，从而避免了网络传输

限制扩展性的问题：

- 路径查找成本过高
- 客户端发出了太多TestAuth协议消息 （类似NFS的GETATTR）生成大量流量检查文件

#### AFSv2

AFSv2引入了回调 *callback* 的概念，以减少客户端/服务器交互的数量。

>  回调就是服务器对客户端的承诺，当客户端缓存的文件被修改时，服务器将通知客户端。

AFSv2还引入了文件标识符（File Identifier，FID）的概念（类似于NFS文件句柄），替代路径名，来指定客户端感兴趣的文件

- 包含卷标识符、文件标识符、全局唯一标识符
- 不是将整个路径名发送到服务器，并让服务器沿着路径名来查找所需的文件
- 而是客户端会沿着路径名查找，每次一个，缓存结果
- 从而有望减少服务器上的负载

- 举例：查找/home/remzi/notes.txt

  客户端将先获取home的目录内容，将它们放在本地磁盘缓存中，然后在home上设置回调

  然后，客户端将获取目录remzi，将其放入本地磁盘缓存，并在服务器上设置remzi的回调

  最后，客户端将获取notes.txt，将此常规文件缓存在本地磁盘中，设置回调

  最后将文件描述符返回给调用应用程序

- 与NFS的关键区别在于，每次获取目录或文件时，AFS客户端都会与服务器建立回调

  从而确保服务器通知客户端，其缓存状态发生变化

  好处就是：尽管首次还是有很多消息，但是后续访问完全是本地的

缓存一致性

- AFS对同一台机器上的不同进程进行了例外处理。在这种情况下，对文件的写入对于其他本地进程是立即可见的（进程不必等到文件关闭，就能查看其最新更新版本）

- 在极少数情况下，不同机器上的进程会同时修改文件

  AFS自然会采用最后的更新

崩溃恢复

​	问题是回调被保存在内存中。因此，当服务器重新启动时，它不知道哪个客户端机器具有哪些文件。因此，在服务器重新启动时，服务器的每个客户端必须意识到服务器已崩溃，并将其所有缓存内容视为可疑，并且（如上所述）在使用之前重新检查文件的有效性

性能与扩展性

- 首先，在许多情况下，每个系统的性能大致相当
- 其次，在大文件顺序重新读取时，AFS在这种情况下比NFS快一倍，此外，NFS会增加服务器负载，这也会对扩展产生影响
- 第三，（新文件的）顺序写入应该在两个系统上性能差不多
- 第四，AFS在顺序文件覆盖上表现较差 -- 因为客户端先完整地提取旧文件，只是为了后来覆盖它。相反，NFS只会覆盖块，从而避免了初始的读取
- 最后，访问大型文件中的一小部分数据的工作负载，在NFS上比AFS执行得更好

