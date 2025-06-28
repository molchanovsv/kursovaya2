#ifndef AVLTREE_H
#define AVLTREE_H

#include "Concerts_entry.h"
#include <ostream>

class AVLTree {
private:
    struct Node {
        Concerts_entry data;
        Node* left;
        Node* right;
        int height;
        int key;

        Node(const Concerts_entry& entry);
    };

    Node* root;

    int height(Node* node) const;
    int balanceFactor(Node* node) const;
    void updateHeight(Node* node);
    Node* rotateRight(Node* y);
    Node* rotateLeft(Node* x);
    Node* balance(Node* node);
    Node* insert(Node* node, const Concerts_entry& entry);
    Node* findMin(Node* node) const;
    Node* removeMin(Node* node);
    Node* remove(Node* node, const FIO& fio);
    void print(Node* node, std::ostream& os, int level = 0) const;
    void clear(Node* node);

public:
    AVLTree();
    ~AVLTree();

    void insert(const Concerts_entry& entry);
    void remove(const FIO& fio);
    void print(std::ostream& os) const;
};

#endif