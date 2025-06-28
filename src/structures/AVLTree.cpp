#include "structures/AVLTree.h"
#include <algorithm>
#include <iostream>
#include <vector>

AVLTree::Node::Node(const Concerts_entry& entry)
    : data(entry), left(nullptr), right(nullptr), height(1) {
    std::string fullName = entry.fio.surname + entry.fio.name + entry.fio.patronymic;
    key = 0;
    for (unsigned char c : fullName) {
        key += static_cast<int>(c);
    }
}

AVLTree::AVLTree() : root(nullptr) {}
AVLTree::~AVLTree() { clear(root); }

int AVLTree::height(Node* node) const {
    return node ? node->height : 0;
}

int AVLTree::balanceFactor(Node* node) const {
    return height(node->right) - height(node->left);
}

void AVLTree::updateHeight(Node* node) {
    int hl = height(node->left);
    int hr = height(node->right);
    node->height = (hl > hr ? hl : hr) + 1;
}

AVLTree::Node* AVLTree::rotateRight(Node* y) {
    Node* x = y->left;
    y->left = x->right;
    x->right = y;
    updateHeight(y);
    updateHeight(x);
    return x;
}

AVLTree::Node* AVLTree::rotateLeft(Node* x) {
    Node* y = x->right;
    x->right = y->left;
    y->left = x;
    updateHeight(x);
    updateHeight(y);
    return y;
}

AVLTree::Node* AVLTree::balance(Node* node) {
    updateHeight(node);
    if (balanceFactor(node) == 2) {
        if (balanceFactor(node->right) < 0)
            node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    if (balanceFactor(node) == -2) {
        if (balanceFactor(node->left) > 0)
            node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    return node;
}

AVLTree::Node* AVLTree::insert(Node* node, const Concerts_entry& entry) {
	if (!node) return new Node(entry);

	std::string fullNameCurrent = node->data.fio.surname + node->data.fio.name + node->data.fio.patronymic;
	std::string fullNameNew = entry.fio.surname + entry.fio.name + entry.fio.patronymic;

	if (fullNameNew < fullNameCurrent)
		node->left = insert(node->left, entry);
	else
		node->right = insert(node->right, entry);

	return balance(node);
}

AVLTree::Node* AVLTree::findMin(Node* node) const{
	return node->left ? findMin(node->left) : node;
}

AVLTree::Node* AVLTree::removeMin(Node* node) {
	if (!node->left) return node->right;
	node->left = removeMin(node->left);
	return balance(node);
}

AVLTree::Node* AVLTree::remove(Node* node, const FIO& fio) {
	if (!node) return nullptr;

	std::string fullNameCurrent = node->data.fio.surname + node->data.fio.name + node->data.fio.patronymic;
	std::string fullNameToRemove = fio.surname + fio.name + fio.patronymic;

	if (fullNameToRemove < fullNameCurrent)
		node->left = remove(node->left, fio);
	else if (fullNameToRemove > fullNameCurrent)
		node->right = remove(node->right, fio);
	else {
		Node* left = node->left;
		Node* right = node->right;
		delete node;

		if (!right) return left;

		Node* min = findMin(right);
		min->right = removeMin(right);
		min->left = left;
		return balance(min);
	}
	return balance(node);
}

void AVLTree::clear(Node* node) {
	if (node) {
		clear(node->left);
		clear(node->right);
		delete node;
	}
}

void AVLTree::insert(const Concerts_entry& entry) {
	root = insert(root, entry);
}

void AVLTree::remove(const FIO& fio) {
	root = remove(root, fio);
}

void AVLTree::print(Node* node, std::ostream& os, int level) const {
	if (node) {
		print(node->right, os, level + 1);
		for (int i = 0; i < level; i++) os << "    ";
		os << node->data.fio << std::endl;
		print(node->left, os, level + 1);
	}
}

void AVLTree::print(std::ostream& os) const {
        print(root, os);
        os << std::endl;
}

void AVLTree::toVector(Node* node, std::vector<Concerts_entry>& vec) const {
    if (!node) return;
    toVector(node->left, vec);
    vec.push_back(node->data);
    toVector(node->right, vec);
}

void AVLTree::toVector(std::vector<Concerts_entry>& vec) const {
    toVector(root, vec);
}

void AVLTree::searchHall(Node* node, const std::string& hall, std::vector<Concerts_entry>& vec) const {
    if (!node) return;
    searchHall(node->left, hall, vec);
    if (node->data.hall == hall)
        vec.push_back(node->data);
    searchHall(node->right, hall, vec);
}

std::vector<Concerts_entry> AVLTree::searchByHall(const std::string& hall) const {
    std::vector<Concerts_entry> res;
    searchHall(root, hall, res);
    return res;
}

void AVLTree::searchDate(Node* node, const std::string& date, std::vector<Concerts_entry>& vec) const {
    if (!node) return;
    searchDate(node->left, date, vec);
    if (node->data.date == date)
        vec.push_back(node->data);
    searchDate(node->right, date, vec);
}

std::vector<Concerts_entry> AVLTree::searchByDate(const std::string& date) const {
    std::vector<Concerts_entry> res;
    searchDate(root, date, res);
    return res;
}

void AVLTree::fillTreeWidget(Node* node, QTreeWidgetItem* parent, QTreeWidget* tree,
                             const Concerts_entry* highlight,
                             const QString& prefix) const {
    if (!node) return;
    QString base = QString::fromStdString(node->data.fio.surname + " " + node->data.fio.name + " " +
                                          node->data.fio.patronymic + " - " + node->data.play +
                                          " - " + node->data.hall + " - " + node->data.date);
    QString text = prefix.isEmpty() ? base : prefix + base;
    QTreeWidgetItem* item;
    if (parent)
        item = new QTreeWidgetItem(parent);
    else {
        item = new QTreeWidgetItem(tree);
        tree->addTopLevelItem(item);
    }
    item->setText(0, text);

    if (highlight &&
        highlight->fio.surname == node->data.fio.surname &&
        highlight->fio.name == node->data.fio.name &&
        highlight->fio.patronymic == node->data.fio.patronymic &&
        highlight->play == node->data.play &&
        highlight->hall == node->data.hall &&
        highlight->date == node->data.date) {
        item->setBackground(0, QBrush(Qt::magenta));
    }

    fillTreeWidget(node->left, item, tree, highlight, "L: ");
    fillTreeWidget(node->right, item, tree, highlight, "R: ");
}

void AVLTree::buildTreeWidget(QTreeWidget* widget,
                              const Concerts_entry* highlight) const {
    if (!widget)
        return;
    widget->clear();
    fillTreeWidget(root, nullptr, widget, highlight, "");
    widget->expandAll();
}

bool AVLTree::find(const FIO& fio, Concerts_entry& res, int& steps) const {
    Node* node = root;
    std::string target = fio.surname + fio.name + fio.patronymic;
    steps = 0;
    while (node) {
        ++steps;
        std::string cur = node->data.fio.surname + node->data.fio.name + node->data.fio.patronymic;
        if (target == cur) {
            res = node->data;
            return true;
        }
        if (target < cur)
            node = node->left;
        else
            node = node->right;
    }
    return false;
}

