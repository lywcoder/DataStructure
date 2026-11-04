#ifndef CHAPTER16_GRAPH_HPP
#define CHAPTER16_GRAPH_HPP

#include <cstddef>
#include <list>
#include <stdexcept>
#include <vector>

// 顶点编号为 [0, V)；无权边统一记作权值 1。
struct graphEdge {
    int to;
    int weight;
};

class graph {
public:
    graph(int vertices, bool directed, bool weighted)
        : vertices_(vertices), edges_(0), directed_(directed), weighted_(weighted) {
        if (vertices < 0) throw std::invalid_argument("negative vertex count");
    }
    virtual ~graph() {}
    int vertexCount() const { return vertices_; }
    std::size_t edgeCount() const { return edges_; }
    bool directed() const { return directed_; }
    bool weighted() const { return weighted_; }
    virtual void addEdge(int from, int to, int weight = 1) = 0;
    virtual std::vector<graphEdge> neighbors(int vertex) const = 0;

    virtual bool hasEdge(int from, int to) const {
        checkVertex(from);
        checkVertex(to);
        const std::vector<graphEdge> adjacent = neighbors(from);
        for (std::size_t i = 0; i < adjacent.size(); ++i)
            if (adjacent[i].to == to) return true;
        return false;
    }
    virtual int weight(int from, int to) const {
        checkVertex(from);
        checkVertex(to);
        const std::vector<graphEdge> adjacent = neighbors(from);
        for (std::size_t i = 0; i < adjacent.size(); ++i)
            if (adjacent[i].to == to) return adjacent[i].weight;
        throw std::out_of_range("edge does not exist");
    }

protected:
    int vertices_;
    std::size_t edges_; // 无向边及自环均只计一次
    bool directed_;
    bool weighted_;
    void checkVertex(int vertex) const {
        if (vertex < 0 || vertex >= vertices_) throw std::out_of_range("invalid vertex");
    }
    void checkEdge(int from, int to, int weight) const {
        checkVertex(from);
        checkVertex(to);
        if (!weighted_ && weight != 1) throw std::invalid_argument("unweighted edge must have weight 1");
    }
};

class adjacencyMatrixGraph : public graph {
public:
    adjacencyMatrixGraph(int vertices, bool directed, bool weighted)
        : graph(vertices, directed, weighted), present_(vertices, std::vector<bool>(vertices, false)),
          weights_(vertices, std::vector<int>(vertices, 0)) {}

    void addEdge(int from, int to, int weight = 1) override {
        checkEdge(from, to, weight);
        if (!present_[from][to]) ++edges_;
        present_[from][to] = true;
        weights_[from][to] = weight;
        if (!directed_) {
            present_[to][from] = true;
            weights_[to][from] = weight;
        }
    }
    bool hasEdge(int from, int to) const override {
        checkVertex(from);
        checkVertex(to);
        return present_[from][to];
    }
    int weight(int from, int to) const override {
        if (!hasEdge(from, to)) throw std::out_of_range("edge does not exist");
        return weights_[from][to];
    }
    std::vector<graphEdge> neighbors(int vertex) const override {
        checkVertex(vertex);
        std::vector<graphEdge> result;
        for (int to = 0; to < vertices_; ++to)
            if (present_[vertex][to]) result.push_back(graphEdge{to, weights_[vertex][to]});
        return result;
    }

private:
    std::vector<std::vector<bool> > present_;
    std::vector<std::vector<int> > weights_;
};

class adjacencyListGraph : public graph {
public:
    adjacencyListGraph(int vertices, bool directed, bool weighted)
        : graph(vertices, directed, weighted), lists_(vertices) {}

    void addEdge(int from, int to, int weight = 1) override {
        checkEdge(from, to, weight);
        if (setArc(from, to, weight)) ++edges_;
        if (!directed_ && from != to) setArc(to, from, weight);
    }
    std::vector<graphEdge> neighbors(int vertex) const override {
        checkVertex(vertex);
        return std::vector<graphEdge>(lists_[vertex].begin(), lists_[vertex].end());
    }

private:
    std::vector<std::list<graphEdge> > lists_;
    bool setArc(int from, int to, int weight) {
        for (std::list<graphEdge>::iterator it = lists_[from].begin(); it != lists_[from].end(); ++it) {
            if (it->to == to) { it->weight = weight; return false; }
        }
        lists_[from].push_back(graphEdge{to, weight});
        return true;
    }
};

// 链式前向星：head[v] 指向第一条弧，next 把同一起点的弧串起来。
class adjacencyArrayGraph : public graph {
public:
    adjacencyArrayGraph(int vertices, bool directed, bool weighted)
        : graph(vertices, directed, weighted), head_(vertices, -1), tail_(vertices, -1) {}

    void addEdge(int from, int to, int weight = 1) override {
        checkEdge(from, to, weight);
        if (setArc(from, to, weight)) ++edges_;
        if (!directed_ && from != to) setArc(to, from, weight);
    }
    std::vector<graphEdge> neighbors(int vertex) const override {
        checkVertex(vertex);
        std::vector<graphEdge> result;
        for (int i = head_[vertex]; i != -1; i = arcs_[i].next)
            result.push_back(graphEdge{arcs_[i].to, arcs_[i].weight});
        return result;
    }

private:
    struct arc { int to; int weight; int next; };
    std::vector<int> head_;
    std::vector<int> tail_;
    std::vector<arc> arcs_;
    bool setArc(int from, int to, int weight) {
        for (int i = head_[from]; i != -1; i = arcs_[i].next) {
            if (arcs_[i].to == to) { arcs_[i].weight = weight; return false; }
        }
        const int index = static_cast<int>(arcs_.size());
        arcs_.push_back(arc{to, weight, -1});
        if (head_[from] == -1) head_[from] = index;
        else arcs_[tail_[from]].next = index;
        tail_[from] = index;
        return true;
    }
};

#endif
