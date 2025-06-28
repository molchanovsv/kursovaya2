#include <QApplication>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QDialogButtonBox>
#include "mainwindow.h"
#include "DataLoader.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QDialog fileDialog;
    fileDialog.setWindowTitle("Выбор файлов");
    QFormLayout layout(&fileDialog);
    QLineEdit studEdit, concEdit;
    QPushButton browseStud("...");
    QPushButton browseConc("...");
    QWidget studRow, concRow;
    QHBoxLayout studLay, concLay;
    studLay.addWidget(&studEdit);
    studLay.addWidget(&browseStud);
    studRow.setLayout(&studLay);
    concLay.addWidget(&concEdit);
    concLay.addWidget(&browseConc);
    concRow.setLayout(&concLay);
    layout.addRow("Студенты", &studRow);
    layout.addRow("Концерты", &concRow);
    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout.addRow(&buttons);
    QObject::connect(&browseStud, &QPushButton::clicked, [&]() {
        QString f = QFileDialog::getOpenFileName(&fileDialog, "Файл студентов", "", "Text files (*.txt)");
        if (!f.isEmpty()) studEdit.setText(f);
    });
    QObject::connect(&browseConc, &QPushButton::clicked, [&]() {
        QString f = QFileDialog::getOpenFileName(&fileDialog, "Файл концертов", "", "Text files (*.txt)");
        if (!f.isEmpty()) concEdit.setText(f);
    });
    QObject::connect(&buttons, &QDialogButtonBox::accepted, &fileDialog, &QDialog::accept);
    QObject::connect(&buttons, &QDialogButtonBox::rejected, &fileDialog, &QDialog::reject);
    if (fileDialog.exec() != QDialog::Accepted)
        return 0;

    int count = 0;
    Students_entry* records = DataLoader::loadStudents(20, count, studEdit.text().toStdString());
    auto *table = new HashTable(5);
    for(int i=0;i<count;++i)
        table->insert(records[i]);

    auto *tree = new AVLTree();
    auto concerts = DataLoader::loadConcertsData(concEdit.text().toStdString().empty() ? "concerts.txt" : concEdit.text().toStdString());
    for(const auto& c : concerts)
        tree->insert(c);

    MainWindow w(table, tree, studEdit.text(), concEdit.text());
    w.show();
    int res = app.exec();

    delete[] records;
    delete table;
    delete tree;
    return res;
}
