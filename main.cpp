#include <QApplication>
#include "mainwindow.h"
#include "DataLoader.h"
#include <QFileDialog>
#include <QDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>

static bool selectDataFiles(QString& studentsPath, QString& concertsPath)
{
    QDialog dialog;
    dialog.setWindowTitle("Выбор файлов");
    QFormLayout form(&dialog);

    QLineEdit studEdit;
    QPushButton studBrowse("...");
    QWidget studRow;
    QHBoxLayout studLayout(&studRow);
    studLayout.addWidget(&studEdit);
    studLayout.addWidget(&studBrowse);
    form.addRow("Файл студентов", &studRow);

    QObject::connect(&studBrowse, &QPushButton::clicked, [&]() {
        QString f = QFileDialog::getOpenFileName(&dialog, "Выберите файл студентов", QString(), "Text Files (*.txt);;All Files (*)");
        if (!f.isEmpty())
            studEdit.setText(f);
    });

    QLineEdit concEdit;
    QPushButton concBrowse("...");
    QWidget concRow;
    QHBoxLayout concLayout(&concRow);
    concLayout.addWidget(&concEdit);
    concLayout.addWidget(&concBrowse);
    form.addRow("Файл концертов", &concRow);

    QObject::connect(&concBrowse, &QPushButton::clicked, [&]() {
        QString f = QFileDialog::getOpenFileName(&dialog, "Выберите файл концертов", QString(), "Text Files (*.txt);;All Files (*)");
        if (!f.isEmpty())
            concEdit.setText(f);
    });

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form.addRow(&buttons);
    QObject::connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        studentsPath = studEdit.text();
        concertsPath = concEdit.text();
        return !studentsPath.isEmpty() && !concertsPath.isEmpty();
    }
    return false;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QString studentsPath, concertsPath;
    if (!selectDataFiles(studentsPath, concertsPath))
        return 0;

    int count = 0;
    Students_entry* records = DataLoader::loadStudents(20, count, studentsPath.toStdString());
    auto *table = new HashTable(5);
    for(int i=0;i<count;++i)
        table->insert(records[i]);

    auto *tree = new AVLTree();
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
