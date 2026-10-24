# GDB 教学目录

这里的代码只用于练习调试观察，不是正式题目的答案，也不使用每个学生独有的配置。先在这些最小例子上熟悉命令，再进入根目录的 `bomb`。

## 开始

```bash
make
make asm
gdb ./01_abi
```

在 GDB 中先执行：

```gdb
set pagination off
break main
run
disassemble /m main
display/i $pc
ni
info registers
```

需要进入被调用函数时用 `si`，需要执行完当前函数时用 `finish`。看到指针后用 `x/gx`，看到字符串用 `x/s`，看到整数数组用 `x/uw` 或 `x/6uw`。每个例子都保留了函数符号，可以直接 `break function_name`。

## 例子与正式关卡的对应

| 文件 | 观察内容 | 对应关卡 |
| --- | --- | --- |
| `01_abi.c` | 参数寄存器、返回值、helper 调用 | P1 |
| `02_loop_array.c` | 空循环、数组索引、比例寻址 | P2 |
| `03_jump_table.c` | switch、范围检查、跳转表 | P3 |
| `04_recursion.c` | 基例、递归调用、调用栈 | P4 |
| `05_linked_list.c` | 结构体偏移、next 指针、NULL | P5 |
| `06_indirect_call.c` | 函数指针表、间接 call | P6 |
| `07_stack_copy.c` | 局部对象布局、相邻字段覆盖 | 隐藏关 |

每个程序都可以先用 `objdump -d --no-show-raw-insn PROGRAM` 静态观察，再用 GDB 动态验证。教学例子中的地址和常量不对应任何正式 Bomb；正式题必须以自己的 binary 为准。

## 推荐顺序

```text
01 ABI
02 loop/array
03 jump table
04 recursion
05 linked list
06 indirect call
07 stack copy
```

`07_stack_copy` 只演示同一个 struct 内的局部字段覆盖，不演示返回地址覆盖、shellcode 或其他控制流劫持。
