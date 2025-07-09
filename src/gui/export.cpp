#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDate>
#include <fstream>
#include <array>
#include <QMessageBox>
#include <QTableWidgetItem>

void MainWindow::updateReport()
{
    QString instFilter = ui->instrumentFilterEdit->text();
    bool useInst = ui->instrumentFilterCheck->isChecked() && !instFilter.isEmpty();
    QString hallFilter = ui->hallFilterEdit->text();
    bool useHall = ui->hallFilterCheck->isChecked() && !hallFilter.isEmpty();
    QDate selDate = ui->dateEdit->date();
    bool useDate = ui->dateFilterCheck->isChecked();

    std::vector<Concerts_entry> concertsList;
    concerts->toVector(concertsList);
    std::vector<std::array<QString,8>> rows;
    for (const auto& c : concertsList) {
        Students_entry st;
        if (!students->find(c.fio, c.instrument, st))
            continue;

        QString inst = QString::fromStdString(st.instrument);
        QString hall = QString::fromStdString(c.hall);
        QString date = QString::fromStdString(c.date);

        if (useInst && !inst.contains(instFilter, Qt::CaseInsensitive))
            continue;
        if (useHall && !hall.contains(hallFilter, Qt::CaseInsensitive))
            continue;
        if (useDate) {
            QDate d = QDate::fromString(date, "dd.MM.yyyy");
            if (!d.isValid() || d != selDate)
                continue;
        }

        QString teacher = QString::fromStdString(st.teacher.surname + " " + st.teacher.initials);
        rows.push_back({QString::fromStdString(c.fio.surname),
                        QString::fromStdString(c.fio.name),
                        QString::fromStdString(c.fio.patronymic),
                        inst,
                        teacher,
                        QString::fromStdString(c.play),
                        hall,
                        date});
    }
    ui->reportTable->clear();
    ui->reportTable->setColumnCount(8);
    QStringList headers;
    headers << "Фамилия" << "Имя" << "Отчество" << "Инструмент" << "Учитель" << "Пьеса" << "Зал" << "Дата";
    ui->reportTable->setHorizontalHeaderLabels(headers);
    ui->reportTable->setRowCount(static_cast<int>(rows.size()));
    for (int i = 0; i < static_cast<int>(rows.size()); ++i) {
        for (int j = 0; j < 8; ++j) {
            QTableWidgetItem* item = new QTableWidgetItem(rows[i][j]);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            item->setTextAlignment(Qt::AlignCenter);
            ui->reportTable->setItem(i, j, item);
        }
    }
}

void MainWindow::exportStudents()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Выгрузить студентов",
                                                   studentFile,
                                                   "Text files (*.txt)");
    if (fileName.isEmpty())
        return;
    studentFile = fileName;
    std::ofstream out(fileName.toStdString());
    if (!out.is_open()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
        return;
    }
    for (int i = 0; i < students->getFullSize(); ++i) {
        if (students->isOccupied(i)) {
            const auto& st = students->getEntry(i);
            out << st.fio.surname << ' ' << st.fio.name << ' ' << st.fio.patronymic
                << ' ' << st.instrument << ' ' << st.teacher.surname << ' '
                << st.teacher.initials << '\n';
        }
    }
}

void MainWindow::exportConcerts()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Выгрузить концерты",
                                                   concertFile,
                                                   "Text files (*.txt)");
    if (fileName.isEmpty())
        return;
    concertFile = fileName;
    std::ofstream out(fileName.toStdString());
    if (!out.is_open()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
        return;
    }
    std::vector<Concerts_entry> vec;
    concerts->toVector(vec);
    for (const auto& c : vec) {
        out << c.fio.surname << ' ' << c.fio.name << ' ' << c.fio.patronymic << ' ' << c.instrument
            << " \"" << c.play << "\" " << c.hall << ' ' << c.date << '\n';
    }
}

void MainWindow::exportReport()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить отчёт", QString(),
                                                   "Text files (*.txt)");
    if (fileName.isEmpty())
        return;
    std::ofstream out(fileName.toStdString());
    if (!out.is_open()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
        return;
    }
    QString instFilter = ui->instrumentFilterEdit->text();
    bool useInst = ui->instrumentFilterCheck->isChecked() && !instFilter.isEmpty();
    QString hallFilter = ui->hallFilterEdit->text();
    bool useHall = ui->hallFilterCheck->isChecked() && !hallFilter.isEmpty();
    QDate selDate = ui->dateEdit->date();
    bool useDate = ui->dateFilterCheck->isChecked();

    std::vector<Concerts_entry> concertsList;
    concerts->toVector(concertsList);
    for (const auto& c : concertsList) {
        Students_entry st;
        if (!students->find(c.fio, c.instrument, st))
            continue;

        QString inst = QString::fromStdString(st.instrument);
        QString hall = QString::fromStdString(c.hall);
        QString date = QString::fromStdString(c.date);

        if (useInst && !inst.contains(instFilter, Qt::CaseInsensitive))
            continue;
        if (useHall && !hall.contains(hallFilter, Qt::CaseInsensitive))
            continue;
        if (useDate) {
            QDate d = QDate::fromString(date, "dd.MM.yyyy");
            if (!d.isValid() || d != selDate)
                continue;
        }

        out << c.fio.surname << ' ' << c.fio.name << ' ' << c.fio.patronymic << ' '
            << st.instrument << ' ' << st.teacher.surname << ' ' << st.teacher.initials
            << " \"" << c.play << "\" " << c.hall << ' ' << c.date << '\n';
    }
}

