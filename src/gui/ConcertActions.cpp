#include "ConcertActions.h"
#include "ui_mainwindow.h"
#include <QDialog>
#include <QFormLayout>
#include <QDateEdit>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QMenu>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidgetItem>


void MainWindow::addConcert()
{
    Concerts_entry e;
    if (concertDialog(e)) {
        concerts->insert(e);
        refreshTables();
    }
}

void MainWindow::removeConcert()
{
    FIO f;
    if (!fioDialog(f, nullptr, "Удалить концерт"))
        return;
    concerts->remove(f);
    refreshTables();
}

void MainWindow::editConcert()
{
    int row = ui->concertsTable->currentRow();
    if (row < 0 || row >= static_cast<int>(concertList.size())) {
        QMessageBox::warning(this, "Редактировать концерт", "Выберите запись в таблице.");
        return;
    }

    Concerts_entry oldEntry = concertList[row];
    Concerts_entry newEntry = oldEntry;
    if (!concertDialog(newEntry, &oldEntry))
        return;

    concerts->remove(oldEntry.fio);
    concerts->insert(newEntry);
    refreshTables();
}
void MainWindow::searchConcert()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Фильтр концертов");
    QFormLayout layout(&dialog);

    QLineEdit sur, nam, pat, play, hall;
    QDateEdit date;
    QCheckBox dateCheck("Искать по дате?");
    date.setDisplayFormat("dd.MM.yyyy");
    date.setCalendarPopup(true);
    sur.setText(cSurname);
    nam.setText(cName);
    pat.setText(cPatronymic);
    play.setText(cPlay);
    hall.setText(cHall);
    if (!cDate.isEmpty())
        date.setDate(QDate::fromString(cDate, "dd.MM.yyyy"));
    dateCheck.setChecked(cDateEnabled);

    layout.addRow("Фамилия", &sur);
    layout.addRow("Имя", &nam);
    layout.addRow("Отчество", &pat);
    layout.addRow("Пьеса", &play);
    layout.addRow("Зал", &hall);
    layout.addRow(&dateCheck);
    layout.addRow("Дата", &date);

    QPushButton clearBtn("Очистить");
    layout.addRow(&clearBtn);
    connect(&clearBtn, &QPushButton::clicked, [&]() {
        sur.clear();
        nam.clear();
        pat.clear();
        play.clear();
        hall.clear();
        date.setDate(QDate());
        dateCheck.setChecked(false);
    });

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout.addRow(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        cSurname = sur.text();
        cName = nam.text();
        cPatronymic = pat.text();
        cPlay = play.text();
        cHall = hall.text();
        cDate = date.date().isValid() ? date.date().toString("dd.MM.yyyy") : QString();
        cDateEnabled = dateCheck.isChecked();
        if (!cSurname.isEmpty() && !cName.isEmpty() && !cPatronymic.isEmpty()) {
            FIO f{ cSurname.toStdString(), cName.toStdString(), cPatronymic.toStdString() };
            Concerts_entry tmp;
            int steps = 0;
            bool found = concerts->find(f, tmp, steps);
            ui->treeStepsLabel->setText(found
                ? QString("Дерево поиск: %1 шагов").arg(steps)
                : QString("Дерево поиск: не найдено (%1 шагов)").arg(steps));
        } else {
            ui->treeStepsLabel->clear();
        }
        concertFilterActive = !(cSurname.isEmpty() && cName.isEmpty() && cPatronymic.isEmpty() &&
                               cPlay.isEmpty() && cHall.isEmpty() && !(cDateEnabled && !cDate.isEmpty()));
        ui->clearConcertSearchButton->setVisible(concertFilterActive);
        refreshTables();
    }
}
void MainWindow::clearConcertSearch()
{
    cSurname.clear();
    cName.clear();
    cPatronymic.clear();
    cPlay.clear();
    cHall.clear();
    cDate.clear();
    cDateEnabled = false;
    concertFilterActive = false;
    ui->clearConcertSearchButton->setVisible(false);
    ui->treeStepsLabel->clear();
    refreshTables();
}
void MainWindow::updateConcertTree()
{
    int row = ui->concertsTable->currentRow();
    Concerts_entry* highlight = nullptr;
    Concerts_entry temp;
    if (row >= 0 && row < static_cast<int>(concertList.size())) {
        temp = concertList[row];
        highlight = &temp;
    }

    concerts->buildTreeWidget(ui->concertTree, highlight);
}

void MainWindow::concertCellChanged(int row, int column)
{
    if (row < 0 || row >= static_cast<int>(concertList.size()))
        return;

    Concerts_entry oldEntry = concertList[row];
    Concerts_entry newEntry = oldEntry;

    auto getText = [&](int col, const std::string& oldVal) {
        QTableWidgetItem* item = ui->concertsTable->item(row, col);
        return item ? item->text().toStdString() : oldVal;
    };

    newEntry.fio.surname = getText(0, oldEntry.fio.surname);
    newEntry.fio.name = getText(1, oldEntry.fio.name);
    newEntry.fio.patronymic = getText(2, oldEntry.fio.patronymic);
    newEntry.play = getText(3, oldEntry.play);
    newEntry.hall = getText(4, oldEntry.hall);
    newEntry.date = getText(5, oldEntry.date);

    if (newEntry.fio == oldEntry.fio && newEntry.play == oldEntry.play &&
        newEntry.hall == oldEntry.hall && newEntry.date == oldEntry.date)
        return;

    QStringList errors;
    if (!validateFIO(QString::fromStdString(newEntry.fio.surname),
                     QString::fromStdString(newEntry.fio.name),
                     QString::fromStdString(newEntry.fio.patronymic)))
        errors << "FIO";
    if (!validateHall(QString::fromStdString(newEntry.hall)))
        errors << "hall";
    if (!validateDate(QString::fromStdString(newEntry.date)))
        errors << "date";

    if (!errors.isEmpty()) {
        QMessageBox::warning(this, "Input Error",
                             QString("Invalid: %1").arg(errors.join(", ")));
        refreshTables();
        return;
    }

    concerts->remove(oldEntry.fio);
    concerts->insert(newEntry);
    refreshTables();
    ui->concertsTable->setCurrentCell(row, column);
}
void MainWindow::concertContextMenu(const QPoint& pos)
{
    int row = ui->concertsTable->rowAt(pos.y());
    if (row < 0 || row >= static_cast<int>(concertList.size()))
        return;
    ui->concertsTable->setCurrentCell(row, 0);
    QMenu menu(this);
    QAction* edit = menu.addAction("Изменить");
    QAction* remove = menu.addAction("Удалить");
    QAction* chosen = menu.exec(ui->concertsTable->viewport()->mapToGlobal(pos));
    if (chosen == edit) {
        editConcert();
    } else if (chosen == remove) {
        concerts->remove(concertList[row].fio);
        refreshTables();
    }
}
