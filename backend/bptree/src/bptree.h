#ifndef BPTREE_H
#define BPTREE_H

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

struct FixedString {
    static const size_t MAX_LENGTH = 64;
    char data[MAX_LENGTH];

    FixedString() {
        std::memset(data, 0, MAX_LENGTH);
    }

    FixedString(const std::string& str) {
        std::memset(data, 0, MAX_LENGTH);
        std::strncpy(data, str.c_str(), MAX_LENGTH - 1);
    }

    bool operator<(const FixedString& other) const {
        return std::strcmp(data, other.data) < 0;
    }

    bool operator>(const FixedString& other) const {
        return std::strcmp(data, other.data) > 0;
    }

    bool operator==(const FixedString& other) const {
        return std::strcmp(data, other.data) == 0;
    }
};

template<typename KeyType>
class BPTreeImpl {
public:
    BPTreeImpl(int t, const std::string& filename);
    ~BPTreeImpl();

    void insert(const KeyType& key);
    bool search(const KeyType& key);
    void save();
    void load();

private:
    struct Node {
        std::vector<KeyType> keys;
        std::vector<Node*> children;
        bool isLeaf;

        Node(bool leaf);
        ~Node();
    };

    Node* root;
    int t; // Minimum degree
    std::string filename;

    void insertNonFull(Node* node, const KeyType& key);
    void splitChild(Node* parent, int i, Node* child);
    bool search(Node* node, const KeyType& key);

    void saveNode(std::ofstream& out, Node* node);
    Node* loadNode(std::ifstream& in);
    void clear(Node* node);
};

// BPTree of specific types
class IntBPTree {
public:
    IntBPTree(int t, const std::string& filename);
    void insert(int key);
    bool search(int key);
    void save();
    void load();
private:
    BPTreeImpl<int> impl;
};

class StringBPTree {
public:
    StringBPTree(int t, const std::string& filename);
    void insert(const std::string& key);
    bool search(const std::string& key);
    void save();
    void load();
private:
    BPTreeImpl<FixedString> impl;
};

#endif // BPTREE_H
