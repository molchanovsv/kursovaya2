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
#include "gui/theme.h"
#include <QIcon>
#include <QSettings>
#include <memory>
#include <limits>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("База Данных Музыкальной Школы");
    app.setWindowIcon(QIcon(":/app.ico"));

    QCoreApplication::setOrganizationName("MusicSchool");
    QSettings settings;
    Theme theme = themeFromString(settings.value("theme", "dark").toString());
    applyTheme(theme, app);

    QDialog fileDialog;
    fileDialog.setWindowTitle("Выбор файлов");
    QFormLayout layout(&fileDialog);
    QLineEdit studEdit, concEdit;
    studEdit.setText(settings.value("lastStudentFile").toString());
    concEdit.setText(settings.value("lastConcertFile").toString());
    QSpinBox hashSizeSpin;
    hashSizeSpin.setMinimum(1);
    hashSizeSpin.setMaximum(std::numeric_limits<int>::max());
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
        int lineStu = 0;
        int lineConc = 0;

        if (studEdit.text().isEmpty()) {
            msg = "Укажите файл студентов";
            ok = false;
        } else if (!DataLoader::validateStudentsFile(studEdit.text().toStdString(), studCount, err, lineStu)) {
            if (lineStu > 0)
                msg = QString("Студенты: строка %1: %2").arg(lineStu).arg(QString::fromStdString(err));
            else
                msg = QString::fromStdString(err);
            ok = false;
        } else if (concEdit.text().isEmpty()) {
            msg = "Укажите файл концертов";
            ok = false;
        } else if (!DataLoader::validateConcertsFile(concEdit.text().toStdString(), err, lineConc)) {
            if (lineConc > 0)
                msg = QString("Концерты: строка %1: %2").arg(lineConc).arg(QString::fromStdString(err));
            else
                msg = QString::fromStdString(err);
            ok = false;
        }

        if (studCount > 0) {
            hashSizeSpin.setMinimum(studCount);
            if (hashSizeSpin.value() < studCount)
                hashSizeSpin.setValue(studCount);
        }

        if (ok && hashSizeSpin.value() < studCount) {
            msg = "Размер хэш-таблицы меньше количества записей";
            ok = false;
        }

        buttons.button(QDialogButtonBox::Ok)->setEnabled(ok);
        errorLabel.setText(msg);
    };
    QObject::connect(&studEdit, &QLineEdit::textChanged, validate);
    QObject::connect(&concEdit, &QLineEdit::textChanged, validate);
    QObject::connect(&hashSizeSpin, qOverload<int>(&QSpinBox::valueChanged), validate);
    validate();

    QObject::connect(&buttons, &QDialogButtonBox::accepted, &fileDialog, &QDialog::accept);
    QObject::connect(&buttons, &QDialogButtonBox::rejected, &fileDialog, &QDialog::reject);
    if (fileDialog.exec() != QDialog::Accepted)
        return 0;

    settings.setValue("lastStudentFile", studEdit.text());
    settings.setValue("lastConcertFile", concEdit.text());

    int count = 0;
    std::string err;
    int dummyLine = 0;
    DataLoader::validateStudentsFile(studEdit.text().toStdString(), count, err, dummyLine);
    auto records = DataLoader::loadStudents(count, count, studEdit.text().toStdString());
    auto table = std::make_unique<HashTable>(hashSizeSpin.value());
    for(int i=0;i<count && i < static_cast<int>(records.size()); ++i)
        table->insert(records[i]);

    auto tree = std::make_unique<AVLTree>();
    auto concerts = DataLoader::loadConcertsData(concEdit.text().toStdString().empty() ? "concerts.txt" : concEdit.text().toStdString());
    for(const auto& c : concerts)
        tree->insert(c);

    MainWindow w(table.get(), tree.get(), records, studEdit.text(), concEdit.text());
    w.setWindowTitle("База Данных Музыкальной Школы");
    w.setWindowIcon(QIcon(":/app.ico"));
    w.show();
    int res = app.exec();

    return res;
}
