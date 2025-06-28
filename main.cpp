#include <QApplication>
#include "mainwindow.h"
#include "DataLoader.h"
#include <QFileDialog>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QString studentsPath = QFileDialog::getOpenFileName(nullptr,
        "Выберите файл студентов", QString(), "Text Files (*.txt);;All Files (*)");
    if (studentsPath.isEmpty())
        return 0;
    int count = 0;
    Students_entry* records = DataLoader::loadStudents(20, count, studentsPath.toStdString());
    auto *table = new HashTable(5);
    for(int i=0;i<count;++i)
        table->insert(records[i]);

    auto *tree = new AVLTree();
    QString concertsPath = QFileDialog::getOpenFileName(nullptr,
        "Выберите файл концертов", QString(), "Text Files (*.txt);;All Files (*)");
    if (concertsPath.isEmpty()) {
        delete[] records;
        return 0;
    }
    auto concerts = DataLoader::loadConcertsData(concertsPath.toStdString());
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
