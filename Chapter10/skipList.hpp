#ifndef skipList_
#define skipList_

#include "dictionary.hpp"
#include <limits>
#include <memory>
#include <ostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

// 第 0 层包含全部数对；更高层是随机选出的有序子集。
template<class K, class E>
class skipList : public dictionary<K, E> {
public:
    typedef std::pair<const K, E> value_type;

    explicit skipList(int maxLevel = 16, double probability = 0.5,
                      unsigned int seed = std::random_device{}());
    skipList(const skipList<K, E>& other);
    skipList<K, E>& operator=(const skipList<K, E>& other);
    ~skipList() override;

    bool empty() const override { return listSize == 0; }
    int size() const override { return listSize; }
    int levels() const { return activeLevels; }
    value_type* find(const K& theKey) const override;
    void insert(const value_type& thePair) override;
    void erase(const K& theKey) override;
    void output(std::ostream& out) const;
    void outputLevels(std::ostream& out) const;

private:
    struct node {
        value_type element;
        std::vector<node*> next;
        node(const value_type& thePair, int height)
            : element(thePair), next(height, nullptr) {}
    };

    static int checkedMaxLevel(int maxLevel);
    int randomLevel();
    node* lowerBound(const K& theKey, std::vector<node*>* update = nullptr) const;
    void swap(skipList<K, E>& other) noexcept;

    int maxLevels;
    int activeLevels;
    int listSize;
    double promotionProbability;
    std::mt19937 generator;
    std::uniform_real_distribution<double> random;
    // 各层的头指针代替带虚拟键的头结点，因此不需要“无穷大”键。
    std::vector<node*> head;
};

template<class K, class E>
int skipList<K, E>::checkedMaxLevel(int maxLevel) {
    if (maxLevel < 1)
        throw std::invalid_argument("Skip list maxLevel must be >= 1");
    return maxLevel;
}

template<class K, class E>
skipList<K, E>::skipList(int maxLevel, double probability, unsigned int seed)
    : maxLevels(checkedMaxLevel(maxLevel)), activeLevels(1), listSize(0),
      promotionProbability(probability), generator(seed), random(0.0, 1.0),
      head(maxLevels, nullptr) {
    if (!(probability > 0.0 && probability < 1.0))
        throw std::invalid_argument("Skip list probability must be in (0, 1)");
}

template<class K, class E>
skipList<K, E>::skipList(const skipList<K, E>& other)
    : skipList(other.maxLevels, other.promotionProbability, 0) {
    // 复制每个结点的高度和各层连接，不重新随机生成拓扑。
    // 临时容器负责在数对复制失败时释放已分配的结点。
    skipList<K, E> copy(other.maxLevels, other.promotionProbability, 0);
    std::vector<node*> tail(maxLevels, nullptr);
    for (node* current = other.head[0]; current != nullptr;
         current = current->next[0]) {
        int height = static_cast<int>(current->next.size());
        node* newNode = new node(current->element, height);
        for (int level = 0; level < height; ++level) {
            if (tail[level] == nullptr)
                copy.head[level] = newNode;
            else
                tail[level]->next[level] = newNode;
            tail[level] = newNode;
        }
        ++copy.listSize;
    }
    copy.activeLevels = other.activeLevels;
    copy.generator = other.generator;
    copy.random = other.random;
    swap(copy);
}

template<class K, class E>
skipList<K, E>& skipList<K, E>::operator=(const skipList<K, E>& other) {
    if (this != &other) {
        skipList<K, E> copy(other);
        swap(copy);
    }
    return *this;
}

template<class K, class E>
void skipList<K, E>::swap(skipList<K, E>& other) noexcept {
    std::swap(maxLevels, other.maxLevels);
    std::swap(activeLevels, other.activeLevels);
    std::swap(listSize, other.listSize);
    std::swap(promotionProbability, other.promotionProbability);
    std::swap(generator, other.generator);
    std::swap(random, other.random);
    head.swap(other.head);
}

template<class K, class E>
skipList<K, E>::~skipList() {
    node* current = head[0];
    while (current != nullptr) {
        node* nextNode = current->next[0];
        delete current;
        current = nextNode;
    }
}

template<class K, class E>
int skipList<K, E>::randomLevel() {
    int height = 1;
    while (height < maxLevels && random(generator) < promotionProbability)
        ++height;
    return height;
}

// 从最高层开始：向右越过小于目标的键，再下降一层。
// update[level] 记录该层的前驱；nullptr 表示前驱是层头。
template<class K, class E>
typename skipList<K, E>::node* skipList<K, E>::lowerBound(
    const K& theKey, std::vector<node*>* update) const {
    node* current = nullptr;
    for (int level = activeLevels - 1; level >= 0; --level) {
        node* nextNode = current == nullptr ? head[level] : current->next[level];
        while (nextNode != nullptr && nextNode->element.first < theKey) {
            current = nextNode;
            nextNode = current->next[level];
        }
        if (update != nullptr)
            (*update)[level] = current;
    }
    return current == nullptr ? head[0] : current->next[0];
}

template<class K, class E>
typename skipList<K, E>::value_type* skipList<K, E>::find(const K& theKey) const {
    node* current = lowerBound(theKey);
    // lowerBound 已保证 current 的键不小于 theKey。
    if (current != nullptr && !(theKey < current->element.first))
        return &current->element;
    return nullptr;
}

template<class K, class E>
void skipList<K, E>::insert(const value_type& thePair) {
    std::vector<node*> update(maxLevels, nullptr);
    node* current = lowerBound(thePair.first, &update);
    if (current != nullptr && !(thePair.first < current->element.first)) {
        current->element.second = thePair.second;
        return;
    }
    if (listSize == std::numeric_limits<int>::max())
        throw std::length_error("Skip list size is too large");

    int height = randomLevel();
    std::unique_ptr<node> newNode(new node(thePair, height));
    for (int level = 0; level < height; ++level) {
        node*& link = update[level] == nullptr
            ? head[level] : update[level]->next[level];
        newNode->next[level] = link;
        link = newNode.get();
    }
    newNode.release();
    if (height > activeLevels)
        activeLevels = height;
    ++listSize;
}

template<class K, class E>
void skipList<K, E>::erase(const K& theKey) {
    std::vector<node*> update(maxLevels, nullptr);
    node* current = lowerBound(theKey, &update);
    if (current == nullptr || theKey < current->element.first)
        return;

    int height = static_cast<int>(current->next.size());
    for (int level = 0; level < height; ++level) {
        node*& link = update[level] == nullptr
            ? head[level] : update[level]->next[level];
        link = current->next[level];
    }
    delete current;
    --listSize;
    while (activeLevels > 1 && head[activeLevels - 1] == nullptr)
        --activeLevels;
}

template<class K, class E>
void skipList<K, E>::output(std::ostream& out) const {
    for (node* current = head[0]; current != nullptr; current = current->next[0])
        out << current->element.first << ' ' << current->element.second << '\n';
}

template<class K, class E>
void skipList<K, E>::outputLevels(std::ostream& out) const {
    for (int level = activeLevels - 1; level >= 0; --level) {
        out << "Level " << level << ": head";
        for (node* current = head[level]; current != nullptr;
             current = current->next[level])
            out << " -> " << current->element.first;
        out << " -> null\n";
    }
}

template<class K, class E>
std::ostream& operator<<(std::ostream& out, const skipList<K, E>& x) {
    x.output(out);
    return out;
}

#endif
