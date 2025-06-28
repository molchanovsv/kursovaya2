#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QTreeWidget>
#include <QBrush>
#include <QRegularExpression>
#include <vector>
#include <sstream>
#include "FIO.h"

MainWindow::MainWindow(HashTable* studentsTable, AVLTree* concertTree, QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), students(studentsTable), concerts(concertTree)
{
    ui->setupUi(this);
    connect(ui->addStudentButton, &QPushButton::clicked, this, &MainWindow::addStudent);
    connect(ui->removeStudentButton, &QPushButton::clicked, this, &MainWindow::removeStudent);
    connect(ui->editStudentButton, &QPushButton::clicked, this, &MainWindow::editStudent);
    connect(ui->addConcertButton, &QPushButton::clicked, this, &MainWindow::addConcert);
    connect(ui->removeConcertButton, &QPushButton::clicked, this, &MainWindow::removeConcert);
    connect(ui->editConcertButton, &QPushButton::clicked, this, &MainWindow::editConcert);
    connect(ui->searchStudentButton, &QPushButton::clicked, this, &MainWindow::searchStudent);
    connect(ui->searchConcertButton, &QPushButton::clicked, this, &MainWindow::searchConcert);
    connect(ui->concertsTable, &QTableWidget::itemSelectionChanged, this, &MainWindow::updateConcertTree);
    connect(ui->studentsTable, &QTableWidget::cellChanged, this, &MainWindow::studentCellChanged);
    connect(ui->concertsTable, &QTableWidget::cellChanged, this, &MainWindow::concertCellChanged);
    refreshTables();
    updateConcertTree();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::refreshTables()
{
    ui->studentsTable->blockSignals(true);
    ui->studentsTable->clear();
    ui->studentsTable->setColumnCount(5);
    QStringList studentHeaders;
    studentHeaders << "Surname" << "Name" << "Patronymic" << "Instrument" << "Teacher";
    ui->studentsTable->setHorizontalHeaderLabels(studentHeaders);
    ui->studentsTable->setRowCount(students->getFullSize());
    for (int i = 0; i < students->getFullSize(); ++i) {
        if (students->isOccupied(i)) {
            const auto& st = students->getEntry(i);
            ui->studentsTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(st.fio.surname)));
            ui->studentsTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(st.fio.name)));
            ui->studentsTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(st.fio.patronymic)));
            ui->studentsTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(st.instrument)));
            ui->studentsTable->setItem(
                i,
                4,
                new QTableWidgetItem(QString::fromStdString(st.teacher.surname + " " +
                                                st.teacher.initials)));
        }
    }
    ui->studentsTable->blockSignals(false);

    std::vector<Concerts_entry> list;
    concerts->toVector(list);
    ui->concertsTable->blockSignals(true);
    ui->concertsTable->clear();
    ui->concertsTable->setColumnCount(4);
    QStringList headers;
    headers << "FIO" << "Play" << "Hall" << "Date";
    ui->concertsTable->setHorizontalHeaderLabels(headers);
    ui->concertsTable->setRowCount(static_cast<int>(list.size()));
    for (int i = 0; i < list.size(); ++i) {
        const auto& e = list[i];
        QString fio = QString::fromStdString(e.fio.surname + " " + e.fio.name + " " + e.fio.patronymic);
        ui->concertsTable->setItem(i, 0, new QTableWidgetItem(fio));
        ui->concertsTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(e.play)));
        ui->concertsTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(e.hall)));
        ui->concertsTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(e.date)));
    }
    ui->concertsTable->blockSignals(false);
    updateConcertTree();
}

void MainWindow::addStudent()
{
    Students_entry se;
    if (studentDialog(se)) {
        students->insert(se);
        refreshTables();
    }
}

void MainWindow::removeStudent()
{
    FIO f;
    if (!fioDialog(f, nullptr, "Remove Student"))
        return;
    students->remove(f);
    refreshTables();
}

void MainWindow::editStudent()
{
    int row = ui->studentsTable->currentRow();
    if (row < 0 || !ui->studentsTable->item(row, 0)) {
        QMessageBox::warning(this, "Edit Student", "Select a student in the table.");
        return;
    }

    Students_entry oldEntry = students->getEntry(row);
    Students_entry newEntry = oldEntry;
    if (!studentDialog(newEntry, &oldEntry))
        return;

    students->remove(oldEntry.fio);
    students->insert(newEntry);
    refreshTables();
}

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
    if (!fioDialog(f, nullptr, "Remove Concert"))
        return;
    concerts->remove(f);
    refreshTables();
}

void MainWindow::editConcert()
{
    int row = ui->concertsTable->currentRow();
    std::vector<Concerts_entry> list;
    concerts->toVector(list);
    if (row < 0 || row >= list.size()) {
        QMessageBox::warning(this, "Edit Concert", "Select a concert in the table.");
        return;
    }

    Concerts_entry oldEntry = list[row];
    Concerts_entry newEntry = oldEntry;
    if (!concertDialog(newEntry, &oldEntry))
        return;

    concerts->remove(oldEntry.fio);
    concerts->insert(newEntry);
    refreshTables();
}

void MainWindow::searchStudent()
{
    QStringList types;
    types << "Name" << "Instrument";
    bool ok = false;
    QString type = QInputDialog::getItem(this, "Search Student", "Search by:", types, 0, false, &ok);
    if (!ok || type.isEmpty())
        return;
    QString query = QInputDialog::getText(this, "Search Student", "Value:", QLineEdit::Normal, QString(), &ok);
    if (!ok)
        return;
    std::vector<Students_entry> results;
    if (type == "Name")
        results = students->searchByName(query.toStdString());
    else
        results = students->searchByInstrument(query.toStdString());

    QString text;
    for (const auto& st : results)
        text += QString::fromStdString(st.fio.surname + " " + st.fio.name + " " + st.fio.patronymic +
                                       " - " + st.instrument + " (" + st.teacher.surname + " " + st.teacher.initials + ")\n");
    if (text.isEmpty())
        text = "Not found";
    QMessageBox::information(this, "Results", text);
}

void MainWindow::searchConcert()
{
    QStringList types;
    types << "Date" << "Hall";
    bool ok = false;
    QString type = QInputDialog::getItem(this, "Search Concert", "Search by:", types, 0, false, &ok);
    if (!ok || type.isEmpty())
        return;
    QString query = QInputDialog::getText(this, "Search Concert", "Value:", QLineEdit::Normal, QString(), &ok);
    if (!ok)
        return;
    std::vector<Concerts_entry> results;
    if (type == "Date")
        results = concerts->searchByDate(query.toStdString());
    else
        results = concerts->searchByHall(query.toStdString());

    QString text;
    for (const auto& e : results)
        text += QString::fromStdString(e.fio.surname + " " + e.fio.name + " " + e.fio.patronymic +
                                       " - " + e.play + " - " + e.hall + " - " + e.date + "\n");
    if (text.isEmpty())
        text = "Not found";
    QMessageBox::information(this, "Results", text);
}

bool MainWindow::studentDialog(Students_entry& out, const Students_entry* initial)
{
    QDialog dialog(this);
    dialog.setWindowTitle(initial ? "Edit Student" : "Add Student");
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

    layout.addRow("Surname", &surname);
    layout.addRow("Name", &name);
    layout.addRow("Patronymic", &patronymic);
    layout.addRow("Instrument", &instrument);
    layout.addRow("Teacher surname", &teacherSurname);
    layout.addRow("Teacher initials", &teacherInitials);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout.addRow(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        if (!validateFIO(surname.text(), name.text(), patronymic.text())) {
            QMessageBox::warning(this, "Input Error", "Invalid FIO");
            return false;
        }
        if (!validateInstrument(instrument.text())) {
            QMessageBox::warning(this, "Input Error", "Invalid instrument");
            return false;
        }
        if (!validateTeacher(teacherSurname.text(), teacherInitials.text())) {
            QMessageBox::warning(this, "Input Error", "Invalid teacher initials");
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
    dialog.setWindowTitle(initial ? "Edit Concert" : "Add Concert");
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

    layout.addRow("Surname", &surname);
    layout.addRow("Name", &name);
    layout.addRow("Patronymic", &patronymic);
    layout.addRow("Play", &play);
    layout.addRow("Hall", &hall);
    layout.addRow("Date", &date);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout.addRow(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        if (!validateFIO(surname.text(), name.text(), patronymic.text())) {
            QMessageBox::warning(this, "Input Error", "Invalid FIO");
            return false;
        }
        if (!validatePlay(play.text())) {
            QMessageBox::warning(this, "Input Error", "Invalid play");
            return false;
        }
        if (!validateHall(hall.text())) {
            QMessageBox::warning(this, "Input Error", "Invalid hall");
            return false;
        }
        if (!validateDate(date.text())) {
            QMessageBox::warning(this, "Input Error", "Invalid date");
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

void MainWindow::updateConcertTree()
{
    int row = ui->concertsTable->currentRow();
    std::vector<Concerts_entry> list;
    concerts->toVector(list);
    FIO* highlight = nullptr;
    FIO temp;
    if (row >= 0 && row < list.size()) {
        temp = list[row].fio;
        highlight = &temp;
    }

    concerts->buildTreeWidget(ui->concertTree, highlight);
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

    layout.addRow("Surname", &surname);
    layout.addRow("Name", &name);
    layout.addRow("Patronymic", &patronymic);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout.addRow(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        if (!validateFIO(surname.text(), name.text(), patronymic.text())) {
            QMessageBox::warning(this, "Input Error", "Invalid FIO");
            return false;
        }
        out.surname = surname.text().toStdString();
        out.name = name.text().toStdString();
        out.patronymic = patronymic.text().toStdString();
        return true;
    }
    return false;
}

void MainWindow::studentCellChanged(int row, int column)
{
    if (row < 0 || row >= students->getFullSize())
        return;
    if (!students->isOccupied(row))
        return;

    Students_entry oldEntry = students->getEntry(row);
    Students_entry newEntry = oldEntry;

    auto getText = [&](int col, const std::string& oldVal) {
        QTableWidgetItem* item = ui->studentsTable->item(row, col);
        return item ? item->text().toStdString() : oldVal;
    };

    newEntry.fio.surname = getText(0, oldEntry.fio.surname);
    newEntry.fio.name = getText(1, oldEntry.fio.name);
    newEntry.fio.patronymic = getText(2, oldEntry.fio.patronymic);
    newEntry.instrument = getText(3, oldEntry.instrument);
    std::string teacher = getText(4, oldEntry.teacher.surname + " " + oldEntry.teacher.initials);
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

    if (!validateFIO(QString::fromStdString(newEntry.fio.surname),
                     QString::fromStdString(newEntry.fio.name),
                     QString::fromStdString(newEntry.fio.patronymic)) ||
        !validateInstrument(QString::fromStdString(newEntry.instrument)) ||
        !validateTeacher(QString::fromStdString(newEntry.teacher.surname),
                         QString::fromStdString(newEntry.teacher.initials))) {
        QMessageBox::warning(this, "Input Error", "Invalid student data");
        refreshTables();
        return;
    }

    students->remove(oldEntry.fio);
    students->insert(newEntry);
    refreshTables();
    ui->studentsTable->setCurrentCell(row, column);
}

void MainWindow::concertCellChanged(int row, int column)
{
    std::vector<Concerts_entry> list;
    concerts->toVector(list);
    if (row < 0 || row >= static_cast<int>(list.size()))
        return;

    Concerts_entry oldEntry = list[row];
    Concerts_entry newEntry = oldEntry;

    auto getText = [&](int col, const std::string& oldVal) {
        QTableWidgetItem* item = ui->concertsTable->item(row, col);
        return item ? item->text().toStdString() : oldVal;
    };

    std::string fio = getText(0, oldEntry.fio.surname + " " + oldEntry.fio.name + " " + oldEntry.fio.patronymic);
    std::istringstream ss(fio);
    ss >> newEntry.fio.surname >> newEntry.fio.name >> newEntry.fio.patronymic;
    newEntry.play = getText(1, oldEntry.play);
    newEntry.hall = getText(2, oldEntry.hall);
    newEntry.date = getText(3, oldEntry.date);

    if (newEntry.fio == oldEntry.fio && newEntry.play == oldEntry.play &&
        newEntry.hall == oldEntry.hall && newEntry.date == oldEntry.date)
        return;

    if (!validateFIO(QString::fromStdString(newEntry.fio.surname),
                     QString::fromStdString(newEntry.fio.name),
                     QString::fromStdString(newEntry.fio.patronymic)) ||
        !validatePlay(QString::fromStdString(newEntry.play)) ||
        !validateHall(QString::fromStdString(newEntry.hall)) ||
        !validateDate(QString::fromStdString(newEntry.date))) {
        QMessageBox::warning(this, "Input Error", "Invalid concert data");
        refreshTables();
        return;
    }

    concerts->remove(oldEntry.fio);
    concerts->insert(newEntry);
    refreshTables();
    ui->concertsTable->setCurrentCell(row, column);
}

bool MainWindow::validateFIO(const QString& s, const QString& n, const QString& p) const
{
    QRegularExpression word("^[А-ЯЁ][а-яё]+$");
    return word.match(s).hasMatch() && word.match(n).hasMatch() && word.match(p).hasMatch();
}

bool MainWindow::validateInstrument(const QString& instrument) const
{
    QRegularExpression pat("^[А-Яа-яЁё]+( [А-Яа-яЁё]+)*$");
    return pat.match(instrument).hasMatch();
}

bool MainWindow::validateTeacher(const QString& surname, const QString& initials) const
{
    QRegularExpression sPat("^[А-ЯЁ][а-яё]+$");
    QRegularExpression iPat("^[А-ЯЁ]\\.[А-ЯЁ]\\.$");
    return sPat.match(surname).hasMatch() && iPat.match(initials).hasMatch();
}

bool MainWindow::validatePlay(const QString& play) const
{
    QRegularExpression pat("^\".*\"$");
    return pat.match(play).hasMatch();
}

bool MainWindow::validateHall(const QString& hall) const
{
    QRegularExpression pat("^[А-Яа-яЁё]+ зал$");
    return pat.match(hall).hasMatch();
}

bool MainWindow::validateDate(const QString& date) const
{
    QRegularExpression pat("^(0[1-9]|[12][0-9]|3[01])\\.(0[1-9]|1[0-2])\\.[0-9]{4}$");
    return pat.match(date).hasMatch();
}

