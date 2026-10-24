# tutor 教学：从寄存器到通用 solver

tutor 教学不计分，全部教材和练习都在本目录中。这里的 `bomb` 和 `defuser` 是独立的教学程序，不需要填写学号、下载个人炸弹或完成正式关卡。正式关卡仍在仓库根目录完成，不提供源码和参考解法。tutor 教学在程序里的编号是 0，所以命令写作 `--phase 0`，入口函数是 `phase_00`，通过时打印 `Phase 0 defused.`。

从学生包根目录开始：

```bash
cd tutor
make
make test
```

`make` 会编译教学炸弹和已有的 tutor 参考求解程序，生成反汇编、占位输入与示例答案。根目录的 `make tutor` 和 `make test-tutor` 也是兼容入口。以下命令均在 `tutor/` 中执行。

## 1. 固定一次运行

在本目录执行：

```bash
./bomb --seed 42 --phase 0
```

记录屏幕上的 challenge。炸弹只提示输入无符号十进制整数，不提示个数；本练习的口令是两个整数，先输入 `0 0`，本次失败只用于认识输入格式。下一节会说明如何从汇编中看出个数，正式关卡需要用同样的方法自己确定。

## 两种调试方式

tutor 教学用两种方式分析同一关，正式关卡也建议配合使用：

| 方式 | 做法 | 适合回答的问题 |
| --- | --- | --- |
| 导出汇编 | 用 `objdump` 或 `gdb -batch` 把汇编写进文件，在编辑器中阅读、搜索、标注 | 调用了哪些函数，分支、循环和比较在哪里 |
| 在线调试 | 在 GDB 中运行程序，设断点、单步执行，查看寄存器和内存 | 某一时刻寄存器和内存里的具体值是多少 |

一般先导出汇编看清结构，再在线调试确认具体的值。第 2 节是导出方式，第 3 至 6 节是在线方式。

## 2. 方式一：导出汇编

不运行程序，只把汇编保存到文件，两条命令任选其一：

```bash
objdump -d --no-show-raw-insn --disassemble=phase_00 ./bomb > phase_00.S
gdb -batch -ex 'disassemble phase_00' -ex 'disassemble tutorial_mix' ./bomb > phase_00.gdb.txt
```

`objdump` 按地址列出指令；`gdb -batch` 不进入交互界面，依次执行 `-ex` 中的命令后退出，可以一次导出多个函数。`make` 生成的 `phase0.disasm.txt` 也是用 `objdump` 导出的。

在导出的文件中依次找到并标注：

- 每条 `call` 调用的函数：`valid_u32_fields`、`sscanf` 和 `tutorial_mix`；
- 调用 `valid_u32_fields` 之前放进 `esi` 的 2，以及 `sscanf` 之后与 2 的比较，说明口令由两个整数组成；
- 对 challenge 的移位、截取和异或，以及随后与第一个整数的比较；
- `tutorial_mix` 的返回值与第二个整数的比较；
- 设置返回值（`al` 或 `eax`）的位置，也就是关卡判定成功或失败的地方。

导出的文件可以反复阅读，也可以直接写注释。但文件里只有指令，看不到运行时的值：栈上的整数是多少、某个寄存器此刻存着什么，要用方式二确认。

## 3. 方式二：在线调试，在第一条指令停下

```bash
gdb --args ./bomb --seed 42 --phase 0
```

```gdb
set pagination off
break *phase_00
run --seed 42 --phase 0 < phase0-input.txt
x/i $pc
x/s $rdi
p/x $rsi
disassemble /r phase_00
```

`phase0-input.txt` 包含 `0 0`。重定向命令中显式保留参数：`run` 后出现新参数或重定向时，不应假定此前的 `--args` 仍被沿用。也可以首次直接用 `run` 后手工输入。入口原型是：

```c
bool phase_00(const char *input, uint64_t challenge);
```

因此此刻 `rdi` 指向输入，`rsi` 保存 challenge。`break *phase_00` 指定机器指令入口；`break phase_00` 在有调试信息的程序中可能跳过序言。观察参数时不要混淆这两种断点。

## 4. 追踪读取到栈上的整数

在 `disassemble` 输出中找到 `sscanf` 的调用。此前的 `valid_u32_fields` 只检查十进制格式、数量和范围，使用 `ni` 跳过即可。

```gdb
display/i $pc
ni
info registers rdi rsi rdx rcx rbp rsp
```

重复 `ni`，在调用 `sscanf` 的指令执行前停下。这是一个新的函数调用，寄存器含义已经变化：`rdi` 是字符串，`rsi` 是格式串，`rdx` 和 `rcx` 分别是两个输出整数的地址。

```gdb
x/s $rdi
x/s $rsi
set $first_slot = $rdx
set $second_slot = $rcx
ni
p/d $eax
x/1uw $first_slot
x/1uw $second_slot
```

`eax` 应为 2，两个内存位置中都是占位值 0。`x/1uw` 表示把一个 4 字节单元按无符号十进制读取。保存的是 GDB 的便利变量，不要求被调试程序保留任何变量名。

也可以在反汇编中找到 `sscanf` 后第一条指令，将其实际地址用于 `break *地址`。地址必须从本人包中读取，不要照搬其他发行包的偏移。

## 5. 恢复第一个数

继续逐条执行，观察 challenge 如何被保存、右移、截取和异或。寄存器分配由编译器决定，不预设 challenge 一定在 `rbx`。

本练习可恢复为：

```c
uint32_t first = (uint32_t)((challenge >> 5) & 255) ^ 42;
```

把这条规律与第 2 节导出的汇编对照。比较指令最终检查输入的第一个整数是否等于这个值。占位输入会在这里失败，不需要强行跳过判断。

## 6. 观察 helper 的参数和返回值

重新运行，这次读取 tutor 教学提供的正确示例：

```gdb
delete breakpoints
break *tutorial_mix
run --seed 42 --phase 0 < phase0-answer.txt
p/u $edi
disassemble tutorial_mix
si
```

函数参数是一个 `uint32_t`，观察 `edi` 即可。它计算 `3 * first + 7`，编译器可能用 `lea` 表达乘法和加法。用 `si` 观察指令，或执行 `finish` 返回调用者，再用 `p/u $eax` 检查返回值。

```c
uint32_t second = 3 * first + 7;
```

两项条件都成立时，继续运行会打印 `Phase 0 defused.`。

## 7. 将规律写成可执行程序

示例实现在 `phase0_solution.c`，使用 `snprintf` 写入调用者的缓冲区，并检查是否截断。

```bash
make
./defuser --seed 42 --phase 0
./defuser --seed 42 --phase 0 | ./bomb --seed 42 --phase 0
make test
./defuser --seed 987654321 --phase 0 | ./bomb --seed 987654321 --phase 0
```

换 seed 后仍能通过，说明程序实现的不是某一组示例口令。完成 tutor 教学后执行 `cd ..` 回到仓库根目录；还没有领取个人炸弹时，填写学号并运行 `make bomb`。接下来对正式关卡重复同样的过程：先导出 `phase_01` 至 `phase_06` 的汇编看清结构，再用 GDB 在线确认具体的值，例如在 `phase_02` 中额外恢复六元素数组的循环和递推关系。
