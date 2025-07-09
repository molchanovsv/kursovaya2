#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDialog>
#include <QFormLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QMenu>
#include <QLineEdit>
#include <QTableWidgetItem>


void MainWindow::addStudent()
{
    Students_entry se;
    if (studentDialog(se)) {
        if (!students->insert(se)) {
            QMessageBox::warning(this, "Добавить ученика", "Такая запись уже существует");
        } else {
            refreshTables();
        }
    }
}

void MainWindow::removeStudent()
{
    FIO f;
    std::string instr;
    if (!fioInstrumentDialog(f, instr, nullptr, QString(), "Удалить ученика"))
        return;
    students->remove(f, instr);
    refreshTables();
}

void MainWindow::editStudent()
{
    int row = ui->studentsTable->currentRow();
    if (row < 0 || row >= static_cast<int>(studentRowMap.size())) {
        QMessageBox::warning(this, "Редактировать ученика", "Выберите запись в таблице.");
        return;
    }

    int index = studentRowMap[row];
    if (index < 0 || !students->isOccupied(index)) {
        QMessageBox::warning(this, "Редактировать ученика", "Выберите запись в таблице.");
        return;
    }
    Students_entry oldEntry = students->getEntry(index);
    Students_entry newEntry = oldEntry;
    if (!studentDialog(newEntry, &oldEntry))
        return;

    students->remove(oldEntry.fio, oldEntry.instrument);
    if (!students->insert(newEntry)) {
        QMessageBox::warning(this, "Редактировать ученика", "Такая запись уже существует");
        students->insert(oldEntry);
    }
    refreshTables();
}

void MainWindow::searchStudent()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Фильтр учеников");
    QFormLayout layout(&dialog);

    QLineEdit sur, nam, pat, instr, teacher;
    sur.setText(sSurname);
    nam.setText(sName);
    pat.setText(sPatronymic);
    instr.setText(sInstr);
    teacher.setText(sTeacher);

    layout.addRow("Фамилия", &sur);
    layout.addRow("Имя", &nam);
    layout.addRow("Отчество", &pat);
    layout.addRow("Инструмент", &instr);
    layout.addRow("Учитель", &teacher);

    QPushButton clearBtn("Очистить");
    layout.addRow(&clearBtn);
    connect(&clearBtn, &QPushButton::clicked, [&]() {
        sur.clear();
        nam.clear();
        pat.clear();
        instr.clear();
        teacher.clear();
    });

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout.addRow(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        sSurname = sur.text();
        sName = nam.text();
        sPatronymic = pat.text();
        sInstr = instr.text();
        sTeacher = teacher.text();
        if (!sSurname.isEmpty() && !sName.isEmpty() && !sPatronymic.isEmpty()) {
            FIO f{ sSurname.toStdString(), sName.toStdString(), sPatronymic.toStdString() };
            Students_entry tmp;
            int steps = 0;
            bool found = students->find(f, sInstr.toStdString(), tmp, steps);
            ui->hashStepsLabel->setText(found
                ? QString("Хэш поиск: %1 шагов").arg(steps)
                : QString("Хэш поиск: не найдено (%1 шагов)").arg(steps));
        } else {
            ui->hashStepsLabel->clear();
        }
        studentFilterActive = !(sSurname.isEmpty() && sName.isEmpty() &&
                               sPatronymic.isEmpty() && sInstr.isEmpty() &&
                               sTeacher.isEmpty());
        ui->clearStudentSearchButton->setVisible(studentFilterActive);
        refreshTables();
    }
}


void MainWindow::clearStudentSearch()
{
    sSurname.clear();
    sName.clear();
    sPatronymic.clear();
    sInstr.clear();
    sTeacher.clear();
    studentFilterActive = false;
    ui->clearStudentSearchButton->setVisible(false);
    ui->hashStepsLabel->clear();
    refreshTables();
}


void MainWindow::studentCellChanged(int row, int column)
{
    if (row < 0 || row >= static_cast<int>(studentRowMap.size()))
        return;
    int index = studentRowMap[row];
    if (index < 0 || !students->isOccupied(index))
        return;
    Students_entry oldEntry = students->getEntry(index);
    Students_entry newEntry = oldEntry;

    auto getText = [&](int col, const std::string& oldVal) {
        QTableWidgetItem* item = ui->studentsTable->item(row, col);
        return item ? item->text().toStdString() : oldVal;
    };

    newEntry.fio.surname = getText(1, oldEntry.fio.surname);
    newEntry.fio.name = getText(2, oldEntry.fio.name);
    newEntry.fio.patronymic = getText(3, oldEntry.fio.patronymic);
    newEntry.instrument = getText(4, oldEntry.instrument);
    std::string teacher = getText(5, oldEntry.teacher.surname + " " + oldEntry.teacher.initials);
    size_t sp = teacher.find(' ');
    if (sp == std::string::npos) {
        newEntry.teacher.surname = teacher;
        newEntry.teacher.initials.clear();
    } else {
        newEntry.teacher.surname = teacher.substr(0, sp);
        newEntry.teacher.initials = teacher.substr(sp + 1);
    }

    if (newEntry.fio == oldEntry.fio && newEntry.instrument == oldEntry.instrument &&
        newEntry.teacher.surname == oldEntry.teacher.surname && newEntry.teacher.initials == oldEntry.teacher.initials)
        return;

    QStringList errors;
    if (!validateFIO(QString::fromStdString(newEntry.fio.surname),
                     QString::fromStdString(newEntry.fio.name),
                     QString::fromStdString(newEntry.fio.patronymic)))
        errors << "FIO";
    if (!validateInstrument(QString::fromStdString(newEntry.instrument)))
        errors << "instrument";
    if (!validateTeacher(QString::fromStdString(newEntry.teacher.surname),
                         QString::fromStdString(newEntry.teacher.initials)))
        errors << "teacher initials";

    if (!errors.isEmpty()) {
        QMessageBox::warning(this, "Input Error",
                             QString("Invalid: %1").arg(errors.join(", ")));
        refreshTables();
        return;
    }

    students->remove(oldEntry.fio, oldEntry.instrument);
    if (!students->insert(newEntry)) {
        QMessageBox::warning(this, "Редактировать ученика", "Такая запись уже существует");
        students->insert(oldEntry);
    }
    refreshTables();
    ui->studentsTable->setCurrentCell(row, column);
}

void MainWindow::studentContextMenu(const QPoint& pos)
{
    int row = ui->studentsTable->rowAt(pos.y());
    if (row < 0 || row >= static_cast<int>(studentRowMap.size()))
        return;
    if (studentRowMap[row] < 0 || !students->isOccupied(studentRowMap[row]))
        return;
    ui->studentsTable->setCurrentCell(row, 1);
    QMenu menu(this);
    QAction* edit = menu.addAction("Изменить");
    QAction* remove = menu.addAction("Удалить");
    QAction* chosen = menu.exec(ui->studentsTable->viewport()->mapToGlobal(pos));
    if (chosen == edit) {
        editStudent();
    } else if (chosen == remove) {
        Students_entry entry = students->getEntry(studentRowMap[row]);
        students->remove(entry.fio, entry.instrument);
        refreshTables();
    }
}
