# STL 的 next_permutation：原理与教学实现

## 1. 它做什么？

`std::next_permutation` 定义在 `<algorithm>` 中，对半开区间 `[first, last)` **原地**产生按字典序紧接着的下一个排列。

```cpp
bool found = std::next_permutation(first, last);
bool found = std::next_permutation(first, last, comp);
```

- 若存在更大的排列：修改区间，返回 `true`。
- 若当前已是最大排列：把区间恢复为最小排列，返回 `false`。不是保持原排列不变。
- 空区间、只有一个元素、所有元素都等价：没有下一个排列，返回 `false`。
- 默认按 `<` 比较；自定义 `comp` 可以改变元素的先后关系。

本目录的 `nextPermutation.hpp` 提供 `chapter1::next_permutation`，展示标准库常见的实现思路。它不是某一版本 STL 源代码的逐字复制；不同标准库可以优化实现，但必须遵守相同的算法语义。已有的 `perm.hpp` 是调用标准库算法的示例，`permutation.hpp` 是递归交换法，二者没有被修改。

## 2. “字典序”是什么意思？

比较两个序列时，从左往右找第一个不同的位置，用该位置上的元素比较大小。例如：

```text
1 2 3 < 1 3 2 < 2 1 3 < 2 3 1 < 3 1 2 < 3 2 1
```

不能把序列简单理解成整数：元素也可以是字符串、对象等；只需要有一致的比较规则。

我们的目标不是找到“任意一个更大排列”，而是找到 **所有更大排列中最小的那个**。

## 3. 四步算法

以序列 `1 3 5 4 2` 为例。

### 第一步：从右向左找最右侧的上升相邻对

找到最大的下标 `i`，满足 `a[i] < a[i + 1]`：

```text
1 3 | 5 4 2
  ^   -----
枢轴   非递增后缀
```

这里枢轴（pivot）是 `3`，后缀是 `5 4 2`。因为我们从右向左扫描，后缀已经按非递增顺序排列；后缀自身已是这些元素的最大排列，无法仅改动后缀得到更大的整个序列。

要让排列增大，必须改动枢轴或更靠左的位置。选择最靠右的枢轴，才能保留最长的左侧前缀，让增量尽可能小。

如果根本没有上升相邻对，整个区间已非递增，也就是最大排列。将它反转为最小排列并返回 `false`。

### 第二步：从右向左找第一个比枢轴大的元素

后缀非递增，所以从右边遇到的第一个大于 `3` 的元素，就是后缀里最小的可用更大值：这里是 `4`，不是 `5`。

```text
1 [3] 5 [4] 2
```

因为第一步保证存在 `a[i + 1] > a[i]`，这一步一定能找到后继，不会越过枢轴。

### 第三步：交换枢轴和这个后继

```text
1 [4] 5 [3] 2
```

现在整个排列已经变大，但还不是下一个：后缀 `5 3 2` 还可以更小。

### 第四步：反转后缀，使它最小

```text
1 4 | 2 3 5
```

交换后的后缀仍是非递增顺序，因此反转后就是非递减顺序，不需要执行 `sort`。

最终：

```text
1 3 5 4 2 -> 1 4 2 3 5
```

### 为什么交换后仍能直接反转？

后继是从右往左找到的第一个大于原枢轴的元素。因此它右边的元素都不大于原枢轴；它左边的元素不小于后继，自然也大于原枢轴。把原枢轴放到后继位置后，后缀仍非递增。

这也是必须选择“从右边遇到的第一个更大值”的原因，尤其在重复元素存在时，不能随意选择后继。

## 4. 核心伪代码

以下下标版本仅用于说明；空区间和单元素应先单独处理：

```text
i = n - 2
while i >= 0 and not (a[i] < a[i + 1]):
    i--

if i < 0:
    reverse(a[0:n])
    return false

j = n - 1
while not (a[i] < a[j]):
    j--

swap(a[i], a[j])
reverse(a[i + 1:n])
return true
```

实际 C++ 实现在 `nextPermutation.hpp` 中使用迭代器而非数组下标：

- `pivot`：当前检查的枢轴。
- `suffix`：后缀起点，即枢轴后一位。
- `successor`：右侧最小的可用更大元素。
- `std::iter_swap`：交换两个迭代器指向的元素。
- `std::reverse`：把后缀反转为最小排列。
- `comp(a, b)`：表示在指定顺序中 `a` 排在 `b` 之前。

## 5. 怎样枚举全部排列？

必须先从最小排列出发，再用 `do ... while`：

```cpp
#include "nextPermutation.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

std::vector<int> values = {3, 1, 2};
std::sort(values.begin(), values.end());
do {
    for (int value : values)
        std::cout << value << ' ';
    std::cout << '\n';
} while (chapter1::next_permutation(values.begin(), values.end()));
```

- 如果不先排序，只会枚举当前排列到最大排列的这一段；最终恢复最小排列时已经返回 `false`，循环不会继续。
- 用 `do ... while` 是因为初始排列也应输出。先调用算法再输出会跳过初始排列。
- 算法只改动传入的区间，区间外的元素不变。

## 6. 重复元素与自定义比较

### 重复元素

```text
1 1 2 -> 1 2 1 -> 2 1 1 -> 返回 false，恢复 1 1 2
```

按照数值比较，从最小排列出发不会重复输出相同的数值序列。若 `n` 个元素中各值分别重复 `c1, c2, ...` 次，不同排列数为：

```text
n! / (c1! × c2! × ...)
```

如果比较器把两个不同对象判为等价，算法也不会因为对象身份不同而专门枚举等价对象的内部交换。

### 按相反顺序枚举

```cpp
std::sort(values.begin(), values.end(), std::greater<int>());
do {
    // 输出 values
} while (chapter1::next_permutation(
    values.begin(), values.end(), std::greater<int>()));
```

此时 `3 2 1` 是比较规则中的最小排列，后续是 `3 1 2`、`2 3 1` 等；这里的“大”“小”都是相对于比较器，而不是固定的数值升降。

比较器必须满足严格弱序，不能用 `<=` 代替 `<`。排序和生成排列必须使用同一种比较规则。

## 7. 复杂度和迭代器要求

单次调用对长度为 `n` 的区间：

- 最坏时间复杂度 `O(n)`：寻找枢轴、寻找后继、反转后缀都至多线性。
- 额外空间 `O(1)`：只保存少量迭代器，没有额外数组或递归栈。
- 本教学实现支持 C++11，需要可写的**双向迭代器**和可交换的元素。数组指针、`std::vector`、`std::string`、`std::list` 都可以；`std::forward_list` 只有前向迭代器，不能用于该算法。

这些复杂度把比较和交换视为常数时间。枚举所有互异元素的排列仍有 `n!` 种结果；若打印每个长度为 `n` 的排列，仅输出成本就达到 `O(n × n!)`。单次线性不代表全部枚举也是线性。

## 8. 和递归交换法有什么区别？

`permutation.hpp` 中的递归方法通过交换并回溯遍历排列树：通常需要 `O(n)` 递归栈，不自动保证字典序；有重复值时，若不额外去重，可能输出重复的数值排列。

`next_permutation` 则只生成当前排列的下一个，无需递归，天然按照指定比较规则的字典序推进。它并不是每次先生成所有排列再挑选下一个。

## 9. 文件和运行方式

| 文件 | 内容 |
| --- | --- |
| `nextPermutation.hpp` | 双向迭代器版教学实现，含自定义比较器重载 |
| `nextPermutationDemo.cpp` | 全排列、单步变化、重复元素和反向比较演示 |
| `testNextPermutation.cpp` | 与 `std::next_permutation` 对照的断言测试 |
| `nextPermutation.md` | 本讲义 |

在 `Chapter1` 目录分别编译，不要把两个含 `main` 的源文件一起链接：

```sh
g++ -std=c++11 -Wall -Wextra -Wpedantic nextPermutationDemo.cpp -o nextPermutationDemo.exe
./nextPermutationDemo.exe

g++ -std=c++11 -Wall -Wextra -Wpedantic testNextPermutation.cpp -o testNextPermutation.exe
./testNextPermutation.exe
```

PowerShell 运行时使用 `.\nextPermutationDemo.exe` 或 `.\testNextPermutation.exe`。测试使用 `assert`，不要定义 `NDEBUG`。
