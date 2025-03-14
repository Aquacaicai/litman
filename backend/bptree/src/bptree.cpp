#include "bptree.h"
#include <pybind11/pybind11.h>

template<typename KeyType>
BPTreeImpl<KeyType>::BPTreeImpl(int t, const std::string& filename) : t(t), root(nullptr), filename(filename) {}

template<typename KeyType>
BPTreeImpl<KeyType>::~BPTreeImpl() {
    clear(root);
}

template<typename KeyType>
void BPTreeImpl<KeyType>::clear(Node* node) {
    if (node) {
        if (!node->isLeaf) {
            for (auto child : node->children) {
                clear(child);
            }
        }
        delete node;
    }
}

template<typename KeyType>
BPTreeImpl<KeyType>::Node::Node(bool leaf) : isLeaf(leaf) {}

template<typename KeyType>
BPTreeImpl<KeyType>::Node::~Node() {}

template<typename KeyType>
void BPTreeImpl<KeyType>::insert(const KeyType& key) {
    if (!root) {
        root = new Node(true);
        root->keys.push_back(key);
    }
    else {
        if (root->keys.size() == 2 * t - 1) {
            Node* newRoot = new Node(false);
            newRoot->children.push_back(root);
            splitChild(newRoot, 0, root);
            root = newRoot;
        }
        insertNonFull(root, key);
    }
}

template<typename KeyType>
void BPTreeImpl<KeyType>::insertNonFull(Node* node, const KeyType& key) {
    int i = node->keys.size() - 1;

    if (node->isLeaf) {
        node->keys.push_back(KeyType()); // key type
        while (i >= 0 && node->keys[i] > key) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
    }
    else {
        while (i >= 0 && node->keys[i] > key) {
            i--;
        }
        i++;
        if (node->children[i]->keys.size() == 2 * t - 1) {
            splitChild(node, i, node->children[i]);
            if (node->keys[i] < key) {
                i++;
            }
        }
        insertNonFull(node->children[i], key);
    }
}

template<typename KeyType>
void BPTreeImpl<KeyType>::splitChild(Node* parent, int i, Node* child) {
    Node* newChild = new Node(child->isLeaf);
    parent->children.insert(parent->children.begin() + i + 1, newChild);
    parent->keys.insert(parent->keys.begin() + i, child->keys[t - 1]);

    newChild->keys.assign(child->keys.begin() + t, child->keys.end());
    child->keys.resize(t - 1);

    if (!child->isLeaf) {
        newChild->children.assign(child->children.begin() + t, child->children.end());
        child->children.resize(t);
    }
}

template<typename KeyType>
bool BPTreeImpl<KeyType>::search(const KeyType& key) {
    return root ? search(root, key) : false;
}

template<typename KeyType>
bool BPTreeImpl<KeyType>::search(Node* node, const KeyType& key) {
    int i = 0;
    while (i < node->keys.size() && key > node->keys[i]) {
        i++;
    }
    if (i < node->keys.size() && key == node->keys[i]) {
        return true;
    }
    return node->isLeaf ? false : search(node->children[i], key);
}

template<typename KeyType>
void BPTreeImpl<KeyType>::save() {
    std::ofstream out(filename, std::ios::binary);
    if (out.is_open()) {
        saveNode(out, root);
        out.close();
    }
}

template<typename KeyType>
void BPTreeImpl<KeyType>::load() {
    clear(root);
    root = nullptr;

    std::ifstream in(filename, std::ios::binary);
    if (in.is_open()) {
        root = loadNode(in);
        in.close();
    }
}

template<typename KeyType>
void BPTreeImpl<KeyType>::saveNode(std::ofstream& out, Node* node) {
    if (!node) {
        bool isNull = true;
        out.write(reinterpret_cast<const char*>(&isNull), sizeof(isNull));
        return;
    }

    bool isNull = false;
    out.write(reinterpret_cast<const char*>(&isNull), sizeof(isNull));
    out.write(reinterpret_cast<const char*>(&node->isLeaf), sizeof(node->isLeaf));

    size_t size = node->keys.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    out.write(reinterpret_cast<const char*>(node->keys.data()), size * sizeof(KeyType));

    if (!node->isLeaf) {
        for (Node* child : node->children) {
            saveNode(out, child);
        }
    }
}

template<typename KeyType>
typename BPTreeImpl<KeyType>::Node* BPTreeImpl<KeyType>::loadNode(std::ifstream& in) {
    bool isNull;
    in.read(reinterpret_cast<char*>(&isNull), sizeof(isNull));
    if (isNull) return nullptr;

    bool isLeaf;
    in.read(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));

    Node* node = new Node(isLeaf);
    size_t size;
    in.read(reinterpret_cast<char*>(&size), sizeof(size));

    node->keys.resize(size);
    in.read(reinterpret_cast<char*>(node->keys.data()), size * sizeof(KeyType));

    if (!isLeaf) {
        node->children.resize(size + 1);
        for (size_t i = 0; i <= size; ++i) {
            node->children[i] = loadNode(in);
        }
    }

    return node;
}

// impl templates
template class BPTreeImpl<int>;
template class BPTreeImpl<FixedString>;

// IntBPTree impl
IntBPTree::IntBPTree(int t, const std::string& filename) : impl(t, filename) {}

void IntBPTree::insert(int key) {
    impl.insert(key);
}

bool IntBPTree::search(int key) {
    return impl.search(key);
}

void IntBPTree::save() {
    impl.save();
}

void IntBPTree::load() {
    impl.load();
}

// StringBPTree impl
StringBPTree::StringBPTree(int t, const std::string& filename) : impl(t, filename) {}

void StringBPTree::insert(const std::string& key) {
    impl.insert(FixedString(key));
}

bool StringBPTree::search(const std::string& key) {
    return impl.search(FixedString(key));
}

void StringBPTree::save() {
    impl.save();
}

void StringBPTree::load() {
    impl.load();
}


namespace py = pybind11;

PYBIND11_MODULE(_bptree, m) {
    m.doc() = "B+ tree module";

    py::class_<IntBPTree>(m, "IntBPTree")
        .def(py::init<int, const std::string&>(), py::arg("t"), py::arg("filename"))
        .def("insert", &IntBPTree::insert, py::arg("key"))
        .def("search", &IntBPTree::search, py::arg("key"))
        .def("save", &IntBPTree::save)
        .def("load", &IntBPTree::load);

    py::class_<StringBPTree>(m, "StringBPTree")
        .def(py::init<int, const std::string&>(), py::arg("t"), py::arg("filename"))
        .def("insert", &StringBPTree::insert, py::arg("key"))
        .def("search", &StringBPTree::search, py::arg("key"))
        .def("save", &StringBPTree::save)
        .def("load", &StringBPTree::load);
}
