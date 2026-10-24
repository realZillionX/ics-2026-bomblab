# Lab2: BombLab

**DDL：2026 年 10 月 30 日 23:59**。详细实验文档见[课程主页](https://ics-26fall-fdu.github.io/)。

## 提交物说明

代码方面**只需要改 `defuser.c` 文件。** 其他文件（`defuser.h`、驱动程序、测试程序、`bomb`）都不要动。实验报告必交，但不单独计分。

实验总分共 120 分：前六关各 18 分，共 108 分；Honor（必做）10 分；提交格式 2 分。各关按隐藏测试通过比例计分，Honor 不是额外加分项。按时提交包含正确格式的实验报告且未修改 `defuser.c` 外工作流文件的仓库，可获得 2 分提交格式分。

| 关卡 | 主要知识点 |
| --- | --- |
| 第 1 关 | 函数调用、参数传递、返回值 |
| 第 2 关 | 数组、循环、递推关系 |
| 第 3 关 | switch 分支与跳转表 |
| 第 4 关 | 递归调用与返回过程 |
| 第 5 关 | 结构体布局、指针、链表遍历 |
| 第 6 关 | 函数指针表与间接调用 |
| Honor（必做） | （保密） |

## `defuser.c` 说明

**常规关卡函数：**

```c
int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    // 用 challenge 算出这一关的口令，写进 values 和 count
    return write_answer(out, cap, values, count);   // 还没做时 count = 0
}
```

- 输入只有一个：`challenge`，一个 64 位数，就是这一关的题目，每次以不同种子运行都会不同。
- 你的任务：完成根据 `challenge` 计算关卡口令（几个由 `challenge` 确定的整数）并写进 `values` 的流程，并把 `count` 赋值为此关口令整数的数量。
- `count` 是本关口令中的整数个数；未完成的求解函数返回非 0 时，公开测试会显示 `FAIL (solver_error)`。

**一些提示：**

- **答案不能写死。** `challenge` 每次都不同，评分用的是你没见过的 `challenge`，所以你要交出的是"怎么算"的公式，不是某一次的答案。
- **`seed` 不用你管。** 驱动已经用和炸弹一样的方法把 `seed` 换算成 `challenge` 传给你了。
- 调试信息用 `fprintf(stderr, ...)` 输出。用 `printf` 会把口令弄脏，直接判格式错误。
- 编译开了 `-Werror`，有警告就编不过。
- **别让"还没做"掐断后面的关。** 未完成的关先 `return 1` 是模板给的占位写法，本身不扣分；但**只要有一关返回非 0，驱动就当场停下，后面所有关的口令一行都不会输出**。
  - 想单独调某一关，用 `./defuser --seed 42 --phase 5`，前面没做的关不会挡路。
  - 不要用 `./defuser --seed 42 | ./bomb --seed 42` 去试后面的关，前面的 TODO 一挡就什么都看不到。

**Secret 关 (Honor，必做)：**

- `solve_secret(challenge, context, out, cap)`
- 输入多了 `context` 和 `cap`。 `context` 和 `challenge` 同时用于计算输出； `cap` 则为程序设定好的缓冲区大小。
- 你需要通过 `challenge` 和 `context` 计算出 `out` 输出的东西。输出格式则为不限于整数的一行文本。做完后，需把返回值改成 0；尚未实现时，Honor 公开测试显示 `FAIL (solver_error)` 并返回失败，不会影响其他六关各自的得分。

**本地自测：**

```sh
make
./defuser --seed 42 --phase 1 | ./bomb --seed 42 --phase 1   # 出现 Phase 1 defused. 就对了
python3 public_test.py
```

**已领取炸弹的同学：**

2026 年 10 月 12 日的修订只更正 `bomb --help`，并让 Honor 跳过空行及只含空白字符的行；题目参数、答案和评分规则不变。已领取的炸弹仍可继续使用，不要求重新领取或重做题目。

已有炸弹可以继续使用，不需要更新。重新领取时，请使用当前模板的 `fetch_bomb.py`；旧模板可从[最新正式 Release](https://github.com/ICS-26Fall-FDU/BombLab/releases/latest) 下载同名脚本，只替换这个文件，不要覆盖解法或报告。在 `config.txt` 中填写自己的学号，运行 `python3 fetch_bomb.py` 即可领取。

网络下载失败时，运行 `python3 fetch_bomb.py --print-url`，在浏览器中打开输出的链接下载，再用 `python3 fetch_bomb.py --file "下载的压缩包路径"` 导入。程序只替换 `bomb` 和 `package.json`，不会覆盖 `defuser.c`；如需更新 Actions 使用的炸弹，重新执行 `git add -f bomb` 并提交、push。已有 `bomb` 时，`make bomb` 不会主动更新。

## 实验报告要求

实验报告需要包含：

1. 每个已完成关卡的关键汇编、你恢复出的计算关系，以及推理过程。不用逐行翻译整份汇编，只需要讲清关键指令。
2. 每个已完成关卡至少一次用公开列表之外的 seed 验证的记录（公开 seed 是 `tests/public.json` 里那 10 个）。
3. 已完成关卡的运行结果截图。没做完的关如实写卡在哪、遇到什么问题。
4. 引用的资料，以及使用了哪些课程允许的辅助工具、用来做什么。工具生成的解法不能直接当成自己的分析提交。

格式与提交：

- 文件用 `report.md` 或 `report.pdf`，放仓库根目录，不要交 `.doc` / `.docx`。
- Markdown 报告里的图片放 `images/` 或 `assets/`，用相对路径引用，并且要一起提交，否则 GitHub 上打开是空白。
- 姓名学号可填可不填。
- 推送时别忘了 `git push origin final`：普通的 `git push` 不会推标签，助教按截止时刻收到的 `final` 评分。
