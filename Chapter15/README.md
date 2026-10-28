# 第 15 章：AVL 平衡搜索树

本章在第 14 章普通 BST 的基础上，讲解如何用旋转限制树高；代码实现独立的 AVL 字典。

## 1. AVL 树：在 BST 顺序上限制高度

普通 BST 按 `1,2,3,...` 插入会成为一条链。AVL 仍按 BST 规则保存键，却要求**每个结点**的左右子树高度差不超过 1。`avlTree.hpp` 实现与普通 BST 相同的字典 ADT，另提供 `lowerBound`、`height`、中序遍历和 `clear`。它独立于索引 BST，本章 AVL 结点不保存排名信息。

### 结点、高度和平衡因子

每个结点保存 `(const 键, 值)`、`left`、`right` 和 `height`。本章约定空树高 0、叶子高 1；每次孩子链接改变后，都重新计算 `height(x)=1+max(height(x.left),height(x.right))`。定义 `BF(x)=height(x.left)-height(x.right)`，合法值为 `-1、0、1`。若变成 `2` 或 `-2`，就要旋转。`height()` 只读取根结点记录的高度，因此为 `O(1)`；测试辅助方法 `valid()` 会逐结点核对键序、记录高度和 `BF`，需 `O(n)`。

### 为什么旋转不破坏搜索顺序？

设 `y` 是 `z` 的左孩子，`T₂` 是 `y` 原来的右子树，键序为 `T₁ < y < T₂ < z < T₃`：

```text
右旋前：        z                 右旋后：       y
              / \                             / \
             y   T₃                          T₁  z
            / \                                 / \
           T₁  T₂                              T₂  T₃
```

右旋把 `y` 提上来，`T₂` 接到 `z` 左边，所以上述中序次序完全不变。左旋是镜像操作。代码先更新下降结点 `z` 的高度，再更新新根 `y`，因为后者的高度依赖前者。

| 插入路径 | 例子 | 修复 |
| --- | --- | --- |
| LL | `30,20,10` | 对失衡结点右旋 |
| RR | `10,20,30` | 对失衡结点左旋 |
| LR | `30,10,20` | 先对左孩子左旋，再对失衡结点右旋 |
| RL | `10,30,20` | 先对右孩子右旋，再对失衡结点左旋 |

判断时先看失衡结点的 `BF`，再看较高孩子的 `BF`：例如根左高 2、左孩子右倾（`BF<0`）就是 LR。双旋由两次保持键序的单旋组成。

### 插入与删除如何回溯？

插入仍先沿 BST 路径找到空位置；已有键只更新值。递归返回时，逐层更新高度并修复失衡。插入只会让祖先高度增加，修复最低的失衡祖先后，整体高度恢复，余下祖先仍会被检查。

删除先处理叶子、单孩子、双孩子三种 BST 情况。双孩子时摘取右子树最小的**后继结点**来接替目标，因此无需给 `const` 键赋值。摘取后继的路径和目标到根的路径都要回溯并可能旋转。删除可能让子树高度连续下降，所以**不能在修复第一个失衡祖先后停止**。删除时较高孩子的 `BF` 可能为 0，此时也使用相应的单旋；`testAVLTree.cpp` 覆盖了这两个对称情况。

AVL 的高度为何是 `O(log n)`？高度为 `h` 的 AVL 树若想使用最少结点，两个孩子的高度分别只能是 `h-1` 和 `h-2`，因此 `N(h)=1+N(h-1)+N(h-2)`。该数量随 `h` 指数增长，反过来高度只能随结点数对数增长。故查找、`lowerBound`、插入和删除最坏均为 `O(log n)`；插入和删除的递归调用栈为 `O(log n)`。

```cpp
avlTree<int, std::string> tree;
for (int key = 1; key <= 15; ++key)
    tree.insert({key, "value"});
// 普通 BST 此时高度为 15；AVL 会保持对数级高度。
tree.erase(8);
```

## 2. 文件与运行

| 文件 | 内容 |
| --- | --- |
| `avlTree.hpp` | AVL 字典、四类旋转和再平衡 |
| `chapter15Demo.cpp` | 升序插入与删除演示 |
| `testAVLTree.cpp` | 旋转、删除、随机对照和高度测试 |

在 `Chapter15` 目录运行：

```powershell
g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic -Werror chapter15Demo.cpp -o chapter15Demo.exe
.\chapter15Demo.exe
g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic -Werror testAVLTree.cpp -o testAVLTree.exe
.\testAVLTree.exe
```

测试通过时不输出内容；不要加 `-DNDEBUG`，否则 `assert` 不运行。
