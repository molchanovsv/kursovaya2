#ifndef AVLTREE_H
#define AVLTREE_H

#include "Concerts_entry.h"
#include <ostream>
#include <vector>
#include <QTreeWidget>

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
    void toVector(Node* node, std::vector<Concerts_entry>& vec) const;
    void searchHall(Node* node, const std::string& hall, std::vector<Concerts_entry>& vec) const;
    void searchDate(Node* node, const std::string& date, std::vector<Concerts_entry>& vec) const;
    void fillTreeWidget(Node* node, QTreeWidgetItem* item, QTreeWidget* tree,
                        const FIO* highlight, const QString& prefix) const;

public:
    AVLTree();
    ~AVLTree();

    void insert(const Concerts_entry& entry);
    void remove(const FIO& fio);
    void print(std::ostream& os) const;
    void toVector(std::vector<Concerts_entry>& vec) const;
    std::vector<Concerts_entry> searchByHall(const std::string& hall) const;
    std::vector<Concerts_entry> searchByDate(const std::string& date) const;
    void buildTreeWidget(QTreeWidget* widget, const FIO* highlight = nullptr) const;
};

#endif
