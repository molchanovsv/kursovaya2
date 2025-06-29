#include "mainwindow.h"
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QMessageBox>
#include "validators.h"



bool MainWindow::studentDialog(Students_entry& out, const Students_entry* initial)
{
    QDialog dialog(this);
    dialog.setWindowTitle(initial ? "Редактировать ученика" : "Добавить ученика");
    QFormLayout layout(&dialog);

    QLabel currentLabel;
    if (initial) {
        QString cur = QString::fromStdString(initial->fio.surname + " " + initial->fio.name + " " +
                                            initial->fio.patronymic + " - " + initial->instrument +
                                            " (" + initial->teacher.surname + " " + initial->teacher.initials + ")");
        currentLabel.setText(cur);
        layout.addRow(&currentLabel);
    }

    QLineEdit surname, name, patronymic, instrument, teacherSurname, teacherInitials;
    if (initial) {
        surname.setText(QString::fromStdString(initial->fio.surname));
        name.setText(QString::fromStdString(initial->fio.name));
        patronymic.setText(QString::fromStdString(initial->fio.patronymic));
        instrument.setText(QString::fromStdString(initial->instrument));
        teacherSurname.setText(QString::fromStdString(initial->teacher.surname));
        teacherInitials.setText(QString::fromStdString(initial->teacher.initials));
    }

    layout.addRow("Фамилия", &surname);
    layout.addRow("Имя", &name);
    layout.addRow("Отчество", &patronymic);
    layout.addRow("Инструмент", &instrument);
    layout.addRow("Фамилия учителя", &teacherSurname);
    layout.addRow("Инициалы учителя", &teacherInitials);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout.addRow(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QStringList errors;
        if (!validateFIO(surname.text(), name.text(), patronymic.text()))
            errors << "FIO";
        if (!validateInstrument(instrument.text()))
            errors << "instrument";
        if (!validateTeacher(teacherSurname.text(), teacherInitials.text()))
            errors << "teacher initials";

        if (!errors.isEmpty()) {
            QMessageBox::warning(this, "Input Error",
                                 QString("Invalid: %1").arg(errors.join(", ")));
            return false;
        }

        out.fio.surname = surname.text().toStdString();
        out.fio.name = name.text().toStdString();
        out.fio.patronymic = patronymic.text().toStdString();
        out.instrument = instrument.text().toStdString();
        out.teacher.surname = teacherSurname.text().toStdString();
        out.teacher.initials = teacherInitials.text().toStdString();
        return true;
    }
    return false;
}

bool MainWindow::concertDialog(Concerts_entry& out, const Concerts_entry* initial)
{
    QDialog dialog(this);
    dialog.setWindowTitle(initial ? "Редактировать концерт" : "Добавить концерт");
    QFormLayout layout(&dialog);

    QLabel currentLabel;
    if (initial) {
        QString cur = QString::fromStdString(initial->fio.surname + " " + initial->fio.name + " " +
                                            initial->fio.patronymic + " - " + initial->play + " - " +
                                            initial->hall + " - " + initial->date);
        currentLabel.setText(cur);
        layout.addRow(&currentLabel);
    }

    QLineEdit surname, name, patronymic, play, hall, date;
    if (initial) {
        surname.setText(QString::fromStdString(initial->fio.surname));
        name.setText(QString::fromStdString(initial->fio.name));
        patronymic.setText(QString::fromStdString(initial->fio.patronymic));
        play.setText(QString::fromStdString(initial->play));
        hall.setText(QString::fromStdString(initial->hall));
        date.setText(QString::fromStdString(initial->date));
    }

    layout.addRow("Фамилия", &surname);
    layout.addRow("Имя", &name);
    layout.addRow("Отчество", &patronymic);
    layout.addRow("Пьеса", &play);
    layout.addRow("Зал", &hall);
    layout.addRow("Дата", &date);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout.addRow(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QStringList errors;
        if (!validateFIO(surname.text(), name.text(), patronymic.text()))
            errors << "FIO";
        if (!validateHall(hall.text()))
            errors << "hall";
        if (!validateDate(date.text()))
            errors << "date";

        if (!errors.isEmpty()) {
            QMessageBox::warning(this, "Input Error",
                                 QString("Invalid: %1").arg(errors.join(", ")));
            return false;
        }

        out.fio.surname = surname.text().toStdString();
        out.fio.name = name.text().toStdString();
        out.fio.patronymic = patronymic.text().toStdString();
        out.play = play.text().toStdString();
        out.hall = hall.text().toStdString();
        out.date = date.text().toStdString();
        return true;
    }
    return false;
}

bool MainWindow::fioDialog(FIO& out, const FIO* initial, const QString& title)
{
    QDialog dialog(this);
    dialog.setWindowTitle(title);
    QFormLayout layout(&dialog);

    QLineEdit surname, name, patronymic;
    if (initial) {
        surname.setText(QString::fromStdString(initial->surname));
        name.setText(QString::fromStdString(initial->name));
        patronymic.setText(QString::fromStdString(initial->patronymic));
    }

    layout.addRow("Фамилия", &surname);
    layout.addRow("Имя", &name);
    layout.addRow("Отчество", &patronymic);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout.addRow(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QStringList errors;
        if (!validateWord(surname.text()))
            errors << "surname";
        if (!validateWord(name.text()))
            errors << "name";
        if (!validateWord(patronymic.text()))
            errors << "patronymic";

        if (!errors.isEmpty()) {
            QMessageBox::warning(this, "Input Error",
                                 QString("Invalid: %1").arg(errors.join(", ")));
            return false;
        }

        out.surname = surname.text().toStdString();
        out.name = name.text().toStdString();
        out.patronymic = patronymic.text().toStdString();
        return true;
    }
    return false;
}
bool MainWindow::validateWord(const QString& word) const
{
    return wordValid(word);
}

bool MainWindow::validateFIO(const QString& s, const QString& n, const QString& p) const
{
    return fioValid(s, n, p);
}

bool MainWindow::validateInstrument(const QString& instrument) const
{
    return instrumentValid(instrument);
}

bool MainWindow::validateTeacher(const QString& surname, const QString& initials) const
{
    return teacherValid(surname, initials);
}


bool MainWindow::validateHall(const QString& hall) const
{
    return hallValid(hall);
}

bool MainWindow::validateDate(const QString& date) const
{
    return dateValid(date);
}
