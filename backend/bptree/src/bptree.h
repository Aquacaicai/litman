#ifndef BPTREE_H
#define BPTREE_H

#define LEAF true
#include <iostream>
#include <fstream> 
#include <vector>
#include <queue>
#include <map>
#include <sys/socket.h>

std::ostream& operator<<(std::ostream& os, const std::vector<int>& vec) {
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        os << vec[i];
        if (i != vec.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

template<typename KeyT, typename ValT>
class Node {
public:
    bool leaf;
    Node* parent;   //for non-root only
    Node* next;     //for leaf only
    std::vector<KeyT> key;
    std::vector<Node*> ptr2node;    //for non-leaf only
    std::vector<ValT*> ptr2val;     //for leaf only
    Node(bool _leaf = false);
};

template<typename KeyT, typename ValT>
class BPTree {
private:
    int order;
    Node<KeyT, ValT>* root;
    inline int keyIndex(Node<KeyT, ValT>* _node, KeyT _key);
    inline std::pair<Node<KeyT, ValT>*, int> keyIndexInLeaf(KeyT _key);
    Node<KeyT, ValT>* splitLeaf(Node<KeyT, ValT>* _leaf);
    void createIndex(Node<KeyT, ValT>* _new_node, KeyT _index);
    std::pair<Node<KeyT, ValT>*, KeyT> splitNode(Node<KeyT, ValT>* _node);
    void handleNodeUnderflow(Node<KeyT, ValT>* node);
    void handleLeafUnderflow(Node<KeyT, ValT>* leaf);

public:
    BPTree(int order);
    std::vector<KeyT> getAllKeys();
    std::vector<ValT> getAllValues();
    void insert(KeyT _key, ValT _val);
    void erase(KeyT _key);
    bool update(KeyT _key, ValT _new_val);
    ValT* find(KeyT _key);
    void deserialize(const std::string& filename);
    void serialize(const std::string& filename);
};

template<typename KeyT, typename ValT>
std::vector<KeyT> BPTree<KeyT, ValT>::getAllKeys() {
    std::vector<KeyT> result;

    if (!root) {
        return result;
    }

    Node<KeyT, ValT>* leaf = root;
    while (!leaf->leaf) {
        leaf = leaf->ptr2node[0];
    }

    while (leaf) {
        for (const KeyT& k : leaf->key) {
            result.push_back(k);
        }
        leaf = leaf->next;
    }

    return result;
}

template<typename KeyT, typename ValT>
std::vector<ValT> BPTree<KeyT, ValT>::getAllValues() {
    std::vector<ValT> result;

    if (!root) {
        return result;
    }

    Node<KeyT, ValT>* leaf = root;
    while (!leaf->leaf) {
        leaf = leaf->ptr2node[0];
    }

    while (leaf) {
        for (ValT* val_ptr : leaf->ptr2val) {
            if (val_ptr) {
                result.push_back(*val_ptr);
            }
        }
        leaf = leaf->next;
    }
    return result;
}


template<typename KeyT, typename ValT>
inline int BPTree<KeyT, ValT>::keyIndex(Node<KeyT, ValT>* _node, KeyT _key) {
    int loc = -1;
    int size = _node->key.size();
    while (loc + 1 < size && _node->key[loc + 1] <= _key) {
        loc++;
    }
    return loc;
}

template<typename KeyT, typename ValT>
inline std::pair<Node<KeyT, ValT>*, int> BPTree<KeyT, ValT>::keyIndexInLeaf(KeyT _key) {
    if (root == nullptr) {
        return std::make_pair(nullptr, -1);
    }
    Node<KeyT, ValT>* node = root;
    while (true) {
        int loc = keyIndex(node, _key);
        if (node->leaf) {
            return std::make_pair(node, loc);
        }
        else {
            node = node->ptr2node[loc + 1];
        }
    }
}

template<typename KeyT, typename ValT>
Node<KeyT, ValT>* BPTree<KeyT, ValT>::splitLeaf(Node<KeyT, ValT>* _leaf) {
    Node<KeyT, ValT>* new_leaf = new Node<KeyT, ValT>(LEAF);
    new_leaf->next = _leaf->next;
    _leaf->next = new_leaf;
    new_leaf->parent = _leaf->parent;
    int mid = _leaf->key.size() / 2;
    new_leaf->key.assign(_leaf->key.begin() + mid, _leaf->key.end());
    new_leaf->ptr2val.assign(_leaf->ptr2val.begin() + mid, _leaf->ptr2val.end());
    _leaf->key.erase(_leaf->key.begin() + mid, _leaf->key.end());
    _leaf->ptr2val.erase(_leaf->ptr2val.begin() + mid, _leaf->ptr2val.end());
    return new_leaf;
}

template<typename KeyT, typename ValT>
std::pair<Node<KeyT, ValT>*, KeyT> BPTree<KeyT, ValT>::splitNode(Node<KeyT, ValT>* _node) {
    Node<KeyT, ValT>* new_node = new Node<KeyT, ValT>();
    new_node->parent = _node->parent;
    int mid = (_node->key.size() + 1) / 2 - 1;
    KeyT push_key = _node->key[mid];
    new_node->key.assign(_node->key.begin() + mid + 1, _node->key.end());
    new_node->ptr2node.assign(_node->ptr2node.begin() + mid + 1, _node->ptr2node.end());
    _node->key.erase(_node->key.begin() + mid, _node->key.end());
    _node->ptr2node.erase(_node->ptr2node.begin() + mid + 1, _node->ptr2node.end());
    for (Node<KeyT, ValT>* each : new_node->ptr2node)
        each->parent = new_node;
    return std::make_pair(new_node, push_key);
}

template<typename KeyT, typename ValT>
void BPTree<KeyT, ValT>::createIndex(Node<KeyT, ValT>* _new_node, KeyT _index) {
    Node<KeyT, ValT>* node = _new_node->parent;
    int loc = keyIndex(node, _index);
    node->key.insert(node->key.begin() + loc + 1, _index);
    node->ptr2node.insert(node->ptr2node.begin() + loc + 2, _new_node);
    if (node->key.size() > order) {
        std::pair<Node<KeyT, ValT>*, KeyT> pair = splitNode(node);
        Node<KeyT, ValT>* new_node = pair.first;
        KeyT push_key = pair.second;
        if (node == root) {
            Node<KeyT, ValT>* new_root = new Node<KeyT, ValT>();
            new_root->key.push_back(push_key);
            new_root->ptr2node.push_back(node);
            new_root->ptr2node.push_back(new_node);
            root = new_root;
            node->parent = root;
            new_node->parent = root;
        }
        else {
            createIndex(new_node, push_key);
        }
    }
}

template<typename KeyT, typename ValT>
Node<KeyT, ValT>::Node(bool _leaf) : leaf(_leaf), parent(nullptr), next(nullptr) {}

template<typename KeyT, typename ValT>
BPTree<KeyT, ValT>::BPTree(int order) : root(nullptr), order(order) {}


template<typename KeyT, typename ValT>
void BPTree<KeyT, ValT>::insert(KeyT _key, ValT _val) {
    if (root == nullptr) {
        root = new Node<KeyT, ValT>(LEAF);
        root->key.push_back(_key);
        root->ptr2val.emplace_back(new ValT(_val));
        root->ptr2node.push_back(nullptr);
        return;
    }
    std::pair<Node<KeyT, ValT>*, int> pair = keyIndexInLeaf(_key);
    Node<KeyT, ValT>* leaf = pair.first;
    int loc = pair.second;
    if (loc != -1 && leaf->key[loc] == _key) {
        std::cout << "Key " << _key << " with value " << *(leaf->ptr2val[loc]) << " is already in B+ tree, overwrite it with new val " << _val << std::endl;
        *(leaf->ptr2val[loc]) = _val;
        return;
    }
    leaf->key.insert(leaf->key.begin() + loc + 1, _key);
    leaf->ptr2val.insert(leaf->ptr2val.begin() + loc + 1, new ValT(_val));
    if (leaf->key.size() > order) {
        Node<KeyT, ValT>* new_leaf = splitLeaf(leaf);
        if (leaf == root) {
            Node<KeyT, ValT>* new_root = new Node<KeyT, ValT>();
            new_root->key.push_back(new_leaf->key[0]);
            new_root->ptr2node.push_back(leaf);
            new_root->ptr2node.push_back(new_leaf);
            root = new_root;
            leaf->parent = root;
            new_leaf->parent = root;
        }
        else {
            createIndex(new_leaf, new_leaf->key[0]);
        }
    }
}
template<typename KeyT, typename ValT>
void BPTree<KeyT, ValT>::erase(KeyT _key) {
    std::pair<Node<KeyT, ValT>*, int> pair = keyIndexInLeaf(_key);
    Node<KeyT, ValT>* leaf = pair.first;
    int loc = pair.second;

    if (loc == -1 || leaf->key[loc] != _key) {
        std::cout << "Key " << _key << " is not in B+ tree" << std::endl;
        return;
    }

    leaf->key.erase(leaf->key.begin() + loc);
    delete leaf->ptr2val[loc];
    leaf->ptr2val.erase(leaf->ptr2val.begin() + loc);

    // no underflow
    if (leaf->key.size() >= (order + 1) / 2 || leaf == root) {
        return;
    }

    handleLeafUnderflow(leaf);
}

template<typename KeyT, typename ValT>
void BPTree<KeyT, ValT>::handleLeafUnderflow(Node<KeyT, ValT>* leaf) {
    Node<KeyT, ValT>* parent = leaf->parent;
    int idx = -1;

    // index in parent node
    for (size_t i = 0; i < parent->ptr2node.size(); ++i) {
        if (parent->ptr2node[i] == leaf) {
            idx = i;
            break;
        }
    }

    // borrow from left sib
    if (idx > 0) {
        Node<KeyT, ValT>* left_sibling = parent->ptr2node[idx - 1];
        if (left_sibling->key.size() > (order + 1) / 2) {
            // borrow
            leaf->key.insert(leaf->key.begin(), left_sibling->key.back());
            leaf->ptr2val.insert(leaf->ptr2val.begin(), left_sibling->ptr2val.back());
            left_sibling->key.pop_back();
            left_sibling->ptr2val.pop_back();

            // update parent index
            parent->key[idx - 1] = leaf->key[0];
            return;
        }
    }

    // borrow from right sib
    if (idx < parent->ptr2node.size() - 1) {
        Node<KeyT, ValT>* right_sibling = parent->ptr2node[idx + 1];
        if (right_sibling->key.size() > (order + 1) / 2) {
            // borrow
            leaf->key.push_back(right_sibling->key.front());
            leaf->ptr2val.push_back(right_sibling->ptr2val.front());
            right_sibling->key.erase(right_sibling->key.begin());
            right_sibling->ptr2val.erase(right_sibling->ptr2val.begin());

            // update parent index
            parent->key[idx] = right_sibling->key[0];
            return;
        }
    }

    // can't borrow, merge
    if (idx > 0) {
        // left
        Node<KeyT, ValT>* left_sibling = parent->ptr2node[idx - 1];
        left_sibling->key.insert(left_sibling->key.end(), leaf->key.begin(), leaf->key.end());
        left_sibling->ptr2val.insert(left_sibling->ptr2val.end(), leaf->ptr2val.begin(), leaf->ptr2val.end());
        left_sibling->next = leaf->next;

        parent->key.erase(parent->key.begin() + idx - 1);
        parent->ptr2node.erase(parent->ptr2node.begin() + idx);

        delete leaf;

        if (parent->key.size() < (order + 1) / 2 && parent != root) {
            handleNodeUnderflow(parent);
        }
    }
    else {
        // right
        Node<KeyT, ValT>* right_sibling = parent->ptr2node[idx + 1];
        leaf->key.insert(leaf->key.end(), right_sibling->key.begin(), right_sibling->key.end());
        leaf->ptr2val.insert(leaf->ptr2val.end(), right_sibling->ptr2val.begin(), right_sibling->ptr2val.end());
        leaf->next = right_sibling->next;

        parent->key.erase(parent->key.begin() + idx);
        parent->ptr2node.erase(parent->ptr2node.begin() + idx + 1);

        delete right_sibling;

        if (parent->key.size() < (order + 1) / 2 && parent != root) {
            handleNodeUnderflow(parent);
        }
    }

    // parent is empty
    if (root->key.empty() && !root->ptr2node.empty()) {
        Node<KeyT, ValT>* new_root = root->ptr2node[0];
        delete root;
        root = new_root;
        root->parent = nullptr;
    }
}

template<typename KeyT, typename ValT>
void BPTree<KeyT, ValT>::handleNodeUnderflow(Node<KeyT, ValT>* node) {
    Node<KeyT, ValT>* parent = node->parent;
    int idx = -1;

    for (size_t i = 0; i < parent->ptr2node.size(); ++i) {
        if (parent->ptr2node[i] == node) {
            idx = i;
            break;
        }
    }

    // borrow from left
    if (idx > 0) {
        Node<KeyT, ValT>* left_sibling = parent->ptr2node[idx - 1];
        if (left_sibling->key.size() > (order + 1) / 2) {
            // borrow
            node->key.insert(node->key.begin(), parent->key[idx - 1]);
            node->ptr2node.insert(node->ptr2node.begin(), left_sibling->ptr2node.back());
            left_sibling->ptr2node.back()->parent = node;

            // update parent index
            parent->key[idx - 1] = left_sibling->key.back();
            left_sibling->key.pop_back();
            left_sibling->ptr2node.pop_back();
            return;
        }
    }

    // borrow from right
    if (idx < parent->ptr2node.size() - 1) {
        Node<KeyT, ValT>* right_sibling = parent->ptr2node[idx + 1];
        if (right_sibling->key.size() > (order + 1) / 2) {
            // borrow
            node->key.push_back(parent->key[idx]);
            node->ptr2node.push_back(right_sibling->ptr2node.front());
            right_sibling->ptr2node.front()->parent = node;

            // update parent index
            parent->key[idx] = right_sibling->key.front();
            right_sibling->key.erase(right_sibling->key.begin());
            right_sibling->ptr2node.erase(right_sibling->ptr2node.begin());
            return;
        }
    }

    // can't borrow, merge
    if (idx > 0) {
        // left
        Node<KeyT, ValT>* left_sibling = parent->ptr2node[idx - 1];
        left_sibling->key.push_back(parent->key[idx - 1]);
        left_sibling->key.insert(left_sibling->key.end(), node->key.begin(), node->key.end());
        left_sibling->ptr2node.insert(left_sibling->ptr2node.end(), node->ptr2node.begin(), node->ptr2node.end());

        for (auto& child : node->ptr2node) {
            child->parent = left_sibling;
        }

        parent->key.erase(parent->key.begin() + idx - 1);
        parent->ptr2node.erase(parent->ptr2node.begin() + idx);

        delete node;

        if (parent->key.size() < (order + 1) / 2 && parent != root) {
            handleNodeUnderflow(parent);
        }
    }
    else {
        // right
        Node<KeyT, ValT>* right_sibling = parent->ptr2node[idx + 1];
        node->key.push_back(parent->key[idx]);
        node->key.insert(node->key.end(), right_sibling->key.begin(), right_sibling->key.end());
        node->ptr2node.insert(node->ptr2node.end(), right_sibling->ptr2node.begin(), right_sibling->ptr2node.end());

        for (auto& child : right_sibling->ptr2node) {
            child->parent = node;
        }

        parent->key.erase(parent->key.begin() + idx);
        parent->ptr2node.erase(parent->ptr2node.begin() + idx + 1);

        delete right_sibling;

        if (parent->key.size() < (order + 1) / 2 && parent != root) {
            handleNodeUnderflow(parent);
        }
    }

    // parent is empty
    if (root->key.empty() && !root->ptr2node.empty()) {
        Node<KeyT, ValT>* new_root = root->ptr2node[0];
        delete root;
        root = new_root;
        root->parent = nullptr;
    }
}

template<typename KeyT, typename ValT>
ValT* BPTree<KeyT, ValT>::find(KeyT _key) {
    std::pair<Node<KeyT, ValT>*, int> pair = keyIndexInLeaf(_key);
    Node<KeyT, ValT>* leaf = pair.first;
    int loc = pair.second;
    if (loc == -1 || leaf->key[loc] != _key) {
        // std::cout << "Key " << _key << " is not in B+ tree" << std::endl;
        return nullptr;
    }
    else {
        return leaf->ptr2val[loc];
    }
}


template<typename KeyT, typename ValT>
bool BPTree<KeyT, ValT>::update(KeyT _key, ValT _new_val) {
    std::pair<Node<KeyT, ValT>*, int> pair = keyIndexInLeaf(_key);
    Node<KeyT, ValT>* leaf = pair.first;
    int loc = pair.second;
    if (loc == -1 || leaf->key[loc] != _key) {
        std::cout << "Key " << _key << " is not in B+ tree" << std::endl;
        return false;
    }
    else {
        *(leaf->ptr2val[loc]) = _new_val;
        return true;
    }
}


template<typename KeyT, typename ValT>
void BPTree<KeyT, ValT>::serialize(const std::string& filename) {
    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Error opening file for writing!" << std::endl;
        return;
    }

    // order
    outfile.write(reinterpret_cast<const char*>(&order), sizeof(order));

    // is empty
    bool has_root = (root != nullptr);
    outfile.write(reinterpret_cast<const char*>(&has_root), sizeof(has_root));
    if (!has_root) {
        outfile.close();
        return;
    }

    // level-order trav
    std::map<Node<KeyT, ValT>*, size_t> node_id_map; // unique id for every node
    std::queue<Node<KeyT, ValT>*> q;
    std::vector<Node<KeyT, ValT>*> all_nodes;

    q.push(root);
    size_t id = 0;
    node_id_map[root] = id++;
    all_nodes.push_back(root);

    // id
    while (!q.empty()) {
        Node<KeyT, ValT>* node = q.front();
        q.pop();

        if (!node->leaf) {
            for (Node<KeyT, ValT>* child : node->ptr2node) {
                if (child && node_id_map.find(child) == node_id_map.end()) {
                    node_id_map[child] = id++;
                    all_nodes.push_back(child);
                    q.push(child);
                }
            }
        }
    }

    // node count
    size_t total_nodes = all_nodes.size();
    outfile.write(reinterpret_cast<const char*>(&total_nodes), sizeof(total_nodes));

    // data
    for (Node<KeyT, ValT>* node : all_nodes) {
        // id
        size_t node_id = node_id_map[node];
        outfile.write(reinterpret_cast<const char*>(&node_id), sizeof(node_id));

        // type
        outfile.write(reinterpret_cast<const char*>(&node->leaf), sizeof(node->leaf));

        // parent
        size_t parent_id = (node->parent) ? node_id_map[node->parent] : size_t(-1);
        outfile.write(reinterpret_cast<const char*>(&parent_id), sizeof(parent_id));

        // next
        size_t next_id = (node->leaf && node->next) ? node_id_map[node->next] : size_t(-1);
        outfile.write(reinterpret_cast<const char*>(&next_id), sizeof(next_id));

        // keys
        size_t key_count = node->key.size();
        outfile.write(reinterpret_cast<const char*>(&key_count), sizeof(key_count));
        for (const KeyT& key : node->key) {
            // for str
            if constexpr (std::is_same_v<KeyT, std::string>) {
                size_t str_len = key.length();
                outfile.write(reinterpret_cast<const char*>(&str_len), sizeof(str_len));
                outfile.write(key.c_str(), str_len);
            }
            else {
                outfile.write(reinterpret_cast<const char*>(&key), sizeof(KeyT));
            }
        }

        if (node->leaf) {
            // v
            for (size_t i = 0; i < key_count; i++) {
                // vec<int>
                if constexpr (std::is_same_v<ValT, std::vector<int>>) {
                    const auto& vec = *(node->ptr2val[i]);
                    size_t vec_size = vec.size();
                    outfile.write(reinterpret_cast<const char*>(&vec_size), sizeof(vec_size));
                    for (int val : vec) {
                        outfile.write(reinterpret_cast<const char*>(&val), sizeof(int));
                    }
                }
                else {
                    outfile.write(reinterpret_cast<const char*>(node->ptr2val[i]), sizeof(ValT));
                }
            }
        }
        else {
            // child id
            size_t child_count = node->ptr2node.size();
            outfile.write(reinterpret_cast<const char*>(&child_count), sizeof(child_count));
            for (Node<KeyT, ValT>* child : node->ptr2node) {
                size_t child_id = (child) ? node_id_map[child] : size_t(-1);
                outfile.write(reinterpret_cast<const char*>(&child_id), sizeof(child_id));
            }
        }
    }

    outfile.close();
}

template<typename KeyT, typename ValT>
void BPTree<KeyT, ValT>::deserialize(const std::string& filename) {
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "Error opening file for reading!" << std::endl;
        return;
    }

    // clean
    if (root) {
        // trav remove
        std::queue<Node<KeyT, ValT>*> q;
        q.push(root);
        while (!q.empty()) {
            Node<KeyT, ValT>* node = q.front();
            q.pop();
            if (!node->leaf) {
                for (Node<KeyT, ValT>* child : node->ptr2node) {
                    if (child) q.push(child);
                }
            }
            else {
                // release
                for (ValT* val_ptr : node->ptr2val) {
                    delete val_ptr;
                }
            }
            delete node;
        }
        root = nullptr;
    }

    // order
    infile.read(reinterpret_cast<char*>(&order), sizeof(order));

    // is empty
    bool has_root;
    infile.read(reinterpret_cast<char*>(&has_root), sizeof(has_root));
    if (!has_root) {
        infile.close();
        return;
    }

    // count
    size_t total_nodes;
    infile.read(reinterpret_cast<char*>(&total_nodes), sizeof(total_nodes));

    std::vector<Node<KeyT, ValT>*> nodes(total_nodes, nullptr);
    std::vector<size_t> parent_ids(total_nodes);
    std::vector<size_t> next_ids(total_nodes);
    std::vector<std::vector<size_t>> children_ids(total_nodes);

    // create nodes
    for (size_t i = 0; i < total_nodes; i++) {
        size_t node_id;
        infile.read(reinterpret_cast<char*>(&node_id), sizeof(node_id));

        bool is_leaf;
        infile.read(reinterpret_cast<char*>(&is_leaf), sizeof(is_leaf));

        nodes[node_id] = new Node<KeyT, ValT>(is_leaf);

        // parent
        size_t parent_id;
        infile.read(reinterpret_cast<char*>(&parent_id), sizeof(parent_id));
        parent_ids[node_id] = parent_id;

        // next
        size_t next_id;
        infile.read(reinterpret_cast<char*>(&next_id), sizeof(next_id));
        next_ids[node_id] = next_id;

        // k
        size_t key_count;
        infile.read(reinterpret_cast<char*>(&key_count), sizeof(key_count));
        nodes[node_id]->key.resize(key_count);
        for (size_t j = 0; j < key_count; j++) {
            // for str
            if constexpr (std::is_same_v<KeyT, std::string>) {
                size_t str_len;
                infile.read(reinterpret_cast<char*>(&str_len), sizeof(str_len));
                char* buffer = new char[str_len + 1];
                infile.read(buffer, str_len);
                buffer[str_len] = '\0';
                nodes[node_id]->key[j] = std::string(buffer);
                delete[] buffer;
            }
            else {
                infile.read(reinterpret_cast<char*>(&nodes[node_id]->key[j]), sizeof(KeyT));
            }
        }

        if (is_leaf) {
            // v
            nodes[node_id]->ptr2val.resize(key_count);
            for (size_t j = 0; j < key_count; j++) {
                // for vec<int>
                if constexpr (std::is_same_v<ValT, std::vector<int>>) {
                    size_t vec_size;
                    infile.read(reinterpret_cast<char*>(&vec_size), sizeof(vec_size));
                    std::vector<int>* vec_ptr = new std::vector<int>(vec_size);
                    for (size_t k = 0; k < vec_size; k++) {
                        int val;
                        infile.read(reinterpret_cast<char*>(&val), sizeof(int));
                        (*vec_ptr)[k] = val;
                    }
                    nodes[node_id]->ptr2val[j] = vec_ptr;
                }
                else {
                    ValT* val_ptr = new ValT();
                    infile.read(reinterpret_cast<char*>(val_ptr), sizeof(ValT));
                    nodes[node_id]->ptr2val[j] = val_ptr;
                }
            }
        }
        else {
            // child
            size_t child_count;
            infile.read(reinterpret_cast<char*>(&child_count), sizeof(child_count));
            children_ids[node_id].resize(child_count);
            for (size_t j = 0; j < child_count; j++) {
                size_t child_id;
                infile.read(reinterpret_cast<char*>(&child_id), sizeof(child_id));
                children_ids[node_id][j] = child_id;
            }
            nodes[node_id]->ptr2node.resize(child_count, nullptr);
        }
    }

    // link
    for (size_t i = 0; i < total_nodes; i++) {
        // parent
        if (parent_ids[i] != size_t(-1)) {
            nodes[i]->parent = nodes[parent_ids[i]];
        }
        else {
            root = nodes[i]; // root
        }

        // next
        if (nodes[i]->leaf && next_ids[i] != size_t(-1)) {
            nodes[i]->next = nodes[next_ids[i]];
        }

        // child
        if (!nodes[i]->leaf) {
            for (size_t j = 0; j < children_ids[i].size(); j++) {
                if (children_ids[i][j] != size_t(-1)) {
                    nodes[i]->ptr2node[j] = nodes[children_ids[i][j]];
                }
            }
        }
    }

    infile.close();
}

#endif // BPTREE_H