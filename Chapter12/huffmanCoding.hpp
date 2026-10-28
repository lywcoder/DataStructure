#ifndef CHAPTER12_HUFFMAN_CODING_HPP
#define CHAPTER12_HUFFMAN_CODING_HPP

#include <cstdint>
#include <map>
#include <memory>
#include <limits>
#include <queue>
#include <stdexcept>
#include <string>
#include <vector>

// 对字节串建立霍夫曼前缀码；相同频率按字节值和创建顺序稳定打破平局。
class huffmanCoding {
public:
    explicit huffmanCoding(const std::string& trainingText) {
        std::map<unsigned char, std::uint64_t> frequencies;
        for (unsigned char byte : trainingText) ++frequencies[byte];
        build(frequencies);
    }
    explicit huffmanCoding(const std::map<unsigned char, std::uint64_t>& frequencies) {
        build(frequencies);
    }

    const std::map<unsigned char, std::string>& codes() const { return codes_; }
    std::uint64_t weightedPathLength() const { return weightedPathLength_; }

    std::string encode(const std::string& text) const {
        std::string bits;
        for (unsigned char byte : text) {
            std::map<unsigned char, std::string>::const_iterator it = codes_.find(byte);
            if (it == codes_.end()) throw std::invalid_argument("symbol not in code table");
            bits += it->second;
        }
        return bits;
    }
    std::string decode(const std::string& bits) const {
        if (!root_) {
            if (!bits.empty()) throw std::invalid_argument("nonempty bits for empty code table");
            return std::string();
        }
        std::string text;
        if (root_->symbol >= 0) {
            for (char bit : bits) {
                if (bit != '0') throw std::invalid_argument("invalid single-symbol code");
                text += static_cast<char>(root_->symbol);
            }
            return text;
        }
        NodePtr current = root_;
        for (char bit : bits) {
            if (bit != '0' && bit != '1') throw std::invalid_argument("invalid bit");
            current = bit == '0' ? current->left : current->right;
            if (!current) throw std::invalid_argument("invalid code path");
            if (current->symbol >= 0) {
                text += static_cast<char>(current->symbol);
                current = root_;
            }
        }
        if (current != root_) throw std::invalid_argument("incomplete final code");
        return text;
    }

private:
    struct Node;
    typedef std::shared_ptr<Node> NodePtr;
    struct Node {
        std::uint64_t weight;
        std::size_t order;
        int symbol; // -1 表示内部结点
        NodePtr left, right;
        Node(std::uint64_t w, std::size_t o, int s, NodePtr l = NodePtr(), NodePtr r = NodePtr())
            : weight(w), order(o), symbol(s), left(l), right(r) {}
    };
    struct LowerFirst {
        bool operator()(const NodePtr& a, const NodePtr& b) const {
            if (a->weight != b->weight) return a->weight > b->weight;
            return a->order > b->order;
        }
    };
    NodePtr root_;
    std::map<unsigned char, std::string> codes_;
    std::uint64_t weightedPathLength_ = 0;

    void build(const std::map<unsigned char, std::uint64_t>& frequencies) {
        std::priority_queue<NodePtr, std::vector<NodePtr>, LowerFirst> queue;
        std::size_t order = 0;
        for (const auto& item : frequencies) {
            if (item.second == 0) throw std::invalid_argument("zero symbol frequency");
            queue.push(NodePtr(new Node(item.second, order++, item.first)));
        }
        while (queue.size() > 1) {
            NodePtr left = queue.top(); queue.pop();
            NodePtr right = queue.top(); queue.pop();
            if (left->weight > std::numeric_limits<std::uint64_t>::max() - right->weight)
                throw std::overflow_error("frequency sum overflow");
            queue.push(NodePtr(new Node(left->weight + right->weight,
                                        order++, -1, left, right)));
        }
        if (!queue.empty()) root_ = queue.top();
        if (root_) makeCodes(root_, "");
        for (const auto& item : frequencies) {
            const std::uint64_t length = static_cast<std::uint64_t>(codes_.at(item.first).size());
            const std::uint64_t limit = std::numeric_limits<std::uint64_t>::max();
            if (item.second > limit / length || weightedPathLength_ > limit - item.second * length)
                throw std::overflow_error("weighted path length overflow");
            weightedPathLength_ += item.second * length;
        }
    }
    void makeCodes(const NodePtr& node, const std::string& prefix) {
        if (node->symbol >= 0) {
            codes_[static_cast<unsigned char>(node->symbol)] = prefix.empty() ? "0" : prefix;
            return;
        }
        makeCodes(node->left, prefix + '0');
        makeCodes(node->right, prefix + '1');
    }
};

#endif
