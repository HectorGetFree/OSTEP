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

## Chapter 36 I/O设备

