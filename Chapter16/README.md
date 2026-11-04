# 第 16 章：图、遍历与生成树

本章把图看作“顶点以及顶点之间的关系”。先认识图和三种存储方式，再学习 DFS、BFS；最后用遍历解决路径、连通性和生成树问题。代码均兼容 C++11，顶点用从 `0` 到 `V-1` 的整数编号。

## 1. 基本概念与特性

图记为 `G=(V,E)`：`V` 是顶点集合，`E` 是边集合。无向边 `{u,v}` 表示双向关系；有向弧 `(u,v)` 只表示从 `u` 到 `v`。无权图只关心边是否存在，有权图还保存权值，例如距离、成本或时间。本章权值为 `int`；无权边统一记作权值 `1`，但无权算法只看连接关系。

| 术语 | 含义 |
| --- | --- |
| 相邻、邻接 | 两顶点之间有边；有向图还需注意弧的方向 |
| 路径 | 沿边依次到达的顶点序列；长度通常指边数，带权路径也可计算权值和 |
| 环 | 从某顶点出发，沿边返回该顶点的非空路径 |
| 度 | 无向图中与顶点关联的边数；自环对度贡献 2 |
| 入度、出度 | 有向图中进入、离开顶点的弧数 |
| 连通 | 无向图的两个顶点之间存在路径；所有顶点互通则图连通 |
| 连通分量 | 无向图中极大的连通顶点集合 |
| 树 | 连通且无环的无向图；`V>0` 时恰有 `V-1` 条边 |

图比树更一般：可以有环、可以有多个分量，也没有天然的根或父子关系。因此遍历必须记录 `visited`，否则遇到环会重复访问。无向图满足所有顶点度数之和为 `2E`；有向图满足总入度和总出度均为 `E`。简单无向图至多有 `V(V-1)/2` 条非自环边；稀疏图的 `E` 远小于 `V²`，稠密图的 `E` 接近 `V²`。

本章允许自环，不保存平行边：重复插入同一方向的边会更新权值，边数不变。无向边只计一次，但为遍历会在两个顶点的邻接结构中各存一条弧；自环只存一次。有向图中的 `(u,v)` 与 `(v,u)` 是两条不同的边。不存在的边调用 `weight`、无效顶点编号、无权图插入非 `1` 权值，都会抛出异常。

## 2. 抽象数据类型 `graph`

[`graph.hpp`](./graph.hpp) 中的抽象类统一了三种表示。核心接口如下，`neighbors(v)` 返回从 `v` 出发的弧；它返回一个新的 `vector`，调用者可以安全遍历而不依赖内部存储。

```cpp
struct graphEdge { int to; int weight; };

class graph {
public:
    virtual ~graph() {}
    int vertexCount() const;
    std::size_t edgeCount() const;
    bool directed() const;
    bool weighted() const;
    virtual void addEdge(int from, int to, int weight = 1) = 0;
    virtual std::vector<graphEdge> neighbors(int vertex) const = 0;
    virtual bool hasEdge(int from, int to) const;
    virtual int weight(int from, int to) const;
};
```

`graph` 保存公共元信息和参数检查。三个具体类都继承它：`adjacencyMatrixGraph`、`adjacencyListGraph`、`adjacencyArrayGraph`。构造函数统一接收 `(顶点数, 是否有向, 是否有权)`。例如：

```cpp
adjacencyListGraph roads(4, false, true); // 4 顶点、无向、有权
roads.addEdge(0, 1, 7);
roads.addEdge(1, 2, 3);
for (const graphEdge& edge : roads.neighbors(1)) {
    // edge.to 是终点，edge.weight 是权值
}
```

### 邻接矩阵

`adjacencyMatrixGraph` 使用 `present[u][v]` 表示边是否存在，用 `weights[u][v]` 保存权值。独立的存在位使权值 `0` 也合法。若是无向图，插入 `{u,v}` 时同时设置 `[u][v]`、`[v][u]`，矩阵关于主对角线对称。

```text
无向边 {0,1}, {0,2}, {1,2}：
       0  1  2  3
    0  0  1  1  0
    1  1  0  1  0
    2  1  1  0  0
    3  0  0  0  0
```

访问 `neighbors(0)` 要扫描第 0 行，即使它只有两个邻居。矩阵适合稠密图，或者频繁判断特定边是否存在的场景。代码中矩阵类重写 `hasEdge` 和 `weight`，可直接用下标访问。

### 邻接链表

`adjacencyListGraph` 使用 `vector<list<graphEdge>>`，每个顶点拥有一条邻接链表。上述图可写成：

```text
0 -> (1) -> (2)
1 -> (0) -> (2)
2 -> (0) -> (1)
3 -> 空
```

有权时结点还保存 `weight`。有向图只把 `(u,v)` 放进 `u` 的链表。它无需为不存在的边预留单元，适合稀疏图；查询某条边须扫描该顶点的邻接表。重复插入时顺着链表找到原弧并更新。

### 邻接数组（链式前向星）

`adjacencyArrayGraph` 把邻接链表的结点集中放在连续数组 `arcs` 中。`head[v]` 是顶点 `v` 的第一条弧下标，`arcs[i]` 保存 `to、weight、next`，`next=-1` 表示链尾；`tail[v]` 帮助把新弧接到尾部。上述图的 0 号顶点可能表示为：

```text
head[0] = 0
arcs[0] = {to:1, weight:1, next:1}
arcs[1] = {to:2, weight:1, next:-1}
```

遍历时执行 `for (int i=head[v]; i!=-1; i=arcs[i].next)`。这种结构仍能按顶点快速找到所有出边，弧集中存放；与链表相比，它用整数下标代替每个结点的独立分配。这里采用可增量插入的链式前向星，并非构建后固定的压缩稀疏行（CSR）。

| 操作／空间 | 邻接矩阵 | 邻接链表 | 邻接数组 |
| --- | --- | --- | --- |
| 存储空间 | `O(V²)` | `O(V+E)` | `O(V+E)` |
| 判断或读取一条边 | `O(1)` | `O(deg(u))` | `O(deg(u))` |
| 插入或更新边 | `O(1)` | `O(deg(u))` | `O(deg(u))` |
| 枚举 `u` 的邻居 | `O(V)` | `O(deg(u))` | `O(deg(u))` |

其中 `deg(u)` 对有向图指 `u` 的出度；无向边占两个弧位置，仍为 `O(V+E)`。链表和邻接数组的插入复杂度包含检查重复边的扫描。`neighbors` 会复制结果，占额外 `O(deg(u))` 临时空间。

## 3. 深度优先搜索 DFS

**思路：** 从起点沿一条尚未走过的边不断深入；当前顶点没有未访问邻居时，回退到上一个顶点，继续试其他边。递归调用栈天然保存“回退到哪里”。必须在递归进入顶点时立即标记已访问，这样环和交叉边不会造成重复访问。

```text
DFS(v):
    visited[v] = true；记录 v
    对 v 的每个邻居 w：
        若 w 未访问：DFS(w)
```

[`depthFirstVisit`](./graphAlgorithms.hpp) 实现这一递归，公开函数 [`depthFirstSearch`](./graphAlgorithms.hpp) 初始化 `visited` 和访问顺序。以演示图的插入顺序为例：`0-1, 0-2, 1-3, 2-3, 3-4`，从 `0` 出发，先走 `0→1→3→2`；`2` 没有新邻居，回退到 `3`，再走 `4`。结果是 `0,1,3,2,4`；孤立顶点 `5` 不在本次搜索范围内。不同邻居顺序会改变遍历顺序，但不会改变可达集合。

```cpp
void depthFirstVisit(const graph& g, int v,
                     std::vector<bool>& visited, std::vector<int>& order) {
    visited[v] = true;
    order.push_back(v);
    for (const graphEdge& edge : g.neighbors(v))
        if (!visited[edge.to]) depthFirstVisit(g, edge.to, visited, order);
}

std::vector<int> depthFirstSearch(const graph& g, int start) {
    g.neighbors(start); // 检查顶点编号
    std::vector<bool> visited(g.vertexCount(), false);
    std::vector<int> order;
    depthFirstVisit(g, start, visited, order);
    return order;
}
```

**复杂度：** 邻接链表或邻接数组下，每个顶点最多进入一次，每条弧最多检查一次，时间 `O(V_r+E_r)`；矩阵需对每个到达顶点扫描整行，最坏 `O(V²)`。`V_r、E_r` 表示从起点可达部分。`visited`、输出及递归栈最多占 `O(V)`；很深的图可能耗尽程序调用栈。若需遍历整个非连通图，就对每个尚未访问的顶点再启动一次 DFS。

## 4. 广度优先搜索 BFS

**思路：** 先访问离起点 1 条边的顶点，再访问 2 条边的顶点，逐层向外扩展。队列保证“先发现的顶点先展开”。一个顶点应在**入队时**标记，否则多条边可能把它反复加入队列。

```text
visited[start] = true；start 入队
队列非空时：
    v = 队首并出队；记录 v
    对 v 的每个邻居 w：
        若 w 未访问：标记 w；w 入队
```

[`breadthFirstSearch`](./graphAlgorithms.hpp) 实现这一过程。对上图，从 `0` 出发，先入队 `1,2`，再由 `1` 发现 `3`，再由 `3` 发现 `4`，结果为 `0,1,2,3,4`。BFS 首次发现某顶点时走过的边数最少；它不保证**权值和**最小。

```cpp
std::vector<int> breadthFirstSearch(const graph& g, int start) {
    g.neighbors(start); // 检查顶点编号
    std::vector<bool> visited(g.vertexCount(), false);
    std::vector<int> order;
    std::queue<int> pending;
    visited[start] = true;
    pending.push(start);
    while (!pending.empty()) {
        int v = pending.front(); pending.pop();
        order.push_back(v);
        for (const graphEdge& edge : g.neighbors(v)) {
            if (!visited[edge.to]) {
                visited[edge.to] = true;
                pending.push(edge.to);
            }
        }
    }
    return order;
}
```

**复杂度：** 邻接链表或邻接数组为 `O(V_r+E_r)`，矩阵最坏 `O(V²)`；队列、`visited`、输出合计 `O(V)` 空间。BFS 不使用递归栈。

## 5. 应用一：寻找路径

[`findPath`](./graphAlgorithms.hpp) 在 BFS 首次发现 `w` 时记录 `parent[w]=v`。找到目标后，从目标沿 `parent` 回溯到起点，再反转序列。例如上图 `0→4` 得到 `0,1,3,4`，共 3 条边；`0→5` 不可达，返回空数组；起点等于终点，返回只含该顶点的数组。

因为 BFS 按边数分层，首次发现目标时得到**边数最少**的路径。带权图上它仍只优化边数；若需权值和最小，应另用适合权值条件的最短路算法，例如非负权的 Dijkstra。时间与 BFS 相同，回溯至多 `O(V)`；额外空间 `O(V)`。

## 6. 应用二：连通分量

无向图中，从一个未访问顶点启动 BFS，能到达的顶点恰好组成一个连通分量。重复选取未访问顶点，直到全部处理完，就是 [`connectedComponents`](./graphAlgorithms.hpp)。演示图得到 `{0,1,2,3,4}` 和 `{5}` 两个分量；[`isConnected`](./graphAlgorithms.hpp) 检查分量数是否不超过 1。本章约定空图也视为连通。

连通分量和 `isConnected` 仅接受无向图，有向图会抛出 `invalid_argument`；有向图需要另外区分弱连通和强连通。邻接链表／数组的总时间为 `O(V+E)`，矩阵为 `O(V²)`，额外空间 `O(V)`。

## 7. 应用三：生成树和最小生成树

连通无向图的一棵**生成树**包含全部顶点，而且没有环。它恰有 `V-1` 条边：每次从已发现顶点走向一个**尚未发现**顶点，就加入一条树边；已发现顶点之间的边不能加入，否则可能成环。[`spanningForest`](./graphAlgorithms.hpp) 用 BFS 的首次发现边构造生成树；图不连通时，为每个分量生成一棵树，合称**生成森林**。若图有 `C` 个分量，森林共有 `V-C` 条边。自环不会进入森林。它适合找网络的无环骨架、遍历关系层级等，但不保证总权值最小。复杂度与全图 BFS 相同。

若边代表铺线成本，通常还想要**最小生成树**：在连通的有权无向图中，寻找总权值最小的生成树。[`minimumSpanningForest`](./graphAlgorithms.hpp) 用 Kruskal 算法；不连通时返回每个分量的最小生成树，并报告 `componentCount`。

```text
取出每条无向边一次，按权值从小到大排序
每个顶点先自成一个集合
对每条边 (u,v,w)：
    若 u、v 属于不同集合：选这条边；合并两个集合
    否则：跳过，因为它会形成环
```

“当前最小且不成环的边”可以安全加入：若某棵最优树没有它，加入后会出现环；环上必有跨越相同两个部分的另一条边，替换掉那条不更便宜的边，仍得到一棵不更贵的树。代码用并查集的路径压缩、按秩合并判断是否成环。权值可以是 0 或负数；自环跳过。枚举邻接结构并排序的时间为链表／数组 `O(V+E log E)`，矩阵为 `O(V²+E log E)`；额外空间 `O(V+E)`。总权值用 `long long` 累加。它只接受**有权无向图**。

## 8. 文件与运行

| 文件 | 内容 |
| --- | --- |
| [`graph.hpp`](./graph.hpp) | `graph` ADT 和三种表示类 |
| [`graphAlgorithms.hpp`](./graphAlgorithms.hpp) | DFS、BFS、路径、连通分量、生成森林和 Kruskal |
| [`chapter16Demo.cpp`](./chapter16Demo.cpp) | 无权遍历和有权最小生成树示例 |
| [`testChapter16.cpp`](./testChapter16.cpp) | 三种表示及算法的断言测试 |

从仓库根目录执行：

```powershell
g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic -Werror Chapter16/testChapter16.cpp -o Chapter16/testChapter16.exe
.\Chapter16\testChapter16.exe
g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic -Werror Chapter16/chapter16Demo.cpp -o Chapter16/chapter16Demo.exe
.\Chapter16\chapter16Demo.exe
```

测试通过时没有输出。不要用 `-DNDEBUG`，否则 `assert` 会被禁用。演示程序应显示 DFS `0 1 3 2 4`、BFS `0 1 2 3 4`、两个连通分量，以及权值为 `8` 的最小生成树。
