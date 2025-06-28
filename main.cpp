#include <QApplication>
#include "mainwindow.h"
#include "DataLoader.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    int count = 0;
    Students_entry* records = DataLoader::loadStudents(20, count);
    auto *table = new HashTable(5);
    for(int i=0;i<count;++i)
        table->insert(records[i]);

    auto *tree = new AVLTree();
    auto concerts = DataLoader::loadConcertsData("concerts.txt");
    for(const auto& c : concerts)
        tree->insert(c);

    MainWindow w(table, tree);
    w.show();
    int res = app.exec();

    delete[] records;
    delete table;
    delete tree;
    return res;
}
