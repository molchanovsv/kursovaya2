#include <QApplication>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QSpinBox>
#include "mainwindow.h"
#include "dataloader.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QDialog fileDialog;
    fileDialog.setWindowTitle("Выбор файлов");
    QFormLayout layout(&fileDialog);
    QLineEdit studEdit, concEdit;
    QSpinBox hashSizeSpin;
    hashSizeSpin.setMinimum(1);
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
    layout.addRow("Размер хэш-таблицы", &hashSizeSpin);
    QLabel errorLabel;
    errorLabel.setStyleSheet("color:red");
    layout.addRow(&errorLabel);
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

    auto validate = [&]() {
        QString msg;
        bool ok = true;
        std::string err;
        int studCount = 0;
        if (studEdit.text().isEmpty()) {
            msg = "Укажите файл студентов";
            ok = false;
        } else if (!DataLoader::validateStudentsFile(studEdit.text().toStdString(), studCount, err)) {
            msg = QString::fromStdString(err);
            ok = false;
        } else if (concEdit.text().isEmpty()) {
            msg = "Укажите файл концертов";
            ok = false;
        } else if (!DataLoader::validateConcertsFile(concEdit.text().toStdString(), err)) {
            msg = QString::fromStdString(err);
            ok = false;
        } else if (hashSizeSpin.value() < studCount) {
            msg = "Размер хэш-таблицы меньше количества записей";
            ok = false;
        }
        if (studCount > 0)
            hashSizeSpin.setMinimum(studCount);
        buttons.button(QDialogButtonBox::Ok)->setEnabled(ok);
        errorLabel.setText(msg);
    };
    QObject::connect(&studEdit, &QLineEdit::textChanged, validate);
    QObject::connect(&concEdit, &QLineEdit::textChanged, validate);
    validate();

    QObject::connect(&buttons, &QDialogButtonBox::accepted, &fileDialog, &QDialog::accept);
    QObject::connect(&buttons, &QDialogButtonBox::rejected, &fileDialog, &QDialog::reject);
    if (fileDialog.exec() != QDialog::Accepted)
        return 0;

    int count = 0;
    std::string err;
    DataLoader::validateStudentsFile(studEdit.text().toStdString(), count, err);
    Students_entry* records = DataLoader::loadStudents(count, count, studEdit.text().toStdString());
    auto *table = new HashTable(hashSizeSpin.value());
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
