# 第 11 章：二叉树与其它树的应用

## 1. 先认识“树”

树由结点和父子关系组成。除根以外，每个结点恰有一个父结点；从根到某个结点只有一条路径。没有孩子的结点叫**叶子**。以某个结点为根，连同它的所有后代，构成一棵**子树**。

二叉树中，每个结点最多有两个孩子，而且**左孩子和右孩子有区别**。例如只有左孩子的树与只有右孩子的树不是同一棵二叉树。二叉树可以为空；本章约定空树高度为 0，单结点树高度为 1，高度按结点层数计算。

贯穿本章的例子：

```text
        1
       / \
      2   3
     / \
    4   5
```

它有 5 个结点、高度为 3。结点 2 的左子树是单结点 4，右子树是单结点 5。

## 2. 二叉树的抽象数据类型（ADT）

`binaryTree.hpp` 定义共同接口。ADT 说的是使用者能做什么，存储方式由实现类决定。`linkedBinaryTree<T>` 和 `arrayBinaryTree<T>` 均可通过 `binaryTree<T>&` 使用。

| 操作 | 含义 |
| --- | --- |
| `empty()`、`size()` | 判断空树、读取结点数 |
| `height()` | 读取高度，空树为 0 |
| `preOrder()` | 根、左、右 |
| `inOrder()` | 左、根、右 |
| `postOrder()` | 左、右、根 |
| `levelOrder()` | 从上到下、每层从左到右 |
| `clear()` | 删除整棵树 |

遍历参数是 `std::function<void(T&)>`：回调每访问一个元素执行一次，因此既可以打印，也可以累加或修改元素。接口没有暴露结点位置，`setRoot`、`insertLeft` 等构造操作由具体实现提供。`erase()` 是为原有代码保留的 `clear()` 别名。

以上例为例，四种遍历得到：

| 遍历 | 顺序 | 记忆方法 |
| --- | --- | --- |
| 前序 | `1 2 4 5 3` | 先处理根，再处理子树 |
| 中序 | `4 2 5 1 3` | 根夹在左右子树之间 |
| 后序 | `4 5 2 3 1` | 子树处理完再处理根 |
| 层序 | `1 2 3 4 5` | 队列保存下一批待访问结点 |

三种深度优先遍历使用递归，层序遍历使用 FIFO 队列。打印表达式树的中序结果时通常还需括号，否则运算优先级可能产生歧义；遍历顺序本身不会自动补括号。

## 3. 链式表示：按关系连结结点

`binaryTreeNode.hpp` 的一个结点包含 `element`、`leftChild`、`rightChild`。空指针表示对应子树为空。`linkedBinaryTree.hpp` 保存根指针和结点数，创建结点时用 `new`，删除子树时先递归删除孩子，再删除根结点。

```cpp
linkedBinaryTree<int> tree;
binaryTreeNode<int>* root = tree.setRoot(1);
binaryTreeNode<int>* left = tree.insertLeft(root, 2);
tree.insertRight(root, 3);
tree.insertLeft(left, 4);
tree.insertRight(left, 5);
tree.preOrder([](int& value) { std::cout << value << ' '; });
```

`insertLeft/Right` 的父结点必须是这棵树中仍有效的结点；位置已被占用会抛 `std::logic_error`。`eraseLeft/Right` 删除整个子树并更新结点数。`rootNode()` 和插入操作返回的指针**不拥有**结点；删除相应子树或整棵树后不能再使用。为避免双重释放，该类禁止复制。

链式结构只为实际结点分配空间，适合形状不规则、经常接入或删除子树的二叉树。已知父结点后接入一个孩子是 `O(1)`；删除一棵含 `k` 个结点的子树是 `O(k)`。

## 4. 数组表示：用下标编码亲子关系

`arrayBinaryTree.hpp` 从下标 1 开始存根；下标 `i` 的左、右孩子分别是 `2i` 和 `2i+1`，父结点是 `⌊i/2⌋`。例子里的位置如下：

```text
下标   1  2  3  4  5
元素   1  2  3  4  5
```

代码用 `std::vector<std::unique_ptr<T>>` 保存槽位：空指针表示此位置无结点，元素类型 `T` 不必提供默认构造函数。

```cpp
arrayBinaryTree<int> tree;
tree.setRoot(1);
tree.insertLeft(1, 2);   // 返回下标 2
tree.insertRight(1, 3);  // 返回下标 3
tree.insertLeft(2, 4);   // 返回下标 4
```

调用 `at(index)` 可检查某槽位；空位返回 `nullptr`。插入时父位置必须存在，目标位置必须为空。数组适合接近完全二叉树的形状；若不断只插入右孩子，下标依次为 1、3、7、15……，即使结点很少也会出现大量空槽，所以偏斜树更适合链式表示。`slotCount()` 显示当前分配的最大槽位数，不等于结点数。

## 5. 操作成本与递归思路

设树有 `n` 个结点、高度为 `h`，删除的子树含 `k` 个结点。

| 操作 | 链式 | 数组 |
| --- | --- | --- |
| 读结点数、判空 | `O(1)` | `O(1)` |
| 已知父位置后插入孩子 | `O(1)` | 通常 `O(1)`，扩容时与新容量有关 |
| 四种遍历 | `O(n)` | `O(n)` |
| 求高度 | `O(n)` | `O(n)` |
| 删除指定子树 | `O(k)` | `O(k)` |
| 额外存储 | `O(n)` | `O(最大下标)` |

深度优先递归另外使用 `O(h)` 调用栈；层序遍历的队列最坏可存放 `O(n)` 个结点。高度的递推式为 `height(node) = 1 + max(height(left), height(right))`，空结点返回 0。删除子树采用后序思路：先删左、右子树，再删当前结点，避免孩子失去入口。

## 6. 应用：设置信号放大器

`signalAmplifiers.hpp` 中，根是信号源，信号从根向叶传播。每个非根结点的 `edgeLoss` 表示父结点到它的损耗。若从最近的信号源或放大器到当前结点的累计损耗超过阈值，信号就不合格。在结点安装放大器后，**它的所有出边**重新从累计损耗 0 计起。

```text
源 0 --2--> 1 --2--> 2       阈值 3
              +--1--> 3
```

若不安装，抵达 2 时累计损耗为 4。算法在结点 1 安装放大器，之后抵达 2 的新段损耗为 2，抵达 3 的新段损耗为 1；从源到结点 1 的上一段损耗仍是 2。`setSignalAmplifiers(root, 3)` 返回 `{1}`。

算法从根向下：检查当前结点到孩子的每条边；如果当前累计损耗加上某条边会超限，就在当前结点安装并将累计损耗清零，然后继续处理孩子。每条边本身若超过阈值，则无论如何布置结点放大器都无法满足约束，函数抛出 `std::invalid_argument`。输入损耗必须非负、根的损耗必须为 0。运行时间 `O(n)`，递归空间 `O(h)`。这是一个**可行布置**算法；返回的放大器数量不保证全局最少。

## 7. 应用：并查集是一片树

并查集维护互不相交的集合，核心问题是“两个元素是否在同一组”。`disjointSet.hpp` 用 `parent[i]` 保存结点 `i` 的父下标，每个集合都是一棵以代表元为根的树，根满足 `parent[root] == root`。这些是**普通树**，结点可能有多个孩子，并不要求是二叉树。

```cpp
disjointSet groups(5); // 初始为 {0} {1} {2} {3} {4}
groups.unite(0, 1);    // 合并 0、1 所在集合
groups.unite(2, 3);
bool same = groups.connected(0, 3); // false
groups.unite(1, 2);
same = groups.connected(0, 3);      // true
```

`find(x)` 沿父指针走到根；**路径压缩**在返回时让沿途结点直接指向根。`unite(a,b)` 先找两个根，再把较小集合的根挂到较大集合的根下（**按规模合并**）。重复合并返回 `false`，新合并返回 `true`。`count()` 是当前集合数，`componentSize(x)` 是 `x` 所在集合大小。下标范围是 `0` 到 `n-1`，越界抛 `std::out_of_range`。两种优化合用时，单次操作的均摊复杂度是 `O(α(n))`，`α` 是增长极慢的反 Ackermann 函数；总空间 `O(n)`。

## 8. 文件与运行

| 文件 | 内容 |
| --- | --- |
| `binaryTree.hpp`、`binaryTreeNode.hpp` | ADT 与链式结点 |
| `linkedBinaryTree.hpp`、`arrayBinaryTree.hpp` | 两种存储实现 |
| `traversal.hpp` | 可独立用于链式结点的四种遍历 |
| `signalAmplifiers.hpp`、`disjointSet.hpp` | 两个树的应用 |
| `chapter11Demo.cpp`、`testChapter11.cpp` | 演示与断言测试 |

在 `Chapter11` 目录运行：

```powershell
g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic -Werror chapter11Demo.cpp -o chapter11Demo.exe
.\chapter11Demo.exe
g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic -Werror testChapter11.cpp -o testChapter11.exe
.\testChapter11.exe
```

测试通过时没有输出；不要加 `-DNDEBUG`，否则 `assert` 不执行。

**课后思考：** ① 例子中删除结点 2 的整棵子树后，高度和结点数各是多少？② 为什么数组表示偏斜树时空槽位增长很快？③ 并查集的父指针为何不能套用 `2i`、`2i+1` 的二叉树下标公式？④ 如果某条边损耗本身大于阈值，为什么在它的终点安装放大器已经太晚？
