# OSTEP

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
