#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDialogButtonBox>

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
    refreshTables();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::refreshTables()
{
    ui->studentsTable->clear();
    ui->studentsTable->setColumnCount(1);
    ui->studentsTable->setRowCount(students->getFullSize());
    for (int i = 0; i < students->getFullSize(); ++i) {
        if (students->isOccupied(i)) {
            QString text = QString::fromStdString(students->entryToString(i));
            ui->studentsTable->setItem(i, 0, new QTableWidgetItem(text));
        }
    }

    std::vector<Concerts_entry> list;
    concerts->toVector(list);
    ui->concertsTable->clear();
    ui->concertsTable->setColumnCount(1);
    ui->concertsTable->setRowCount(static_cast<int>(list.size()));
    for (int i = 0; i < list.size(); ++i) {
        const auto& e = list[i];
        QString text = QString::fromStdString(e.fio.surname + " " + e.fio.name + " " +
                                               e.fio.patronymic + " - " + e.play +
                                               " - " + e.hall + " - " + e.date);
        ui->concertsTable->setItem(i, 0, new QTableWidgetItem(text));
    }
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
    f.surname = QInputDialog::getText(this, "Surname", "Surname").toStdString();
    f.name = QInputDialog::getText(this, "Name", "Name").toStdString();
    f.patronymic = QInputDialog::getText(this, "Patronymic", "Patronymic").toStdString();
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
    f.surname = QInputDialog::getText(this, "Surname", "Surname").toStdString();
    f.name = QInputDialog::getText(this, "Name", "Name").toStdString();
    f.patronymic = QInputDialog::getText(this, "Patronymic", "Patronymic").toStdString();
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

