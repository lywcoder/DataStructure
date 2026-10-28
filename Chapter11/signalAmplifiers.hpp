#ifndef CHAPTER11_SIGNAL_AMPLIFIERS_HPP
#define CHAPTER11_SIGNAL_AMPLIFIERS_HPP

#include "binaryTreeNode.hpp"
#include <stdexcept>
#include <vector>

// edgeLoss 是父结点到本结点的损耗；根是信号源，其 edgeLoss 为 0。
struct signalNode {
    int id;
    int edgeLoss;
    signalNode(int idValue, int loss) : id(idValue), edgeLoss(loss) {}
};

// 信号从根向叶传播。放大器装在结点处，使其所有出边的累计损耗从 0 重新计算。
// 返回安装结点的 id（先序）。这是保证约束成立的贪心布置，不承诺安装数最少。
inline void placeSignalAmplifiers(binaryTreeNode<signalNode>* node,
                                  int incomingLoss, int maxLoss,
                                  bool isRoot, std::vector<int>& result) {
    if (!node) return;
    binaryTreeNode<signalNode>* children[2] = {node->leftChild, node->rightChild};
    bool needsAmplifier = false;
    for (int i = 0; i < 2; ++i) {
        if (!children[i]) continue;
        const int edge = children[i]->element.edgeLoss;
        if (edge < 0) throw std::invalid_argument("negative edge loss");
        if (edge > maxLoss) throw std::invalid_argument("edge exceeds max loss");
        if (incomingLoss > maxLoss - edge) needsAmplifier = true;
    }
    if (needsAmplifier) {
        if (isRoot) throw std::logic_error("source edge exceeds max loss");
        result.push_back(node->element.id);
        incomingLoss = 0;
    }
    for (int i = 0; i < 2; ++i)
        if (children[i])
            placeSignalAmplifiers(children[i], incomingLoss + children[i]->element.edgeLoss,
                                  maxLoss, false, result);
}

inline std::vector<int> setSignalAmplifiers(binaryTreeNode<signalNode>* root, int maxLoss) {
    if (maxLoss < 0) throw std::invalid_argument("negative max loss");
    if (root && root->element.edgeLoss != 0)
        throw std::invalid_argument("root edge loss must be zero");
    std::vector<int> result;
    placeSignalAmplifiers(root, 0, maxLoss, true, result);
    return result;
}

#endif
